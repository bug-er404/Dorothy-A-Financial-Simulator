/**
     *  TradingStock.h
     *  Class that handles the investment portfolio of a trading stock in the simulator.
     *  Handles trading details for each ticker used in the simulator.
     *
     *
     *  Created by Salil Maharjan on 05/03/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#ifndef DOROTHY_TRADINGSTOCK_H
#define DOROTHY_TRADINGSTOCK_H

#include "PrefixHeader.pch"
#include "DateTime.h"
#include "Transaction.h"

class TradingStock
{
public:

    // *********************************************************
    // ********************* CONSTRUCTORS **********************
    // *********************************************************

    /// Class parameterized constructor
    TradingStock (std::string a_tickerName, double a_startingCapital);

    // *********************************************************
    // *********************** ACCESSORS ***********************
    // *********************************************************

    const std::string GetStockName() { return m_tickerName; }
    const bool IsLong() { return m_longPosFlag; }
    const bool IsShort() { return m_shortPosFlag; }
    const std::vector<double> GetSignalRecord(){ return m_signalActivityRecord; }
    const double GetTotalShares() { return m_totalSharesCount; }
    const double GetAvailableCapital() { return m_availableCapital; }
    const double GetDailyPnL() { return m_dailyPNLRecord.at(m_dailyPNLRecord.size()-1);}
    const double GetDailyROR() { return m_dailyReturnRecord.at(m_dailyReturnRecord.size()-1); }
    // Accessors to use to calculate individual trading object's Sharpe Ratio
    const std::vector<double> GetROR() { return m_dailyReturnRecord; }
    const double GetLatestShareActivity() { return m_shareActivityRecord.at(m_shareActivityRecord.size()-1); }
    const double GetSharpeToday();
    /// Method to get current completed transaction count (Does not consider incomplete transactions.)
    double GetTransactionCount();
    /// Method to get short shares held
    const double GetShortSharesHeld();
    /// Method to get long shares held
    const double GetLongSharesHeld();
    /// Method to get closed transactions and remove them from memory
    std::vector<Transaction*> GetClosedTransactions();
    /// Method to get short capital in investment
    const double GetShortCapital();
    /// Method to get long capital in investment
    const double GetLongCapital();


    // *********************************************************
    // *********************** MUTATORS ************************
    // *********************************************************

    void SetLong( bool a_long ) { m_longPosFlag = a_long; }
    void SetShort( bool a_short) { m_shortPosFlag = a_short; }
    void IncreaseSharesBy( double a_share );
    void DecreaseSharesBy( double a_share );
    void IncreaseCapitalBy( double a_share );
    void DecreaseCapitalBy( double a_share);


    // *********************************************************
    // ***************** PUBLIC CLASS METHODS ******************
    // *********************************************************

    /// Method to sell shares
    void SellShares( double a_shares, double a_price );

    /// Method to buys shares
    void BuyShares( double a_shares, double a_price );

    /// Method to close positions that exceed maximum days limit
    void CloseStalePositions( int a_dayLimit, DateTime* a_date, double a_signal, double a_price );

    /// Method to open a position for the trading stock
    void OpenPosition(DateTime* a_date, double a_signal, double a_share, double a_price);

    /// Method to close all long positions
    double CloseLongPositions(DateTime* a_date, double a_signal, double a_price, double &a_investedCaptial, double &a_numOfShares);

    /// Method to close all short positions
    double CloseShortPositions(DateTime* a_date, double a_signal, double a_price, double &a_investedCaptial, double &a_numOfShares);

    /// Method to check if signal is profitable for short positions.
    bool CheckShortSignal( double a_price );

    /// Method to check if signal is profitable for long positions.
    bool CheckLongSignal( double a_price );

    /// Method to update long/short position status of the trading stock.
    void UpdateStockPosition();

    // Methods to update daily records:
    /// Record day's signal
    void RecordSignal( double a_signal );
    /// Record day's PNL
    void RecordPNL( double a_pnl, double a_investedCapital);
    /// Record day's number of transactions
    void RecordTransactionsCount( int a_count);
    /// Recard day's daily return
    void RecordDailyReturn( double a_amount );

    // Methods to calculate individual stock's sharpe ratio:
    /// Get latest committed capital in an investment
    double GetInvestedCapital();
    /// Calculate daily sharpe ratio for individual stock
    void CalculateDailySharpeRatio();

private:
    /// Private default constructor
    TradingStock()=default;

    // *********************************************************
    // ********************* CLASS VARIABLES *******************
    // *********************************************************

    /// Ticker name
    std::string m_tickerName;
    /// Available capital for trading stock
    double m_availableCapital;
    /// Total transactions count
    int m_transactionsTotalCount;
    /// Current complete transaction counter
    int m_transactionCounter;
    /// Total shares held for stock
    double m_totalSharesCount;
    /// Long position flag
    bool m_longPosFlag;
    /// Short position flag
    bool m_shortPosFlag;


    // *********************************************************
    // ********************* RECORD VARIABLES ******************
    // *********************************************************

    /// Records all transactions
    std::vector<Transaction*> m_transactions;
    /// Records daily signals
    std::vector<double> m_signalActivityRecord;
    /// Records daily capital activities
    std::vector<double> m_capitalActivityRecord;
    /// Records daily position changes
    std::vector<double> m_shareActivityRecord;
    /// Records daily PNL
    std::vector<double> m_dailyPNLRecord;
    /// Records daily rate of return (ROR)
    std::vector<double> m_dailyReturnRecord;
    /// Records daily cumulative PNL
    std::vector<double> m_cumulativePNLRecord;
    /// Records daily transaction counts
    std::vector<int> m_TransactionsCountRecord;
    /// Records daily sharpe ratio
    std::vector<double> m_sharpeRatioRecord;

    // *********************************************************
    // *************** PRIVATE UTILITY METHODS *****************
    // *********************************************************

    /// Method to update days in position of all held shares
    void newTradingDay();
};


#endif //DOROTHY_TRADINGSTOCK_H
