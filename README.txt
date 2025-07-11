DeepQ-RL Agent Portfolio Optimization:

Goal:
This project aims to train an optimal policy for a reinforcement learning agent with the intent to buy,sell, or hold an asset. 

For the test asset AAPL the agent will make decisions based on the following states...
- GBM 1 day Log return forecast

- USA monthly CPI //Gauge consumer sentiment

- Daily gold price //Gauge market uncertainty

- average/variance of previous 20 closing price of asset //Gauge short term volatility

- Monthly unemployment rate

data source for economic indicators

https://fred.stlouisfed.org/tags/series?t=mei&ob=pv&od=desc&et=&pageID=4


Note: return forecast method "updateReturnForecast" and "access_data.cpp" functions are currently hardcoded to work for only initial 5 companies for sake of simplicity in early stage of project. 


Due to large noise in market due to economic policy, model will be trained and tuned with data from 1/12/15 to 1/12/24. Year 2025 will be used for final evaluation to gauge how well model responds to difficult market conditions and its ability to hedge uncertainty. 

I believe COVID data will help AGENT develop a strategy to navigate difficult conditions and hedge better. 



MODEL ASSUMES NO TRANSACTION FEES ON TRADES


MUST INCLUDE THE UNZIP LIBTORCH LIB FOLDER IN ./DeepQ Portfolio Agent/
path - This was excluded from the GIT COMMIT