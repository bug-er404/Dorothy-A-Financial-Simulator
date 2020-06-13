/**
     *  Simulator.h
     *  Main container class for the simulator. Uses member classes for financial simulation.
     *
     *
     *  Created by Salil Maharjan on 4/29/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#ifndef Simulator_h
#define Simulator_h

#include "PrefixHeader.pch"
#include "Config.h"
#include "DataAccess.h"
#include "TradingStock.h"

class Simulator
{
public:

    // *********************************************************
    // ********************* CONSTRUCTORS **********************
    // *********************************************************

    /// Main simulator constructor.
    Simulator( int argc, const char* argv[] );

    // ********************************************************
    // **************** PUBLIC CLASS METHODS  *****************
    // ********************************************************

    /// Interface method to start the simulation
    void StartSimulation();

    /// Main method to run simulation
    void RunSimulation();

private:

    /// Private default constructor
    Simulator()=default;

    // ********************************************************
    // ******** CONFIGURATION PARAMETER IDENTIFIERS  **********
    // ********************************************************

    /// # [DIRECTORY DATA]
    const std::string UNIVERSE_PATH_PARAM = "UNIVERSE_DIRECTORY";
    const std::string TICKER_PATH_PARAM = "TICKER_DATA_DIRECTORY";
    /// # [PORTFOLIO DATA]
    const std::string PORTFOLIO_CAPITAL = "PORTFOLIO_CAPITAL";
    const std::string MAX_CAPITAL_PER_STOCK_PARAM = "MAX_CAPITAL_PER_STOCK";
    /// # [SIMULATION PERIOD]
    const std::string PERIOD_START_DATE = "START_DATE";
    const std::string PERIOD_END_DATE = "END_DATE";
    /// # [SIGNAL THRESHOLDS]
    const std::string ENTRY_SIGNAL_PARAM = "ENTRY_THRESHOLD";
    const std::string EXIT_SIGNAL_PARAM = "EXIT_THRESHOLD";
    /// # [TRADING PARAMETERS]
    const std::string MAX_POS_PER_STOCK = "MAX_POSITIONS_PER_STOCK";
    const std::string DAYS_IN_POS_PARAM = "MAX_DAYS_IN_POSITION";
    const std::string STOP_LOSS_PARAM = "STOP_LOSS";
    /// # [PUBLICATION PARAMETERS]
    const std::string DAILY_PUB_PARAM = "DAILY";
    const std::string MONTHLY_PUB_PARAM = "MONTHLY";
    const std::string TRANSACTION_PUB_PARAM = "TRANSACTION";
    const std::string SIGNAL_PUB_PARAM = "SIGNAL";
    // TODO: (To include when considering portfolio publication)
//    const std::string PORTFOLIO_PUB_PARAM = "PORTFOLIO";


    // ********************************************************
    // ************** CONFIGURATION VARIABLES  ****************
    // ********************************************************

    /// Member variables from the configuration file
    /// # [DIRECTORY DATA]
    std::string m_universePath;
    std::string m_tickerPath;
    /// # [PORTFOLIO DATA]
    double m_portfolioCapital;
    double m_capitalLimitPerStock;
    /// # [SIGNAL THRESHOLDS]
    double m_entrySignal;
    double m_exitSignal;
    /// # [SIMULATION PERIOD]
    DateTime* m_startDate;
    DateTime* m_endDate;
    /// # [TRADING PARAMETERS]
    int m_maxPositionsPerStock;
    int m_maxDaysPerPosition;
    double m_stopLoss;
    /// # [PUBLICATION PARAMETERS]
    bool m_dailyPublication;
    bool m_monthlyPublication;
    bool m_transactionPublication;
    bool m_signalPublication;
    // TODO: (To include when considering portfolio publication)
//    bool m_portfolioPublication;


    // ********************************************************
    // ************ OUTPUT STREAMS FOR RECORDS  ***************
    // ********************************************************

    /// Output streams to write publications
    std::ofstream m_dailyInfoFile;
    std::ofstream m_monthlyInfoFile;
    std::ofstream m_transactionInfoFile;
    std::ofstream m_signalInfoFile;
    // TODO: (To include when considering portfolio publication)
//    std::ofstream m_portfolioInfoFile;


    // ********************************************************
    // *************** SIMULATION VARIABLES  ******************
    // ********************************************************

    /// Daily return. The profit for the day divided by the capital committed.
    double m_ROR;
    /// Cumulative PNL from the beginning of the simulation to the current date.
    double m_totalPNL;
    /// The profit or loss in dollars for the day.
    double m_dailyPNL;
    /// Daily long position counter variable.
    double m_longPosCount;
    /// Daily short position counter variable.
    double m_shortPosCount;
    /// Daily total position counter variable.
    double m_totalPosCount;
    /// Total amount of capital committed to the model at the end of the day.
    double m_netMarketValue;
    /// Long capital minus the short capital at the end of the day.
    double m_grossMarketValue;
    /// Date today
    DateTime* m_today;


    // ********************************************************
    // ************** DAILY RECORD VARIABLES  *****************
    // ********************************************************

    /// Daily PNL record.
    std::vector<double> m_dailyPNLRecord;
    /// Daily cumulative ROR record for all trading stocks in simulation.
    std::vector<double> m_dailyCumulativeROR;
    /// Sharpe ratio record.
    std::vector<double> m_sharpeRecord;


    // ********************************************************
    // *************** MEMBER CLASS VARIABLES  ****************
    // ********************************************************

    /// Number of config files passed
    int m_configFilesPassed;
    /// Count of how many config files have been simulated
    int m_simulationCount;
    /// Var with config file name
    std::vector<const char*> m_configFile;
    /// Configuration object
    Config* m_config;
    /// Data access object
    DataAccess* m_data;
    /// All the trading stocks in the simulation.
    std::vector<TradingStock*> m_tradingStocks;


    // ********************************************************
    // *************** PRIVATE UTILITY METHODS  ***************
    // ********************************************************

    /// Initializes the simulator by loading configurations, ticker data and trading stocks.
    void initializeSimulator( const char* a_configFile );

    /// Rerun simulator for consecutive configuration files
    void rerunSimulator( const char* a_configFile);

    /// Initialize file write stream for the publications requested in config
    void initializePublications();

    /// Checks if all required parameters are passed for simulator to run.
    bool checkParameters();

    /// Loads parameters required for the simulator
    void loadParameters();

    /// Initialize stocks that the simulator will be trading
    void initializeTradingTickers();

    /// Verify date range
    void verifyDateParameters();

    /// Calculate trade signal
    double calculateSignal( TickerData &a_stockData );

    /// Function to open or close positions according to the signal by checking current position.
    void makeTrade( TradingStock &a_stock, double &a_currentPrice, double &a_signalToday );

    /// Check if available capital for a stock is enough to purchase more equities at a_price
    bool hasEnoughCapital( TradingStock &a_stock, double a_price );

    /// Close positions for a trading stock
    void closePositions( TradingStock &a_stock, double &a_currentPrice, double &a_signal );

    /// Open positions for a trading stock
    void openPositions( TradingStock &a_stock, double &a_currentPrice, double &a_signal );

    /// Function to record transactions to a file.
    void recordTransaction( TradingStock &a_stock );

    /// Function to record signals to a file.
    void recordSignalInfo( double a_signal);

    /// Function to record daily statistics to a file.
    void recordDailyStatistics();

    /// Function to record monthly statistics to a file.
    void recordMonthlyStatistics();

    /// Method to close daily publication record file.
    void closeDailyPublication();

    /// Method to close monthly publication record file.
    void closeMonthlyPublication( bool a_flag );

    /// Method to close transaction publication record file.
    void closeTransactionPublication();

    /// Method to close signal publication record file.
    void closeSignalPublication();

    /// Function to record sharpe ratio to daily statistics file.
    void recordSharpeRatio();

    /// Method to record necessary elements for no activity (code reuse purposes).
    void recordNoActivity( TradingStock &a_stock );

    /// Function to calculate sharpe ratio
    double calculateSharpeRatio();

};

#endif /* Simulator_h */