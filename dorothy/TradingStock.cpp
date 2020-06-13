/**
     *  TradingStock.cpp
     *  Implementation of TradingStock.h
     *
     *
     *  Created by Salil Maharjan on 05/03/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#include "TradingStock.h"
#include "Utilities.h"


// *********************************************************
// ********************* CONSTRUCTORS **********************
// *********************************************************

/**
    * TradingStock::TradingStock
    * Class parameterized constructor to create a trading stock object.
    * @param a_tickerName string Trading stock's ticker name.
    * @param a_startingCapital double Initial capital to invest for the stock.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
TradingStock::TradingStock(std::string a_tickerName, double a_startingCapital) : m_tickerName(a_tickerName), m_availableCapital(a_startingCapital)
{
    m_totalSharesCount = 0;
    m_transactionsTotalCount = 0;
    m_transactionCounter = 0;
    m_longPosFlag = false;
    m_shortPosFlag = false;
}
// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************

/**
    * TradingStock::SellShares
    * Method to sell shares. Updates capital and record variables.
    * @param a_shares double Number of shares to sell.
    * @param a_price double Price at which to sell shares.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::SellShares( double a_shares, double a_price )
{
    // Remove shares
    m_totalSharesCount -= a_shares;
    m_shareActivityRecord.push_back(-a_shares);
    // Update capital
    this->IncreaseCapitalBy(a_shares*a_price);
}

/**
    * TradingStock::BuyShares
    * Method to buy shares. Updates capital and record variables.
    * @param a_shares double Number of shares to buy.
    * @param a_price double Price at which to buy shares.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::BuyShares( double a_shares, double a_price )
{
    // Buy shares
    m_totalSharesCount += a_shares;
    m_shareActivityRecord.push_back(a_shares);
    // Update capital
    this->DecreaseCapitalBy(a_shares*a_price);
}

/**
    * TradingStock::RecordSignal
    * Function to record the trading day signal and update the days in position of the stocks held.
    * @param a_signal double Day's trading signal.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::RecordSignal(double a_signal )
{
    m_signalActivityRecord.push_back(a_signal);
    newTradingDay();
}

/**
    * TradingStock::IncreaseSharesBy
    * Function to increase shares owned in the stock.
    * @param a_share double Number of shares to add.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::IncreaseSharesBy( double a_share )
{
    m_totalSharesCount += a_share;
    m_shareActivityRecord.push_back(a_share);
}

/**
    * TradingStock::DecreaseSharesBy
    * Function to decrease shares owned in the stock.
    * @param a_share double Number of shares to remove.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::DecreaseSharesBy( double a_share )
{
    m_totalSharesCount -= a_share;
    m_shareActivityRecord.push_back(-a_share);
}

/**
    * TradingStock::IncreaseCapitalBy
    * Function to increase available capital in the stock and record it.
    * @param a_capital double Capital to increase.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::IncreaseCapitalBy( double a_capital )
{
    m_availableCapital += a_capital;
    m_capitalActivityRecord.push_back(a_capital);
}

/**
    * TradingStock::DecreaseCapitalBy
    * Function to decrease available capital in the stock and record it.
    * @param a_capital double Capital to decrease.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::DecreaseCapitalBy( double a_share )
{
    m_availableCapital -= a_share;
    m_capitalActivityRecord.push_back(-a_share);
}

/**
    * TradingStock::RecordPNL
    * Records PNL and calls RecordDailyReturn to record ROR. (coupled functions)
    * @param a_pnl double Profit or loss for the day.
    * @param a_investedCapital double Invested capital in the position generating profit or loss.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::RecordPNL( double a_pnl, double a_investedCapital )
{
    // Record daily PNL and cumulative PNL
    m_dailyPNLRecord.push_back(a_pnl);
    if(m_cumulativePNLRecord.empty())
        m_cumulativePNLRecord.push_back(a_pnl);
    else
        m_cumulativePNLRecord.push_back(a_pnl+m_cumulativePNLRecord.back());

    // Get invested capital and call RecordDailyReturn to update daily ROR
    a_investedCapital = abs(a_investedCapital);
    if(a_investedCapital>0)
        this->RecordDailyReturn(a_pnl/a_investedCapital);
    else
        this->RecordDailyReturn(0);
}

/**
    * TradingStock::RecordTransactionsCount
    * Record daily transaction count.
    * @param a_count int Daily transactions count.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::RecordTransactionsCount( int a_count )
{
    m_transactionsTotalCount += a_count;
    m_TransactionsCountRecord.push_back(a_count);
}

/**
    * TradingStock::RecordDailyReturn
    * Record daily rate of return (ROR).
    * @param a_amount double Daily ROR.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::RecordDailyReturn( double a_amount )
{
    m_dailyReturnRecord.push_back(a_amount);
}

/**
    * TradingStock::OpenPosition
    * Open a new position for the trading stock.
    * @param a_date DateTime* Today's date.
    * @param a_signal double Current day's signal.
    * @param a_share double Number of shares to open.
    * @param a_price double Price of shares at the time of opening.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::OpenPosition( DateTime* a_date, double a_signal, double a_share, double a_price )
{
    m_transactions.push_back(new Transaction(a_date, a_signal, a_share, a_price));
}

/**
    * TradingStock::CloseLongPositions
    * Method to close profiting long positions. Returns profit/loss generated from closing long positions.
    * Updates invested capital in closing long positions and number of long positions that are variables passed by reference.
    * @param a_date DateTime* Today's date.
    * @param a_signal double Current day's signal.
    * @param a_price double Price of shares at the time of closing.
    * @param a_investedCaptial double& Invested capital placeholder to return the invested capital in long positions.
    * @param a_numOfShares double& Placeholder to return number of shares in long position that were closed.
    * @return double Profit or loss generated from closing long positions.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
double TradingStock::CloseLongPositions( DateTime* a_date, double a_signal, double a_price, double &a_investedCaptial, double &a_numOfShares )
{
    // PNL from closing positions
    double t_PNL = 0;
    // Invested capital in closing positions
    double t_invest = 0;
    // Number of shares in long position
    double t_positions = 0;
    // Transactions closed counter
    int t_counter = 0;

    // Stock's transaction loop
    for(int i=0; i<m_transactions.size(); i++)
    {
        // Check for long positions that are not closed that will profit from closing
        if(m_transactions[i]->IsLongPosition() && !m_transactions[i]->IsClosed() && m_transactions[i]->GetOpenedPrice()<a_price)
        {
            t_PNL += m_transactions[i]->CloseTransaction(a_date, a_signal, a_price);
            t_invest += (m_transactions[i]->GetOpenedPrice()*m_transactions[i]->GetNumSharesHeld());
            t_positions += m_transactions[i]->GetNumSharesHeld();
            t_counter++;
        }
    }

    // Update placeholder variables to return value
    a_investedCaptial = t_invest;
    a_numOfShares = t_positions;

    // Record the number of transactions completed
    RecordTransactionsCount(t_counter);

    return t_PNL;
}

/**
    * TradingStock::CloseShortPositions
    * Method to close profiting short positions. Returns profit/loss generated from closing short positions.
    * Updates invested capital in closing short positions and number of short positions that are variables passed by reference.
    * @param a_date DateTime* Today's date.
    * @param a_signal double Current day's signal.
    * @param a_price double Price of shares at the time of closing.
    * @param a_investedCaptial double& Invested capital placeholder to return the invested capital in long positions.
    * @param a_numOfShares double& Placeholder to return number of shares in long position that were closed.
    * @return double Profit or loss generated from closing long positions.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
double TradingStock::CloseShortPositions( DateTime* a_date, double a_signal, double a_price, double &a_investedCaptial, double &a_numOfShares )
{
    // PNL from closing positions
    double t_PNL = 0;
    // Invested capital in closing positions
    double t_invest = 0;
    // Number of shares in long position
    double t_positions = 0;
    // Transactions closed counter
    int t_counter = 0;

    // Stock's transaction loop
    for(int i=0; i<m_transactions.size(); i++)
    {
        // Check for short positions that are not closed that will profit from closing
        if(!m_transactions[i]->IsLongPosition() && !m_transactions[i]->IsClosed() && m_transactions[i]->GetOpenedPrice()>a_price)
        {
            t_PNL += m_transactions[i]->CloseTransaction(a_date, a_signal, a_price);
            t_invest += (m_transactions[i]->GetOpenedPrice()*m_transactions[i]->GetNumSharesHeld());
            t_positions += m_transactions[i]->GetNumSharesHeld();
            t_counter++;
        }
    }

    // Update placeholder variables to return value
    a_investedCaptial = t_invest;
    a_numOfShares = t_positions;

    // Record the number of transactions completed
    RecordTransactionsCount(t_counter);

    return -t_PNL;
}

/**
    * TradingStock::CheckShortSignal
    * Method to check if signal is profitable for short positions.
    * @param a_price double Current price of shares.
    * @return bool If signal will be profitable.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
bool TradingStock::CheckShortSignal( double a_price )
{
    // Stock's transaction loop
    for(int i=0; i<m_transactions.size(); i++)
    {
        // Check for short positions that are not closed
        if(!m_transactions[i]->IsLongPosition() && !m_transactions[i]->IsClosed() && m_transactions[i]->GetOpenedPrice()>a_price)
            return true;
    }
    return false;
}

/**
    * TradingStock::CheckLongSignal
    * Method to check if signal is profitable for long positions.
    * @param a_price double Current price of shares.
    * @return bool If signal will be profitable.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
bool TradingStock::CheckLongSignal( double a_price )
{
    // Stock's transaction loop
    for(int i=0; i<m_transactions.size(); i++)
    {
        // Check for short positions that are not closed
        if(m_transactions[i]->IsLongPosition() && !m_transactions[i]->IsClosed() && m_transactions[i]->GetOpenedPrice()<a_price)
            return true;
    }
    return false;
}

/**
    * TradingStock::UpdateStockPosition
    * Method to update long/short position flags of the trading stock.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::UpdateStockPosition()
{
    // Update short/long flags
    bool t_short = false;
    bool t_long = false;
    for(int i=0; i<m_transactions.size(); i++)
    {
        if(!m_transactions[i]->IsClosed())
        {
            if(m_transactions[i]->IsLongPosition())
                t_long = true;
            else
                t_short = true;
        }
    }
    m_longPosFlag = t_long;
    m_shortPosFlag = t_short;
}

/**
    * TradingStock::CloseStalePositions
    * Method to close positions that exceed maximum days limit.
    * @param a_dayLimit int Maximum days in position limit.
    * @param a_date DateTime* Today's date.
    * @param a_signal double Current day's signal.
    * @param a_price double Current price of shares.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::CloseStalePositions( int a_dayLimit, DateTime* a_date, double a_signal, double a_price )
{
    // PNL from closing positions
    double t_PNL = 0;
    // Invested capital in closing positions
    double t_invest = 0;
    // Number of shares to close
    double t_positions;
    // Flag if transactions are closed
    bool t_closed = false;
    // Transactions counter
    int t_counter = 0;

    // Stock's transactions loop:
    for(int i=0; i<m_transactions.size(); i++)
    {
        // Check for open positions that exceed max days limit
        if( (!m_transactions[i]->IsClosed()) && (m_transactions[i]->GetDaysInPosition()>=a_dayLimit) )
        {
            if(m_transactions[i]->IsLongPosition())
            {
                t_PNL += m_transactions[i]->CloseTransaction(a_date, a_signal, a_price);
                t_invest += (m_transactions[i]->GetOpenedPrice()*m_transactions[i]->GetNumSharesHeld());
                t_positions = m_transactions[i]->GetNumSharesHeld();
                SellShares(t_positions, a_price);
                t_closed = true;
                t_counter++;
            }
            else
            {
                t_PNL -= m_transactions[i]->CloseTransaction(a_date, a_signal, a_price);
                t_invest -= (m_transactions[i]->GetOpenedPrice()*m_transactions[i]->GetNumSharesHeld());
                t_positions = m_transactions[i]->GetNumSharesHeld();
                BuyShares(-t_positions, a_price);
                t_closed = true;
                t_counter++;
            }
        }
    }

    // If stale positions were closed
    if(t_closed)
    {
        // Update records for new closes (Daily PNL, Cumulative PNL and ROR)
        m_dailyPNLRecord.at(m_dailyPNLRecord.size()-1) += t_PNL;
        m_cumulativePNLRecord.at(m_cumulativePNLRecord.size()-1) += t_PNL;
        if(t_invest>0)
            m_dailyReturnRecord.at(m_dailyReturnRecord.size()-1) += (t_PNL/t_invest);


        // Update short/long flags after closing
        UpdateStockPosition();

        // Record transactions performed
        RecordTransactionsCount(t_counter);
    }
}

/**
    * TradingStock::GetInvestedCapital
    * Get latest committed capital in an investment
    * @return double Latest committed capital activity.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
double TradingStock::GetInvestedCapital()
{
    if(m_capitalActivityRecord.size()>0)
    {
        double t_capital = m_capitalActivityRecord.at(m_capitalActivityRecord.size() - 1);
        if (t_capital >= 0)
            return 0;
        else
            return (-t_capital);
    }
    return 0;
}

/**
    * TradingStock::CalculateDailySharpeRatio
    * Calculate daily sharpe ratio for individual stock.
    * Can be used to get individual stock's Sharpe ratio instead of the complete model's Sharpe.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::CalculateDailySharpeRatio()
{
    std::vector<double> t_dailyReturns(m_dailyReturnRecord.begin(), m_dailyReturnRecord.end());

    double t_averageReturn = Utilities::GetAverage(t_dailyReturns);
    double t_stddev = Utilities::GetStandardDeviation(t_dailyReturns, t_averageReturn);
    m_sharpeRatioRecord.push_back( (252/sqrt(252)) * (t_averageReturn / t_stddev) );
}

/**
    * TradingStock::GetSharpeToday
    * Accessor method to get the Sharpe ratio of the current day of an individual stock.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
const double TradingStock::GetSharpeToday()
{
    if(m_sharpeRatioRecord.size()>0)
        return m_sharpeRatioRecord.at(m_sharpeRatioRecord.size()-1);
    else
        return 0;
}

/**
    * TradingStock::GetShortSharesHeld
    * Accessor method to get short shares held in the trading stock.
    * @return double Number of short positions held.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
const double TradingStock::GetShortSharesHeld()
{
    // # of short shares held
    double t_shares = 0;

    for(int i=0; i<m_transactions.size(); i++)
    {
        if(!m_transactions[i]->IsLongPosition())
            t_shares += m_transactions[i]->GetNumSharesHeld();
    }

    return t_shares;
}

/**
    * TradingStock::GetLongSharesHeld
    * Accessor method to get long shares held in the trading stock.
    * @return double Number of short positions held.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
const double TradingStock::GetLongSharesHeld()
{
    // # of long shares held
    double t_shares = 0;

    for(int i=0; i<m_transactions.size(); i++)
    {
        if(m_transactions[i]->IsLongPosition())
            t_shares += m_transactions[i]->GetNumSharesHeld();
    }

    return t_shares;
}

/**
    * TradingStock::GetShortCapital
    * Accessor method to get capital invested in short positions in trading stock.
    * @return double Total capital in short positions.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
const double TradingStock::GetShortCapital()
{
    // capital invested
    double t_capital = 0;

    for(int i=0; i<m_transactions.size(); i++)
    {
        if(!m_transactions[i]->IsLongPosition())
            t_capital += (m_transactions[i]->GetNumSharesHeld()*m_transactions[i]->GetOpenedPrice());
    }

    return -t_capital;
}

/**
    * TradingStock::GetLongCapital
    * Accessor method to get capital invested in long positions in trading stock.
    * @return double Total capital in long positions.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
const double TradingStock::GetLongCapital()
{
    // capital invested
    double t_capital = 0;

    for(int i=0; i<m_transactions.size(); i++)
    {
        if(m_transactions[i]->IsLongPosition())
            t_capital += (m_transactions[i]->GetNumSharesHeld()*m_transactions[i]->GetOpenedPrice());
    }

    return t_capital;
}

/**
    * TradingStock::GetClosedTransactions
    * Method to get closed transactions. Once returned, they are removed from the record.
    * @return double Total capital in long positions.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
std::vector<Transaction*> TradingStock::GetClosedTransactions()
{
    std::vector<Transaction*> t_temp;

    for(int i=0; i<m_transactions.size(); i++)
    {
        if(m_transactions[i]->IsClosed())
        {
            t_temp.push_back(m_transactions[i]);
            m_transactions.erase(m_transactions.begin()+i);
            i--;
        }
    }

    return t_temp;
}

/**
    * TradingStock::GetTransactionCount
    * Function to get current closed transaction counter.
    * @return double Number of current completed transactions.
    * @author Salil Maharjan
    * @date 5/14/20.
*/
double TradingStock::GetTransactionCount()
{
    assert(m_transactionCounter<=m_transactionsTotalCount);
    return m_transactionCounter++;
}

// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

/**
    * TradingStock::newTradingDay
    * Method to update days in position of all held shares
    * @author Salil Maharjan
    * @date 5/14/20.
*/
void TradingStock::newTradingDay()
{
    for(int i=0; i<m_transactions.size(); i++)
        m_transactions[i]->NextDayInPosition();
}

// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************

// Get today and yesterday's signal to check for crossover
//    void GetCurrentPastSignal(double &a_past, double &a_current);
//
//void TradingStock::GetCurrentPastSignal(double &a_past, double &a_current)
//{
//    a_past = m_signalActivityRecord[m_signalActivityRecord.size()-2];
//    a_current = m_signalActivityRecord[m_signalActivityRecord.size()-1];
//}