/**
     *  DataAccess.cpp
     *  Implementation of DataAccess.h
     *
     *
     *  Created by Salil Maharjan on 4/29/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#include "DataAccess.h"


// Member variables declaration:
DataAccess* DataAccess::m_dataInstance = 0;
std::vector<std::string> DataAccess::m_tickerNames;
std::vector<DateTime*> DataAccess::m_tradingDates;
std::map<std::string, TickerData*> DataAccess::m_tickerData;
// Set reference ticker to IBM
const std::string DataAccess::m_tradingDatesRefTicker = "IBM";

// *********************************************************
// ********************* CONSTRUCTORS **********************
// *********************************************************

/**
    * DataAccess::DataAccess.
    * Constructor to create single instance of DataAccess.
    * Stores price data for the specified universe in memory.
    * @param a_directory string Price data directory path.
    * @param a_universe string Directory path for ticker name universe file.
    * @author Salil Maharjan
    * @date 4/29/20.
*/
DataAccess::DataAccess( const std::string &a_directory, const std::string &a_universe )
{
    loadTickerNames(a_universe);
    loadTickerData(a_directory);
}

// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************

/**
    * DataAccess::GetDbInstance.
    * Method to create a single instance of DataAccess and prevent multiple instances.
    * @param a_directory string Price data directory path.
    * @param a_universe string Directory path for file with ticker name universe.
    * @return DataAccess* Single instance of the path.
    * @author Salil Maharjan
    * @date 4/29/20.
*/
DataAccess* DataAccess::GetDbInstance( const std::string a_directory, const std::string a_universe )
{
    if(m_dataInstance == 0)
        m_dataInstance = new DataAccess(a_directory, a_universe);

    return m_dataInstance;
}

/**
    * DataAccess::CheckDateRange.
    * Check date range with available data's date range.
    * @param a_startDate DateTime* Starting date range.
    * @param a_endDate DateTime* End of date range.
    * @return bool If data is available for the date range.
    * @author Salil Maharjan
    * @date 4/29/20.
*/
bool DataAccess::CheckDateRange( DateTime* a_startDate, DateTime* a_endDate )
{
    return (*m_tradingDates[0]<=*a_startDate && *m_tradingDates[m_tradingDates.size()-1]>=*a_endDate);
}

/**
    * DataAccess::GetTradingDatesInRange.
    * Get trading dates from "a_from" to "a_to" according to reference ticker. IBM.
    * @param a_from DateTime* Starting date range.
    * @param a_to DateTime* End of date range.
    * @return vector<DateTime*> All trading dates in range.
    * @author Salil Maharjan
    * @date 4/29/20.
*/
std::vector<DateTime*> DataAccess::GetTradingDatesInRange( DateTime* a_from, DateTime* a_to)
{
    std::vector<DateTime*> t_tradeDates;

    for(int i=0; i<m_tradingDates.size(); i++)
    {
        if(*m_tradingDates[i]>=*a_from && *m_tradingDates[i]<=*a_to)
            t_tradeDates.insert(t_tradeDates.begin(), m_tradingDates[i]);
    }

    return t_tradeDates;
}

/**
    * DataAccess::GetTickerData.
    * Get ticker data for a_ticker.
    * @param a_ticker string The name of the ticker to get data.
    * @return TickerData* Price data of a_ticker
    * @author Salil Maharjan
    * @date 4/29/20.
*/
TickerData* DataAccess::GetTickerData(std::string a_ticker)
{
    if(m_tickerData.find(a_ticker)!=m_tickerData.end())
        return m_tickerData[a_ticker];
    else
        return 0;
}

// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

/**
* DataAccess::loadTickerNames.
    * Method to load ticker names in the universe.
    * @param a_universe string Directory path for ticker name universe file.
    * @author Salil Maharjan
    * @date 4/29/20.
*/
void DataAccess::loadTickerNames( const std::string &a_universe )
{
    std::ifstream t_ins(a_universe);
    std::string t_buffer;

    // Validate directory
    if (!t_ins)
    {
        std::cerr <<"Dorothy Error (LoadTickerNames): Unable to load TickerData Names from directory ["<<a_universe<<"]"<<std::endl;
        exit(1);
    }

    // Record ticker names
    while(!t_ins.eof())
    {
        getline(t_ins, t_buffer);
        if (t_buffer.empty() | t_buffer==" ")
            continue;

        m_tickerNames.push_back(t_buffer);
    }
}

/**
* DataAccess::loadTradingDates.
    * Method to load trading dates from the refence ticker, IBM.
    * @param a_directory string Price data directory path.
    * @author Salil Maharjan
    * @date 4/29/20.
*/
void DataAccess::loadTradingDates( const std::string &a_directory )
{
    std::ifstream t_ins(a_directory);
    std::string t_buffer;
    if(!t_ins)
    {
        std::cerr <<"Dorothy Error (LoadTradingDates): Unable to load Trading Dates from reference ticker "
                  <<m_tradingDatesRefTicker<<" in directory: "<< "["<<a_directory<<"]"<<std::endl;
        exit(1);
    }

    // Skip ticker title and column headers
    getline(t_ins, t_buffer);
    getline(t_ins, t_buffer);

    // Load trading dates from first column and store in reverse chronological order
    while(getline(t_ins, t_buffer))
        m_tradingDates.push_back(new DateTime( stoi( t_buffer.substr(0, t_buffer.find(',')) ) ));
}


/**
    * DataAccess::loadTickerData.
    * Method to load price data for each ticker in the universe.
    * @param a_directory string Price data directory path.
    * @author Salil Maharjan
    * @date 4/29/20.
*/
void DataAccess::loadTickerData( const std::string &a_directory )
{
    // Load trading dates
    loadTradingDates(a_directory+"/" +m_tradingDatesRefTicker+"_.csv");

    std::cout << "Dorothy: Loading data for constituents in universe..." << std::endl << std::endl;
    std::cout<<"******************************************"<<std::endl;
    std::cout<<"Dorothy - Tickers Loaded: "<<std::endl;
    for (int i=0; i<m_tickerNames.size(); i++)
    {
        std::string t_ticker = m_tickerNames[i];
        std::string t_filePath = a_directory + "/" + t_ticker + "_.csv";
        std::ifstream t_ins(t_filePath);

        // Validate data file. Continue with execution...
        if (t_ins.fail())
        {
            std::cerr << "Dorothy Error (LoadTickerData): Unable to load price data for ticker ["<<t_ticker<<"]"<<std::endl;
            std::cout<<"[ "<<(i+1)<<" ]  "<<t_ticker<< "(FAILED)"<<std::endl;
            // Remove ticker for the rest of the simulation
            m_tickerNames.erase(m_tickerNames.begin()+i);
            i--;
            continue;
        }

        m_tickerData[t_ticker] = new TickerData(t_ticker, t_filePath);
        // DEBUG: PRINT DATA HEADS
        std::cout<<"----------------------------------------------------------------------------------------------------------"<<std::endl;
        std::cout<<"[ "<<(i+1)<<" ]  "<<t_ticker<<std::endl;
        m_tickerData[t_ticker]->PrintDataHead();
//        m_tickerData[t_ticker]->PrintAllColumns();

    }
    std::cout<<"******************************************"<<std::endl;


    std::cout << "Dorothy: Loaded "<< m_tickerNames.size() << " tickers successfully."<< std::endl;
}


// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************
