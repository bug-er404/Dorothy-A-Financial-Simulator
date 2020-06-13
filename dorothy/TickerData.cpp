/**
     *  TickerData.cpp
     *  Implementation of TickerData.h
     *
     *
     *  Created by Salil Maharjan on 4/30/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#include "TickerData.h"
#include "Utilities.h"

// Static variable declaration
const int TickerData::NA_VAL = -999;

// *********************************************************
// ********************* CONSTRUCTORS **********************
// *********************************************************

/**
    * TickerData::TickerData.
    * Parameterized constructor to create a ticker object
    * @param a_tickerName string Name of ticker
    * @param a_directory string Price data directory path.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
TickerData::TickerData( std::string &a_tickerName, const std::string &a_directory ) : m_tickerName(a_tickerName), m_tradingDateIndexEnd(INT_MAX), m_tradingDateIndexStart(INT_MAX)
{

    parseTickerData(a_directory);
    calculateMACDfields();
}

// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************

/**
    * TickerData::SetTradingDateRange.
    * Method to set trading date range.
    * @param a_startDate DateTime* Start date of simulation to set.
    * @param a_endDate DateTime* End date of simulation to set.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void TickerData::SetTradingDateRange( DateTime* a_startDate, DateTime* a_endDate )
{
    for(int i=0; i<m_tickerDates.size(); i++)
    {
        if(*m_tickerDates[i] == *a_startDate)
            m_tradingDateIndexStart = i;

        if(*m_tickerDates[i] == *a_endDate)
            m_tradingDateIndexEnd = i;
    }

    // Check if index range is uninitialized
    if(m_tradingDateIndexStart==INT_MAX || m_tradingDateIndexEnd==INT_MAX)
    {
        std::cerr<<"Dorothy Error (SetTradingDateRange): Cannot initialize date range from " \
                    <<a_startDate->GetASCIIDate()<<" to "\
                    <<a_endDate->GetASCIIDate()<<" for ticker: "\
                    <<m_tickerName<<". \n";
        exit(1);
    }
}

/**
    * TickerData::GetMACDSignal.
    * Get current date's MACD signal.
    * @return double The current MACD signal.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
double TickerData::GetMACDSignal()
{
    if(m_tradingDateIndexStart>=m_tradingDateIndexEnd)
    {
        // move to next date and return current date value
        return m_tickerData[TICKER_FIELDS::MACD_HIST].at(m_tradingDateIndexStart--);
    }
    else
    {
        std::cerr<<m_tradingDateIndexStart<<" | "<<m_tradingDateIndexEnd<<std::endl;
        std::cerr<<"Dorothy Error (GetMACDSignal): Date value is out of simulation range for ticker: "\
                    <<m_tickerName<<". \n";

        exit(1);
    }
}


// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

/**
    * TickerData::parseTickerData.
    * Parse ticker data from data source files.
    * @param a_directory string Path where the price data is located.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void TickerData::parseTickerData( const std::string &a_directory )
{
    std::ifstream t_ins(a_directory);
    std::string t_buffer;

    // Validate file open
    if(!t_ins)
    {
        std::cerr <<"Dorothy Error (ParseTickerData): Unable to load price data for ticker "
                  <<m_tickerName<<" from directory: "<< "["<<a_directory<<"]"<<std::endl;
        exit(1);
    }

    // Skip ticker title and column headers
    getline(t_ins, t_buffer);
    getline(t_ins, t_buffer);

    // Parse data
    while(!t_ins.eof())
    {
        // New line
        t_buffer.clear();
        getline(t_ins, t_buffer);

        // Parsing csv data lines
        std::istringstream t_iss(t_buffer);
        std::string t_element;
        int i = 0;
        while (getline(t_iss, t_element, ','))
        {
            // FILL NA value and trim whitespace
            if(t_element.empty())
                t_element = std::to_string(-999.0);
            if(t_element.find(" ") != std::string::npos)
                Utilities::trimBlanks(t_element);

            // Record date information:
            if(i==0)
                m_tickerDates.push_back(new DateTime( stoi(t_element)));
            else
            {
                m_tickerData[i-1].push_back(std::stod(t_element));
            }
            i++;
        }
    }
}

/**
    * TickerData::calculateMACDfields
    * Calculate MACD fields and append to the ticker data.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void TickerData::calculateMACDfields()
{
    // MACD parameters:
    const int FAST_EMA_VAL = 12;
    const int SLOW_EMA_VAL = 26;
    const int SIG_EMA_VAL = 9;

    // Factor values:
    double t_k12 = ((2.0/FAST_EMA_VAL)+1.0);
    double t_k26 = ((2.0/SLOW_EMA_VAL)+1.0);
    double t_k9 = ((2.0/SIG_EMA_VAL)+1.0);

    // Iterators and temp vars:
    int i;

    // FAST EMA CALCULATION
    double t_sma12=0;
    for(i=0;i<FAST_EMA_VAL;i++)
        t_sma12 += m_tickerData[TICKER_FIELDS::CLOSE].at(i);
    t_sma12 /= FAST_EMA_VAL;
    // Squaring and filling in data
    for(i=0; i<m_tickerData[0].size(); i++)
    {
        // EMA
        if (i >= FAST_EMA_VAL)
            m_tickerData[FAST_EMA].push_back(
                    ((m_tickerData[TICKER_FIELDS::CLOSE].at(i) - m_tickerData[TICKER_FIELDS::FAST_EMA].at(i-1)) * t_k12) + m_tickerData[TICKER_FIELDS::FAST_EMA].at(i-1));
            // First SMA
        else if (i==(FAST_EMA_VAL-1))
            m_tickerData[TICKER_FIELDS::FAST_EMA].push_back(t_sma12);
            // Filler value
        else
            m_tickerData[TICKER_FIELDS::FAST_EMA].push_back(NA_VAL);
    }

    // SLOW EMA AND MACD LINE CALCULATION
    double t_sma26=0;
    for(i=0;i<SLOW_EMA_VAL;i++)
        t_sma26 += m_tickerData[TICKER_FIELDS::CLOSE].at(i);
    t_sma26 /= SLOW_EMA_VAL;
    // Squaring and filling in datas
    for(i=0; i<m_tickerData[0].size(); i++)
    {
        if (i >= SLOW_EMA_VAL)
        {
            // SLOW EMA
            m_tickerData[TICKER_FIELDS::SLOW_EMA].push_back(
                    ((m_tickerData[TICKER_FIELDS::CLOSE].at(i) - m_tickerData[TICKER_FIELDS::SLOW_EMA].at(i-1)) * t_k26) + m_tickerData[TICKER_FIELDS::SLOW_EMA].at(i-1));
            // MACD LINE
            m_tickerData[TICKER_FIELDS::MACD_LINE].push_back(m_tickerData[TICKER_FIELDS::FAST_EMA].at(i)-m_tickerData[SLOW_EMA].at(i));
        }
        else if (i==(SLOW_EMA_VAL-1))
        {
            // SMA 26 and MACD LINE
            m_tickerData[TICKER_FIELDS::SLOW_EMA].push_back(t_sma26);
            m_tickerData[TICKER_FIELDS::MACD_LINE].push_back(m_tickerData[TICKER_FIELDS::FAST_EMA].at(i)-m_tickerData[TICKER_FIELDS::SLOW_EMA].at(i));
        }
        else {
            m_tickerData[TICKER_FIELDS::SLOW_EMA].push_back(NA_VAL);
            m_tickerData[TICKER_FIELDS::MACD_LINE].push_back(NA_VAL);
        }
    }

    // SIGNAL AND HISTOGRAM CALCULATION:
    double t_sma9=0;
    for(i=25;i<34;i++)
        t_sma9 += m_tickerData[TICKER_FIELDS::MACD_LINE].at(i);
    t_sma9 /= SIG_EMA_VAL;
    for(i=0; i<m_tickerData[0].size(); i++)
    {
        if (i >= 34)
        {
            // SIGNAL LINE
            m_tickerData[TICKER_FIELDS::SIG_LINE].push_back(
                    ((m_tickerData[TICKER_FIELDS::MACD_LINE].at(i) - m_tickerData[TICKER_FIELDS::SIG_LINE].at(i-1)) * t_k9) +
                    m_tickerData[TICKER_FIELDS::SIG_LINE].at(i-1));
            // HISTOGRAM
            m_tickerData[TICKER_FIELDS::MACD_HIST].push_back(m_tickerData[TICKER_FIELDS::MACD_LINE].at(i)-m_tickerData[TICKER_FIELDS::SIG_LINE].at(i));
        }
        else if (i==33)
        {
            m_tickerData[TICKER_FIELDS::SIG_LINE].push_back(t_sma9);
            m_tickerData[TICKER_FIELDS::MACD_HIST].push_back(m_tickerData[TICKER_FIELDS::MACD_LINE].at(i)-m_tickerData[TICKER_FIELDS::SIG_LINE].at(i));
        }
        else{
            m_tickerData[TICKER_FIELDS::SIG_LINE].push_back(NA_VAL);
            m_tickerData[TICKER_FIELDS::MACD_HIST].push_back(NA_VAL);
        }
    }
}


// *********************************************************
// ******************* DEBUGGING METHODS *******************
// *********************************************************

/**
    * TickerData::PrintDataHead
    * Method to print first 10 data entries for the ticker object. Only print data fields. Used for debugging and checking.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void TickerData::PrintDataHead()
{
//    std::cout<<"TICKER: "<<m_tickerName<<std::endl;
    std::cout<<"----------------------------------------------------------------------------------------------------------"<<std::endl;
    std::cout<< std::left;
    std::cout << std::setw(7) << "DATE" << "\t";
    std::cout << std::setw(7) << "OPEN" << "\t";
    std::cout << std::setw(7) << "CLOSE" << "\t";
    std::cout << std::setw(7) << "LOW" << "\t";
    std::cout << std::setw(7) << "CLOSE" << "\t";
    std::cout << std::setw(7) << "VOLUME" << "\t";
    std::cout << std::setw(7) << "ADJ_CLOSE" << "\t";
    std::cout << std::setw(7) << "DIVIDEND" << "\t";
    std::cout << std::setw(7) << "SPLIT" << "\t";
    std::cout << std::setw(7) << "VWAP" << "\t";
    std::cout << std::setw(7) << "SHARE_OUTSTANDING" << "\t";
    std::cout<<std::endl;

    for (int i = 0; i < 10; i++)
    {
        std::cout << std::setw(7) << m_tickerDates[i]->GetASCIIDate() << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::OPEN].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::HIGH].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::LOW].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::CLOSE].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::VOLUME].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::ADJ_CLOSE].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::DIVIDEND].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::SPLIT].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::VWAP].at(i) << "\t";
        std::cout << std::setw(7) << m_tickerData[TICKER_FIELDS::SHARE_OUTSTANDING].at(i) << "\t";
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
    std::cout<<"----------------------------------------------------------------------------------------------------------"<<std::endl;

}

/**
    * TickerData::PrintAllColumns
    * Method to print a sample of data for all data and calculated columns. Used for debugging and checking.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void TickerData::PrintAllColumns()
{
    std::cout<<"TICKER: "<<m_tickerName<<std::endl;
    std::cout<<"----------------------------------------------------------------------------------------------------------"<<std::endl;
    std::cout<< std::left;
    std::cout << std::setw(9) << "DATE" << "\t";
    std::cout << std::setw(9) << "OPEN" << "\t";
    std::cout << std::setw(9) << "CLOSE" << "\t";
    std::cout << std::setw(9) << "LOW" << "\t";
    std::cout << std::setw(9) << "CLOSE" << "\t";
    std::cout << std::setw(9) << "VOLUME" << "\t";
    std::cout << std::setw(9) << "ADJ_CLOSE" << "\t";
    std::cout << std::setw(9) << "DIVIDEND" << "\t";
    std::cout << std::setw(9) << "SPLIT" << "\t";
    std::cout << std::setw(9) << "VWAP" << "\t";
    std::cout << std::setw(9) << "SHR-OS" << "\t";
    std::cout << std::setw(9) << "FASTEMA" << "\t";
    std::cout << std::setw(9) << "SLOWEMA" << "\t";
    std::cout << std::setw(9) << "MACDLINE" << "\t";
    std::cout << std::setw(9) << "SIGLINE" << "\t";
    std::cout << std::setw(9) << "HISTOGRAM" << "\t";
    std::cout<<std::endl;

    for (int i = 0; i < 50; i++)
    {
        std::cout << std::setw(9) << m_tickerDates[i]->GetASCIIDate() << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::OPEN].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::HIGH].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::LOW].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::CLOSE].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::VOLUME].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::ADJ_CLOSE].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::DIVIDEND].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::SPLIT].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::VWAP].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::SHARE_OUTSTANDING].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::FAST_EMA].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::SLOW_EMA].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::MACD_LINE].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::SIG_LINE].at(i) << "\t";
        std::cout << std::setw(9) << m_tickerData[TICKER_FIELDS::MACD_HIST].at(i) << "\t";
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
    std::cout<<"----------------------------------------------------------------------------------------------------------"<<std::endl;

}


// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************

// Get date offset in the database of the starting trading date specified.
// For example we might have data from 05/02/1996 but the starting trading date specified can be 02/03/2010.
// The offset is the number of trading dates in between these two dates.
// (Can be utilized to optimize the simulator when a different data source is used)
// int GetDateOffset( DateTime* a_date );

//int TickerData::GetDateOffset(DateTime* a_date )
//{
//    for(int i=0; i<m_tickerDates.size(); i++)
//    {
//        if(*m_tickerDates[i] == *a_date)
//            return i;
//    }
//    return INT_MAX;
//}