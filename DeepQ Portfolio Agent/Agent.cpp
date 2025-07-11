#include "Agent.h"
#include <random>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

DQAgent::DQAgent(Hyperparams params) : params(params), update_counter(0)
{
    this->BuildModel();
}

void DQAgent::setEpsilon(double epsilon)
{
    params.epsilon = epsilon;
}

void DQAgent::BuildModel()
{
    // Define online model
    auto online_model = torch::nn::Sequential(
        torch::nn::Linear(params.state_size, params.h1),
        torch::nn::ReLU(),
        torch::nn::Linear(params.h1, params.action_size)
    );

    // Define target model (same architecture)
    auto target_model = torch::nn::Sequential(
        torch::nn::Linear(params.state_size, params.h1),
        torch::nn::ReLU(),
        torch::nn::Linear(params.h1, params.action_size)
    );

    auto loss_function = torch::nn::MSELoss();
    this->online_model = online_model;
    this->target_model = target_model;
    this->loss_function = loss_function;
    optimizer_ptr = std::make_unique<torch::optim::Adam>(online_model->parameters(), torch::optim::AdamOptions(params.learning_rate));

    // Initialize target model with online model weights
    UpdateTargetModel();

    // Register the models as submodules
    register_module("online_model", online_model);
    register_module("target_model", target_model);
}

void DQAgent::UpdateTargetModel()
{
    // Copy weights from online model to target model
    torch::NoGradGuard no_grad;
    for (size_t i = 0; i < target_model->parameters().size(); i++) {
        target_model->parameters()[i].copy_(online_model->parameters()[i]);
    }
}

void DQAgent::Fit(torch::Tensor& state, torch::Tensor& target, int epoch)
{
    for (int i = 0; i < epoch; ++i)
    {
        online_model->train();

        optimizer_ptr->zero_grad();
        auto output = online_model->forward(state);
        auto loss = loss_function(output, target);

        loss.backward();
        optimizer_ptr->step();
    }
}

void DQAgent::Remember(env_result eps_result)
{
    params.memory.push_front(eps_result);
    if (params.memory.size() > params.memory_capacity) 
    {
        params.memory.pop_back();
    }
}

const action_amount DQAgent::Act(std::vector<double> state, Asset& asset)
{
    action_amount agent_action;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double random_value = dis(gen);

    if (random_value <= params.epsilon)
    {
        std::uniform_int_distribution<> dis_action(0, params.action_size - 1);
        std::uniform_real_distribution<> dis_amount(0, 100.0);
        agent_action.action = dis_action(gen);
        agent_action.amount = dis_amount(gen);
    }
    else
    {
        online_model->eval();
        torch::Tensor state_tensor = torch::tensor(state).unsqueeze(0);
        torch::Tensor model_output = online_model->forward(state_tensor);
        int best_action = model_output.argmax(1).item<int>();
        double q_value = model_output[0][best_action].item<double>();
        
        if (best_action == 1) //buy
        {
            double shares_avaliable = asset.getUnspentCash() / asset.getCurrentPricePerShare();
            agent_action.amount = std::min(q_value, shares_avaliable);
        }
        else if (best_action == 2)//sell
        {
            agent_action.amount = std::min(q_value, asset.getNumberOfSharesOwned());
        }
        else if (best_action == 0) //hold
        {
            agent_action.amount = 0;
        }
        
        agent_action.action = best_action;
        //Determines the amount of shares to buy/sell depending on the Q-val of the state

    }

    return agent_action;
}


void DQAgent::Replay(int batch_size) {
    if (params.memory.size() < batch_size) {
        return;
    }

    // Sample random indices
    std::vector<int> indices(batch_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, params.memory.size() - 1);
    std::generate(indices.begin(), indices.end(), [&]() { return dis(gen); });

    // Prepare batch tensors
    std::vector<torch::Tensor> states, next_states, targets;
    std::vector<int> actions;
    std::vector<bool> dones;

    // Collect batch data
    for (int idx : indices) {
        const auto& experience = params.memory[idx];
        states.push_back(torch::tensor(experience.state));
        next_states.push_back(torch::tensor(experience.next_state));
        actions.push_back(experience.action.action);
        dones.push_back(experience.done);
    }

    // Convert to batch tensors
    auto states_tensor = torch::stack(states);
    auto next_states_tensor = torch::stack(next_states);
    auto actions_tensor = torch::tensor(actions);
    std::vector<int64_t> dones_int(dones.begin(), dones.end());
    auto dones_tensor = torch::tensor(dones_int, torch::kBool);

    // Compute current Q values
    auto current_q = online_model->forward(states_tensor);

    // Compute target Q values
    target_model->eval();
    auto next_q_values = target_model->forward(next_states_tensor);
    auto max_next_q = std::get<0>(next_q_values.max(1));
    auto rewards_tensor = torch::tensor(
        [&]() {
            std::vector<double> rewards;
            for (int idx : indices) {
                rewards.push_back(params.memory[idx].reward);
            }
            return rewards;
        }()
            );

    auto targets_tensor = current_q.clone();
    auto update_values = rewards_tensor + params.gamma * max_next_q * (~dones_tensor).to(torch::kFloat32);

    // Update only the taken actions
    targets_tensor.index_put_({ torch::arange(batch_size), actions_tensor }, update_values);

    // Train on batch
    this->Fit(states_tensor, targets_tensor, 1);

    // Update target network and decay epsilon
    if (params.epsilon > 0) //only during training we update target
    {
        if (++update_counter % params.target_update_freq == 0) {
            UpdateTargetModel();
            update_counter = 0;
        }

        if (params.epsilon > params.epsilon_min) {
            params.epsilon *= params.epsilon_decay;
        }
    }
    
}

void DQAgent::SaveModel(const std::string& filename) {
    fs::path save_path = fs::path("../../Saved_Models/") / (filename + ".pt");
    fs::create_directories(save_path.parent_path());

    //Copies weight of online model to target before saving 
    this->UpdateTargetModel();

    // Save both models
    torch::save(online_model, save_path.string() + "_online");
    torch::save(target_model, save_path.string() + "_target");
    std::cout << "Models saved to: " << save_path << "_online and _target" << std::endl;
}

// Alternative version if you need to rebuild architecture
void DQAgent::LoadModel(const std::string& filename) {
    fs::path load_path = fs::path("../../Saved_Models/") / (filename + ".pt");

    std::string online_path = load_path.string() + "_online";
    std::string target_path = load_path.string() + "_target";

    // Check if model files exist
    if (!fs::exists(online_path) || !fs::exists(target_path)) {
        throw std::runtime_error("Model files not found: " + load_path.string() + "_online or _target");
    }

    try {
        // Method 1: Load directly (recommended)
        torch::load(online_model, online_path);
        torch::load(target_model, target_path);

    }
    catch (const std::exception& e) {
        std::cerr << "Direct loading failed: " << e.what() << std::endl;
        std::cout << "Trying state_dict loading..." << std::endl;

        // Method 2: Rebuild models and load state_dict
        this->BuildModel();

        // Load state dictionaries
        torch::serialize::InputArchive online_archive;
        online_archive.load_from(online_path);
        online_model->load(online_archive);

        torch::serialize::InputArchive target_archive;
        target_archive.load_from(target_path);
        target_model->load(target_archive);
    }

    //std::cout << "Models loaded successfully from: " << load_path << "_online and _target" << std::endl;
}

const Hyperparams& DQAgent::getParams()
{
    return params;
}