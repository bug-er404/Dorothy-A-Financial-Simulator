/**
     *  TickerData.h
     *  Handles ticker data for the trading stocks.
     *
     *
     *  Created by Salil Maharjan on 4/30/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#ifndef Ticker_h
#define Ticker_h

#include "PrefixHeader.pch"
#include "DateTime.h"

// *********************************************************
// ******************* TICKER DATA FIELDS ******************
// *********************************************************

/// ENUM of Ticker fields as found in the Ticker data source file.
/// Includes computed fields used in the simulation
enum TICKER_FIELDS
{
    // START OF ALL TICKER FIELDS

    // DATA FIELDS:
    OPEN,
    START_TICKER_FIELDS = OPEN, // Reference to start of all fields
    DATA_FIELD_START = OPEN,    // Reference to start of data fields
    HIGH,
    LOW,
    CLOSE,
    VOLUME,
    ADJ_CLOSE,
    DIVIDEND,
    SPLIT,
    VWAP,
    SHARE_OUTSTANDING,
    DATA_FIELD_END = SHARE_OUTSTANDING, // Reference to end of data fields
    // * END OF DATA FIELDS *

    // CALCULATED FIELDS:
    // MACD FIELDS
    FAST_EMA,   // 12day EMA
    SLOW_EMA,   // 26day EMA
    MACD_LINE,   // FAST_EMA-SLOW_EMA
    SIG_LINE,    // 9day EMA
    MACD_HIST,  // MACD_LINE - SIG_EMA (MACD HISTOGRAM)
    // * END OF CALCULATED FIELDS *

    END_TICKER_FIELDS
    // * END OF ALL TICKER FIELDS *
};

class TickerData
{
public:
    // *********************************************************
    // ******************** CLASS CONSTANTS ********************
    // *********************************************************

    /// Fill constant for unavailable data
    const static int NA_VAL;

    // *********************************************************
    // ********************* CONSTRUCTORS **********************
    // *********************************************************

    /// Parameterized constructor to create a ticker object
    TickerData(std::string &a_tickerName, const std::string &a_directory );

    // *********************************************************
    // *********************** ACCESSORS ***********************
    // *********************************************************

    /// Get today's data of a_field
    double GetCurrentDataOf( TICKER_FIELDS a_field ) { return m_tickerData[a_field].at(m_tradingDateIndexStart+1); }

    /// Get yesterday's close price
    double GetCloseYesterday() { return m_tickerData[TICKER_FIELDS::CLOSE].at(m_tradingDateIndexStart+2); }

    // *********************************************************
    // ***************** PUBLIC CLASS METHODS ******************
    // *********************************************************

    /// Method to set trading date range.
    void SetTradingDateRange( DateTime* a_startDate, DateTime* a_endDate);

    /// Get current date's MACD signal.
    double GetMACDSignal();

    /// Default deconstructor
    ~TickerData()=default;

    // *********************************************************
    // ******************* DEBUGGING METHODS *******************
    // *********************************************************

    /// Print head of data columns.
    void PrintDataHead();

    /// Print sample of all columns including data and calculated columns.
    void PrintAllColumns();


private:
    /// Default constructor.
    TickerData()=default;

    // *********************************************************
    // ******************** CLASS VARIABLES ********************
    // *********************************************************

    /// Stock ticker name
    std::string m_tickerName;
    /// TickerData data for a stock in memory
    std::vector<double> m_tickerData[TICKER_FIELDS::END_TICKER_FIELDS];
    /// TickerData dates of the price data
    std::vector<DateTime*> m_tickerDates;
    /// Trading start date index in m_tickerDates
    int m_tradingDateIndexStart;
    /// Trading end date index in m_tickerDates
    int m_tradingDateIndexEnd;

    // *********************************************************
    // *************** PRIVATE UTILITY METHODS *****************
    // *********************************************************

    /// Parse ticker data from data source files.
    void parseTickerData( const std::string &a_directory );

    /// Calculate MACD fields and append to the ticker data.
    void calculateMACDfields();

};
#endif /* Ticker_h */
