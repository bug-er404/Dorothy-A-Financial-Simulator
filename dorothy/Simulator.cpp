/**
     *  Simulator.cpp
     *  Implementation of Simulator.h.
     *
     *
     *  Created by Salil Maharjan on 4/29/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#include "Simulator.h"
#include "Utilities.h"

// *********************************************************
// ********************* CONSTRUCTORS **********************
// *********************************************************

/**
    * Simulator::Simulator
    * Parameterized constructor for Simulator class. Initializes the Simulator.
    * Can handle multiple configuration files.
    * @param argc int Number of command line arguments.
    * @param argv const char* Array of command line arguments.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
Simulator::Simulator( int argc, const char* argv[] )
{
    // Simulation counters
    m_simulationCount = 0;

    // Record all passed config files
    m_configFilesPassed = argc-1;
    for(int i = 1; i<argc; i++)
        m_configFile.push_back(argv[i]);

    std::cout<<"--------------------------------------------------------------------"<<std::endl;
    std::cout<<"Dorothy: Running simulator for "<<m_configFilesPassed<<" different configurations passed."<<std::endl;
    std::cout<<"--------------------------------------------------------------------"<<std::endl<<std::endl;

}


// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************
/**
    * Simulator::StartSimulation
    * Interface method to start the simulation
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::StartSimulation()
{
    for(int i=0; i<m_configFile.size(); i++)
    {
        std::cout<<"------------------------------------------------------------"<<std::endl;
        std::cout<<"Dorothy: Simulation #"<<i+1<<std::endl;
        std::cout<<"------------------------------------------------------------"<<std::endl<<std::endl;
        if(i==0)
            initializeSimulator(m_configFile[i]);
        else
            rerunSimulator(m_configFile[i]);

        RunSimulation();
    }
}


/**
    * Simulator::RunSimulation
    * Main method to run the simulation.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::RunSimulation()
{
    // Update simulation counter
    m_simulationCount++;

    std::cout<<"Dorothy: Running simulation..."<<std::endl;

    std::vector<std::string> t_tickers = m_data->GetUniverse();
    std::vector<DateTime*> t_tradingDates = m_data->GetTradingDatesInRange(m_startDate, m_endDate);
    std::cout << "Dorothy: Simulating for " << t_tradingDates.size() <<" days."<<std::endl;

    m_totalPNL = 0;

    // Set trading date index for respective tickers to run simulation:
    for (auto &t_stock : m_tradingStocks) {
        TickerData &t_stockData = *(m_data->GetTickerData(t_stock->GetStockName()));
        t_stockData.SetTradingDateRange(t_tradingDates.at(0), t_tradingDates.at(t_tradingDates.size()-1));
    }

    // * DATE RANGE LOOP *
    for (int t_day = 0; t_day < t_tradingDates.size(); t_day++)
    {
        // Reset daily data variables:
        m_today = t_tradingDates[t_day];
        m_totalPosCount = 0;
        m_shortPosCount = 0;
        m_longPosCount = 0;
        m_dailyPNL = 0.0;
        m_ROR = 0.0;
        m_grossMarketValue = 0.0;
        m_netMarketValue = 0.0;

        // * CONSTITUENTS LOOP *
        for (auto &t_stock : m_tradingStocks)
        {
            TickerData &t_stockData = *(m_data->GetTickerData(t_stock->GetStockName()));

            // Get date offset in the database from first date in data to trading start date
            // We get the exact index of the data row.
            double t_signal = calculateSignal(t_stockData);

            // Record signal
            t_stock->RecordSignal(t_signal);

            // Write signal info to signal publication file
            if(m_signalPublication)
                recordSignalInfo(t_signal);

            // No signal:
            if (t_signal == TickerData::NA_VAL)
            {
                // Record no transaction
                recordNoActivity(*t_stock);
                continue;
            }
            else
            {
                // Get closing price and make trade
                double t_currentPrice = t_stockData.GetCurrentDataOf(TICKER_FIELDS::CLOSE);
                makeTrade(*t_stock, t_currentPrice, t_signal);

                // Total, short and long positions in stock
                double t_positions = t_stock->GetTotalShares();
                if (t_stock->IsShort())
                    m_shortPosCount += t_stock->GetShortSharesHeld();
                if (t_stock->IsLong())
                    m_longPosCount += t_stock->GetLongSharesHeld();

                // Update simulation variables:
                m_totalPosCount += (t_stock->GetLongSharesHeld()+(-t_stock->GetShortSharesHeld()));
                m_dailyPNL += t_stock->GetDailyPnL();
                m_totalPNL += t_stock->GetDailyPnL();
                m_ROR += t_stock->GetDailyROR();
                m_grossMarketValue += abs(t_positions*t_currentPrice);
                m_netMarketValue += (t_stock->GetLongCapital()-t_stock->GetShortCapital());
            }
        }
        // Record daily PNL and ROR:
        m_dailyPNLRecord.push_back(m_dailyPNL);
        m_dailyCumulativeROR.push_back(m_ROR);

        // Write info to daily statistics file if flag is set.
        if(m_dailyPublication)
            recordDailyStatistics();
    }

    // End message
    std::cout << "Dorothy: Simulation completed successfully." << std::endl;
    std::cout<< "Dorothy run statistics: "<<std::endl;
    std::cout <<std::fixed<< std::setprecision(2)<<"Total profits: $" <<m_totalPNL<<std::endl;
    std::cout << "Simulation Sharpe Ratio: "<<calculateSharpeRatio()<<std::endl;

    // Close open streams for publications and give update
    std::cout<<"**************************************************"<<std::endl;
    std::cout<<"Dorothy - Publications Reports: "<<std::endl;
    // Close daily publication file
    if(m_dailyPublication)
    {
        closeDailyPublication();

        // Write the monthly statistics file and close.
        // To get monthly statistics, daily publications should be set too.
        if(m_monthlyPublication && m_monthlyInfoFile)
        {
            recordMonthlyStatistics();
            closeMonthlyPublication(true);
        }

    }
    // If monthly publication is set without daily, no record file is made.
    if(m_monthlyPublication)
        closeMonthlyPublication(false);
    // Close transaction publication file
    if(m_transactionPublication)
        closeTransactionPublication();
    if(m_signalPublication)
        closeSignalPublication();
    std::cout<<"**************************************************"<<std::endl;

    // Clear records
    m_dailyPNLRecord.clear();
    m_dailyCumulativeROR.clear();
    m_sharpeRecord.clear();
}


// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

/**
    * Simulator::initializeSimulator
    * Main method to initialize simulator for run.
    * @param a_configFile char* Config file to initialize simulator for.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::initializeSimulator( const char* a_configFile )
{
    // Load configurations
    m_config = new Config(a_configFile);

    // Check if required configurations are in config file and load them
    if(checkParameters())
        loadParameters();
    else
    {
        std::cerr <<"Dorothy Error (initializeSimulator): Unable to load simulator. Configuration file incomplete. \n"
                    "Please enter all required configuration values."<<std::endl;
        exit(1);
    }

    // Get data of all specified constituents in the universe
    m_data = DataAccess::GetDbInstance(m_tickerPath, m_universePath);

    // Assert date is in range.
    verifyDateParameters();

    // Initialize ticker stocks
    initializeTradingTickers();

    // Open files to write publications
    initializePublications();
}
/**
    * Simulator::rerunSimulator
    * Rerun simulator for consecutive configuration files
    * @param a_configFile char* Config file to initialize simulator for.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::rerunSimulator( const char* a_configFile )
{
    // Load new configurations
    m_config = new Config(a_configFile);

    // Check if required configurations are in config file and load them
    if(checkParameters())
        loadParameters();
    else
    {
        std::cerr <<"Dorothy Error (initializeSimulator): Unable to load simulator. Configuration file incomplete. \n"
                    "Please enter all required configuration values."<<std::endl;
        exit(1);
    }

    // Assert date is in range.
    verifyDateParameters();

    // Clear trading stocks and reinitialize ticker stocks
    m_tradingStocks.clear();
    initializeTradingTickers();

    // Open files to write publications
    initializePublications();
}

/**
    * Simulator::initializePublications
    * Initializes file write streams for the publications requested in configurations.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::initializePublications()
{
    // File names according the config file number:
    std::string t_dailyFile = "./publications/DailyReport"+std::to_string(m_simulationCount)+".csv";
    std::string t_monthlyFile = "./publications/MonthlyReport"+std::to_string(m_simulationCount)+".csv";
    std::string t_transactionFile = "./publications/TransactionReport"+std::to_string(m_simulationCount)+".txt";
    std::string t_signalFile = "./publications/SignalReport"+std::to_string(m_simulationCount)+".csv";

    if(m_dailyPublication)
    {
        m_dailyInfoFile.open(t_dailyFile, std::ios::trunc);
        if(m_dailyInfoFile)
            m_dailyInfoFile<<"YYYYMMDD,Date,Short Positions,Long Positions,Total Positions,Daily PnL,Cumulative PnL,ROR,Market Value,Net Market Value,Sharpe\n";
        else
            std::cerr<<"Dorothy Error: Cannot open file DailyReport.csv"<<std::endl;
    }
    if(m_monthlyPublication)
    {
        m_monthlyInfoFile.open(t_monthlyFile, std::ios::trunc);
        if(m_monthlyInfoFile)
            m_monthlyInfoFile<<"MM/YYYY,Short Positions,Long Positions,Total Positions,Monthly PnL,Cumulative PnL,ROR,Market Value,Net Market Value, Sharpe \n";
        else
            std::cerr<<"Dorothy Error: Cannot open file MonthlyReport.csv"<<std::endl;
    }
    if(m_transactionPublication)
    {
        m_transactionInfoFile.open(t_transactionFile, std::ios::trunc);
        if(!m_transactionInfoFile)
            std::cerr<<"Dorothy Error: Cannot open file TransactionReport.txt"<<std::endl;
    }
    if(m_signalPublication)
    {
        m_signalInfoFile.open(t_signalFile, std::ios::trunc);
        if(m_signalInfoFile)
            m_signalInfoFile<<"Date, Signal \n";
        else
            std::cerr<<"Dorothy Error: Cannot open file SignalReport.txt"<<std::endl;
    }

    // TODO: CODE TO INCLUDE WHEN CONSIDERING PORTFOLIO PUBLICATION AND SEPARATE SHARPE PUBLICATION
//    if(m_portfolioPublication)
//    {
//        m_portfolioInfoFile.open("PortfolioReport.csv");
//    }
//    if(m_sharpePublication)
//    {
//        m_sharpeInfoFile.open("SharpeReport.csv", std::ios::trunc);
//        if(!m_sharpeInfoFile)
//            std::cerr<<"Dorothy Error: Cannot open file SharpeReport.csv"<<std::endl;
//    }
}

/**
    * Simulator::checkParameters
    * Checks if all required parameters are passed for simulator to run.
    * @return bool If all required parameters are in configuration file.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
bool Simulator::checkParameters()
{
    // Path parameters
    return (m_config->ContainsParameter(UNIVERSE_PATH_PARAM)
            && m_config->ContainsParameter(TICKER_PATH_PARAM)
            // Portfolio parameters
            && m_config->ContainsParameter(PORTFOLIO_CAPITAL)
            && m_config->ContainsParameter(MAX_CAPITAL_PER_STOCK_PARAM)
            // Signal parameters
            && m_config->ContainsParameter(ENTRY_SIGNAL_PARAM)
            && m_config->ContainsParameter(EXIT_SIGNAL_PARAM)
            // Simulation date parameters
            && m_config->ContainsParameter(PERIOD_START_DATE)
            && m_config->ContainsParameter(PERIOD_END_DATE)
            // Trading portfolio parameters
            && m_config->ContainsParameter(MAX_POS_PER_STOCK)
            && m_config->ContainsParameter(DAYS_IN_POS_PARAM)
            && m_config->ContainsParameter(STOP_LOSS_PARAM)
            // Publication parameters
            && m_config->ContainsParameter(DAILY_PUB_PARAM)
            && m_config->ContainsParameter(MONTHLY_PUB_PARAM)
            && m_config->ContainsParameter(TRANSACTION_PUB_PARAM)
            && m_config->ContainsParameter(SIGNAL_PUB_PARAM)
            // TODO: CODE TO INCLUDE WHEN CONSIDERING PORTFOLIO PUBLICATION AND SEPARATE SHARPE PUBLICATION
//           && m_config->ContainsParameter(PORTFOLIO_PUB_PARAM)
//           && m_config->ContainsParameter(SHARPE_PUB_PARAM)
    );
}

/**
    * Simulator::loadParameters
    * Loads parameters from configuration file that are required for the simulation.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::loadParameters()
{
    // Path parameters
    m_config->GetParameter(UNIVERSE_PATH_PARAM, m_universePath);
    m_config->GetParameter(TICKER_PATH_PARAM, m_tickerPath);
    // Portfolio parameters
    m_config->GetParameter(PORTFOLIO_CAPITAL, m_portfolioCapital);
    m_config->GetParameter(MAX_CAPITAL_PER_STOCK_PARAM, m_capitalLimitPerStock);
    // Signal parameters
    m_config->GetParameter(ENTRY_SIGNAL_PARAM, m_entrySignal);
    m_config->GetParameter(EXIT_SIGNAL_PARAM, m_exitSignal);
    // Simulation date parameters
    int t_date;
    m_config->GetParameter(PERIOD_START_DATE, t_date);
    m_startDate = new DateTime(t_date);
    m_config->GetParameter(PERIOD_END_DATE, t_date);
    m_endDate = new DateTime(t_date);
    // Trading portfolio parameters
    m_config->GetParameter(MAX_POS_PER_STOCK, m_maxPositionsPerStock);
    m_config->GetParameter(DAYS_IN_POS_PARAM, m_maxDaysPerPosition);
    m_config->GetParameter(STOP_LOSS_PARAM, m_stopLoss);
    // Publication parameters
    m_config->GetParameter(DAILY_PUB_PARAM, m_dailyPublication);
    m_config->GetParameter(MONTHLY_PUB_PARAM, m_monthlyPublication);
    m_config->GetParameter(TRANSACTION_PUB_PARAM, m_transactionPublication);
    m_config->GetParameter(SIGNAL_PUB_PARAM, m_signalPublication);
    // TODO: CODE TO INCLUDE WHEN CONSIDERING PORTFOLIO PUBLICATION AND SEPARATE SHARPE PUBLICATION
//    m_config->GetParameter(PORTFOLIO_PUB_PARAM, m_portfolioPublication);
//    m_config->GetParameter(SHARPE_PUB_PARAM, m_sharpePublication);

}

/**
    * Simulator::initializeTradingTickers
    * Initialize stock tickers from constituents file that the simulator will be trading.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::initializeTradingTickers()
{
    // Give max capital
    for(auto const& t_ticker: m_data->GetUniverse())
    {
        // Sufficient capital
        if(m_portfolioCapital>=m_capitalLimitPerStock)
        {
            m_portfolioCapital -= m_capitalLimitPerStock;
            m_tradingStocks.push_back(new TradingStock(t_ticker, m_capitalLimitPerStock));
        }
            // Not sufficient but remaining capital
        else if (m_portfolioCapital>0)
        {
            m_tradingStocks.push_back(new TradingStock(t_ticker, m_portfolioCapital));
            m_portfolioCapital = 0;
        }
            // No capital
        else
        {
            m_tradingStocks.push_back(new TradingStock(t_ticker, 0));
            std::cerr<<"Dorothy Error(initializeTradingTickers): Not sufficient capital for the number of stocks to invest in."
                     << "Please increase portfolio capital, decrease max capital per stock or remove one of the tickers"<<std::endl;
        }

    }

    std::cout<<"Dorothy: Simulator initialized for trading "<<m_tradingStocks.size()<<" stocks."<<std::endl;
}

/**
    * Simulator::verifyDateParameters
    * Verify date range in configuration file to assert if data is available for that range.
    * If not, the simulation will run for all available data instead of stopping.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::verifyDateParameters()
{
    // Check if data is available for entered date range:
    if(m_data->CheckDateRange(m_startDate, m_endDate))
    {
        std::cerr<<"Dorothy Error (loadParameters): Data unavailable for specified date range."<<std::endl;
        std::cout<<"Dorothy: The simulation will run for all available data."<<std::endl;
        m_startDate = m_data->GetOldestTradingDate();
        m_endDate = m_data->GetLatestTradingDate();
    }

}

/**
    * Simulator::calculateSignal
    * Function to calculate trade signal.
    * Currently uses MACD indicator to get signal.
    * If not, the simulation will run for all available data instead of stopping.
    * @param a_stockData TickerData& The data of the trading stock in the specified date range.
    * @return double Trading signal.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
double Simulator::calculateSignal( TickerData &a_stockData )
{
    double t_signal = a_stockData.GetMACDSignal();
    return t_signal;
}

/**
    * Simulator::makeTrade
    * Function to make trade according to the signal.
    * @param a_stock TradingStock& The stock that is being traded.
    * @param a_currentPrice double& Current price of the stock.
    * @param a_signalToday double& Signal calculated for the day.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::makeTrade( TradingStock &a_stock, double &a_currentPrice, double &a_signalToday )
{
    // Get signal record
    std::vector<double> t_signals = a_stock.GetSignalRecord();

    // Check if it is the first trade
    if(t_signals.size()<2)
    {
        // Initial buy/sell only if entry/exit threshold is met respectively.
        if(a_signalToday>m_entrySignal || a_signalToday<-m_entrySignal)
        {
            openPositions(a_stock, a_currentPrice, a_signalToday);
            a_stock.RecordPNL(0, 0);
        }
        // No action
        else
            recordNoActivity(a_stock);
    }
    // Consecutive trades
    else
    {
        // Get yesterday's signal to check for center-line crossovers
        double t_signalYesterday = t_signals.at(t_signals.size()-2);

        // Flag if any positions are closed
        bool t_closed = false;

        // Close positions if exit signal is met.
        if( (a_stock.IsShort() && a_signalToday>(-m_exitSignal) && a_signalToday<0 && a_stock.CheckShortSignal(a_currentPrice))
            || (a_stock.IsLong() && a_signalToday<m_exitSignal && a_signalToday>0 && a_stock.CheckLongSignal(a_currentPrice)) )
        {
            closePositions(a_stock, a_currentPrice, a_signalToday);
            t_closed = true;
        }

        // Check for crossovers of signal for buy/sell indication:
        // Price will increase, go long
        if(a_signalToday>0 && t_signalYesterday<0 && a_signalToday<m_entrySignal && hasEnoughCapital(a_stock, a_currentPrice))
        {
            openPositions(a_stock, a_currentPrice, a_signalToday);

            // Record no PNL for the day
            if(!t_closed)
                a_stock.RecordPNL(0, 0);
        }
        // Price will fall, sell stocks/go short
        else if(a_signalToday<0 && t_signalYesterday>0 && a_signalToday>(-m_entrySignal) && hasEnoughCapital(a_stock, a_currentPrice))
        {
            openPositions(a_stock, a_currentPrice, a_signalToday);

            // Record no PNL for the day
            if(!t_closed)
                a_stock.RecordPNL(0, 0);
        }
        // No action
        else if(!t_closed)
            recordNoActivity(a_stock);

        // Close positions that exceed max days limit and record any closed transactions.
        a_stock.CloseStalePositions(m_maxDaysPerPosition, m_today, a_signalToday, a_currentPrice);
        if(m_transactionPublication)
            recordTransaction(a_stock);
    }
}

/**
    * Simulator::hasEnoughCapital
    * Check if available capital for a stock is enough to purchase more equities at a_price
    * @param a_stock TradingStock& The stock that is being traded.
    * @param a_currentPrice double& Current price of the stock.
    * @return bool If there is sufficient capital to buy positions of a_stock at a_price.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
bool Simulator::hasEnoughCapital( TradingStock &a_stock, double a_price )
{
    return(a_stock.GetAvailableCapital()>a_price);
}

/**
    * Simulator::recordNoActivity
    * Method to record necessary elements for no activity (code reuse purposes).
    * @param a_stock TradingStock& The stock that is being traded.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::recordNoActivity( TradingStock &a_stock )
{
    a_stock.IncreaseSharesBy(0);
    a_stock.IncreaseCapitalBy(0);
    a_stock.RecordPNL(0, 0);
    a_stock.RecordTransactionsCount(0);
}

/**
    * Simulator::openPositions
    * Method to open positions for a trading stock.
    * @param a_stock TradingStock& The stock that is being traded.
    * @param a_currentPrice double& Current price of the stock.
    * @param a_signalToday double& Signal calculated for the day.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::openPositions( TradingStock &a_stock, double &a_currentPrice, double &a_signal )
{
    int t_numOfPositions = Utilities::RoundOff((a_stock.GetAvailableCapital()/a_currentPrice));

    // Check if opening positions will exceed position limit. (MAX POSITIONS PER STOCK)
    // Reduce number of positions to be under the limit.
    int t_positionsHeld = -a_stock.GetShortSharesHeld()+a_stock.GetLongSharesHeld();
    if( (t_positionsHeld+t_numOfPositions) > m_maxPositionsPerStock)
    {
        t_numOfPositions = m_maxPositionsPerStock - t_positionsHeld;
        if(t_numOfPositions<=0)
            return;
    }

    // No position needs to be open
    if(t_numOfPositions==0)
        return;

    // Open long positions
    if(a_signal>0)
    {
        a_stock.SetLong(true);
        a_stock.OpenPosition(m_today, a_signal, t_numOfPositions, a_currentPrice);
        a_stock.BuyShares(t_numOfPositions, a_currentPrice);
    }
    // Open short positions
    else if(a_signal<0)
    {
        a_stock.SetShort(true);
        a_stock.OpenPosition(m_today, a_signal, -t_numOfPositions, a_currentPrice);
        a_stock.SellShares(t_numOfPositions, a_currentPrice);
    }
}

/**
    * Simulator::closePositions
    * Method to close positions for a trading stock.
    * @param a_stock TradingStock& The stock that is being traded.
    * @param a_currentPrice double& Current price of the stock.
    * @param a_signalToday double& Signal calculated for the day.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::closePositions( TradingStock &a_stock, double &a_currentPrice, double &a_signal )
{
    // Variable passed by reference to get invested capital
    double t_investedCapital = 0;
    // Number of positions in stock
    double t_positions = 0;

    // Close short positions
    if(a_signal<0)
    {
        // Close short transactions, record pnl and buy borrowed shares
        a_stock.RecordPNL(    -(a_stock.CloseShortPositions(m_today, a_signal, a_currentPrice, t_investedCapital, t_positions)), t_investedCapital);
        if(m_transactionPublication)
            recordTransaction(a_stock);
        a_stock.BuyShares(-t_positions, a_currentPrice);
        a_stock.UpdateStockPosition();
    }
    // Close long positions
    else
    {
        // Close long transactions, record pnl and sell shares
        a_stock.RecordPNL(    (a_stock.CloseLongPositions(m_today, a_signal, a_currentPrice, t_investedCapital, t_positions)), t_investedCapital);
        if(m_transactionPublication)
            recordTransaction(a_stock);
        a_stock.SellShares(t_positions, a_currentPrice);
        a_stock.UpdateStockPosition();
    }
}

/**
    * Simulator::calculateSharpeRatio
    * Method to calculate sharpe ratio.
    * @return double Daily sharpe ratio of the model.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
double Simulator::calculateSharpeRatio()
{
    std::vector<double> t_dailyReturns(m_dailyCumulativeROR.begin(), m_dailyCumulativeROR.end());
    double t_averageReturn = Utilities::GetAverage(t_dailyReturns);
    double t_stddev = Utilities::GetStandardDeviation(t_dailyReturns, t_averageReturn);
    m_sharpeRecord.push_back( (252/sqrt(252)) * (t_averageReturn / t_stddev) );
    return m_sharpeRecord.at(m_sharpeRecord.size()-1);
}

/**
    * Simulator::recordSharpeRatio
    * Method to record sharpe ratio to statistics file.
    * Uses calculateSharpeRatio function to calculate sharpe ratio for the entire model.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::recordSharpeRatio()
{
    // (PREVIOUS VERSION) GETS SHARPE RATIO FROM INDIVIDUAL TRADING OBJECT.
    // Can be used to check sharpe ratio for individual stocks.
//    double t_sharp = 0;
//    for (auto &a_stock : m_tradingStocks)
//    {
//        a_stock->CalculateDailySharpeRatio();
//        t_sharp += a_stock->GetSharpeToday();
//    }

    m_dailyInfoFile<<calculateSharpeRatio()<<"\n";
}

/**
    * Simulator::recordTransaction
    * Function to record closed transactions to the transaction report file.
    * @param a_stock TradingStock& The stock that is being traded.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::recordTransaction( TradingStock &a_stock )
{
    std::vector<Transaction*> t_transaction = a_stock.GetClosedTransactions();
    for (int i=0; i<t_transaction.size(); i++)
    {
        m_transactionInfoFile << "Transaction #: "<< a_stock.GetTransactionCount() << "\n" \
                            << "Ticker: " << a_stock.GetStockName() << "\n" \
                                << "\t" << "Number of positions: " << t_transaction[i]->GetNumSharesHeld() << "\n" \
                                << "\t" << "Days in position: " << t_transaction[i]->GetDaysInPosition() << "\n" \
                                << "\t" << "Open date: " << t_transaction[i]->GetPositionOpenDate()->GetASCIIDate() << "\n" \
                                << "\t" << "Close date: " << t_transaction[i]->GetPositionCloseDate()->GetASCIIDate() << "\n" \
                                << "\t" << "Open price: " << t_transaction[i]->GetOpenedPrice() << "\n" \
                                << "\t" << "Close price: " << t_transaction[i]->GetClosedPrice() << "\n" \
                                << "\t" << "Open signal: " << t_transaction[i]->GetOpenSignal() << "\n" \
                                << "\t" << "Close signal: " << t_transaction[i]->GetCloseSignal() << "\n" \
                                << "-----------------------------------------------------------------------------\n\n";
    }
}

/**
    * Simulator::recordSignalInfo
    * Function to record signals to a file.
    * @param a_signal The day's signal
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::recordSignalInfo( double a_signal)
{
    if(m_signalPublication && m_signalInfoFile)
        m_signalInfoFile<<m_today->GetASCIIDate()<<","<<a_signal<<"\n";
}

/**
    * Simulator::recordDailyStatistics
    * Function to record daily statistics to daily stats file.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::recordDailyStatistics()
{
    if (m_dailyPublication && m_dailyInfoFile)
    {
        m_dailyInfoFile << m_today->Get() << "," \
                        << m_today->GetASCIIDate()<<","\
                        << m_shortPosCount <<"," \
                        << m_longPosCount <<"," \
                        << m_totalPosCount <<"," \
                        << m_dailyPNL <<"," \
                        << m_totalPNL <<"," \
                        << m_ROR <<"," \
                        << m_grossMarketValue <<"," \
                        << m_netMarketValue <<",";


        recordSharpeRatio();
    }
}


/**
    * Simulator::recordMonthlyStatistics
    * Function to record monthly statistics to a file.
    * Uses the generated daily publication to generate a monthly report.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::recordMonthlyStatistics()
{
    // Temp value holding variables
    DateTime* t_date;
    double t_shortPos;
    double t_longPos;
    double t_totalPos;
    double t_monthlyPNL = 0;
    double t_cumPNL;
    double t_ROR = 0;
    double t_marketValue;
    double t_netMarketValue;
    double t_sharpe;

    // Current month and year
    int t_currentMonth = 0;
    int t_currentYear = 0;

    // File name
    std::string t_dailyFile = "./publications/DailyReport"+std::to_string(m_simulationCount-1)+".csv";

    // Validate file open
    std::ifstream t_ins(t_dailyFile);
    std::string t_buffer;

    if(!t_ins)
    {
        std::cerr <<"Dorothy Error (recordMonthlyStatistics): Unable to make monthly publication from "<< "["<<t_dailyFile<<"]"<<std::endl;
        exit(1);
    }

    // Skip column headers
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

            // Record column information:
            switch(i)
            {
                case 0:
                    t_date = new DateTime(stoi(t_element));
                    // Uninitialized current date
                    if(t_currentYear==0 && t_currentMonth==0)
                    {
                        t_currentMonth = t_date->GetMonth();
                        t_currentYear = t_date->GetYear();
                        break;
                    }
                    // Check if same month and year
                    else if(t_date->GetMonth() == t_currentMonth && t_date->GetYear() == t_currentYear)
                        break;
                    // Next month reached.
                    else
                    {
                        // Record data gathered which is the statistics on the last day of the month
                        m_monthlyInfoFile << std::to_string(t_currentMonth)+"/"+ std::to_string(t_currentYear) << "," \
                        << t_shortPos <<"," \
                        << t_longPos <<"," \
                        << t_totalPos <<"," \
                        << t_monthlyPNL <<"," \
                        << t_cumPNL <<"," \
                        << t_ROR <<"," \
                        << t_marketValue <<"," \
                        << t_netMarketValue <<","\
                        << t_sharpe<<"\n";

                        // Update to next month and reset values
                        t_currentMonth = t_date->GetMonth();
                        t_currentYear = t_date->GetYear();
                        t_monthlyPNL = 0;
                        t_ROR = 0;
                        break;
                    }
                case 1:
                    break;
                case 2:
                    t_shortPos = std::stod(t_element);
                    break;

                case 3:
                    t_longPos = std::stod(t_element);
                    break;

                case 4:
                    t_totalPos = std::stod(t_element);
                    break;

                case 5:
                    t_monthlyPNL += std::stod(t_element);
                    break;

                case 6:
                    t_cumPNL = std::stod(t_element);
                    break;

                case 7:
                    t_ROR += std::stod(t_element);
                    break;

                case 8:
                    t_marketValue = std::stod(t_element);
                    break;

                case 9:
                    t_netMarketValue = std::stod(t_element);
                    break;

                case 10:
                    t_sharpe = std::stod(t_element);
                    break;

            }
            i++;
        }
    }

    // Record the last month:
    m_monthlyInfoFile << std::to_string(t_currentMonth)+"/"+ std::to_string(t_currentYear) << "," \
                        << t_shortPos <<"," \
                        << t_longPos <<"," \
                        << t_totalPos <<"," \
                        << t_monthlyPNL <<"," \
                        << t_cumPNL <<"," \
                        << t_ROR <<"," \
                        << t_marketValue <<"," \
                        << t_netMarketValue <<","\
                        << t_sharpe<<"\n";
}

/**
    * Simulator::closeDailyPublication
    * Method to close daily publication record file.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::closeDailyPublication()
{
    if(m_dailyPublication && m_dailyInfoFile)
    {
        m_dailyInfoFile.close();
        std::cout<<"\t* Daily Report successfully generated."<<std::endl;
        m_dailyPublication = false;
    }
}

/**
    * Simulator::closeMonthlyPublication
    * Method to close monthly publication record file.
    * Monthly statistics use daily statistics, so daily must be set to write monthly publications.
    * @param a_flag Flag if monthly statistics can be generated.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::closeMonthlyPublication( bool a_flag )
{
    if(m_monthlyPublication && m_monthlyInfoFile)
    {
        m_monthlyInfoFile.close();
        if(a_flag)
            std::cout<<"\t* Monthly Report successfully generated."<<std::endl;
        else
            std::cout<<"\t* Monthly Report not generated. Daily statistics not found. Please set Daily statistics to 1."<<std::endl;
        m_monthlyPublication = false;
    }
}

/**
    * Simulator::closeTransactionPublication
    * Method to close transaction publication record file.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void Simulator::closeTransactionPublication()
{
    if(m_transactionPublication && m_transactionInfoFile)
    {
        m_transactionInfoFile.close();
        std::cout<<"\t* Transaction Report successfully generated."<<std::endl;
        m_transactionPublication = false;
    }
}

/**
    * Simulator::closeSignalPublication
    * Method to close signal publication record file.
    * @author Salil Maharjan
    * @date 5/12/20.
*/

void Simulator::closeSignalPublication()
{
    if(m_signalPublication && m_signalInfoFile)
    {
        m_signalInfoFile.close();
        std::cout<<"\t* Signal Report successfully generated."<<std::endl;
        m_signalPublication = false;
    }
}

// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************

// // TODO: CODE TO INCLUDE WHEN CONSIDERING SEPARATE SHARPE PUBLICATION FILES
//    if(m_sharpePublication && m_sharpeInfoFile)
//    {
//        m_sharpeInfoFile.close();
//        std::cout<<"\t* Sharpe Ratio Report successfully generated."<<std::endl;
//    }
//
//    std::cout<<"*******************************************"<<std::endl;