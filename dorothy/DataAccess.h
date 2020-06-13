/**
     *  DataAccess.h
     *  Singleton class for storing data of ticker price data in internal memory.
     *  Cannot have multiple instances since it can use a lot of memory.
     *
     *
     *  Created by Salil Maharjan on 4/29/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#ifndef DataAccess_h
#define DataAccess_h

#include "PrefixHeader.pch"
#include "DateTime.h"
#include "TickerData.h"

class DataAccess
{
public:

    // *********************************************************
    // *********************** ACCESSORS ***********************
    // *********************************************************

    /// Get constituent names in the universe.
    const std::vector<std::string> GetUniverse(){ return m_tickerNames; }

    /// Get trading dates
    const std::vector<DateTime*> GetTradingDates(){ return m_tradingDates; }

    /// Get oldest trading date for which data is available
    DateTime* GetOldestTradingDate(){ return m_tradingDates[m_tradingDates.size()-1]; }

    /// Get most recent trading date for which data is available
    DateTime* GetLatestTradingDate(){ return m_tradingDates[0]; }

    /// Get trading dates from "a_from" to "a_to"
    std::vector<DateTime*> GetTradingDatesInRange( DateTime* a_from, DateTime* a_to);

    /// Get TickerData Data
    TickerData* GetTickerData(std::string a_ticker);

    // *********************************************************
    // ***************** PUBLIC CLASS METHODS ******************
    // *********************************************************

    /// Static access method to create a singleton DataAccess object.
    static DataAccess* GetDbInstance( const std::string a_directory, const std::string a_universe );

    /// Destructor
    ~DataAccess() = default;

    /// Check date range with available data's date range
    bool CheckDateRange( DateTime* a_startDate, DateTime* a_endDate );


private:

    // *********************************************************
    // ******************** CLASS VARIABLES ********************
    // *********************************************************

    /// TickerData for referencing trading dates. Uses IBM.
    const static std::string m_tradingDatesRefTicker;
    /// Single DataAccess instance var
    static DataAccess* m_dataInstance;
    /// TickerData names specified in the universe
    static std::vector<std::string> m_tickerNames;
    /// Trading dates loaded from reference ticker.
    static std::vector<DateTime*> m_tradingDates;
    /// Map variable ( TickerData Name : TickerData Data )
    static std::map<std::string, TickerData*> m_tickerData;

    // *********************************************************
    // ********************* CONSTRUCTORS **********************
    // *********************************************************

    /// Private constructors to prevent multiple instancing:
    DataAccess(){};
    /// Main class parameterized constructor
    DataAccess( const std::string &a_directory, const std::string &a_universe );

    /// Private copy constructor
    DataAccess(DataAccess const&){};
    /// Delete assignment operator
    DataAccess& operator = (DataAccess const&) = delete;

    // *********************************************************
    // *************** PRIVATE UTILITY METHODS *****************
    // *********************************************************

    /// Method to load ticker names from the constituent universe file
    void loadTickerNames( const std::string &a_universe );
    /// Method to load price data for loaded tickers
    void loadTickerData( const std::string &a_directory );
    /// Method to load trading dates from the refence ticker, IBM.
    void loadTradingDates( const std::string &a_directory );
};


#endif /* DataAccess_h */
