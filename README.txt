DeepQ-RL Agent Asset Optimization:

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


MODEL ASSUMES NO TRANSACTION FEES ON TRADES


MUST INCLUDE THE UNZIP LIBTORCH LIB FOLDER IN ./DeepQ Portfolio Agent/
path - This was excluded from the GIT COMMIT
