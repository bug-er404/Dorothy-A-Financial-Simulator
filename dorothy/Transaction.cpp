/**
     *  Transaction.cpp
     *  Implementation of Transaction.h.
     *
     *
     *  Created by Salil Maharjan on 05/22/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#include "Transaction.h"

// *********************************************************
// ********************* CONSTRUCTORS **********************
// *********************************************************

/**
    * Transaction::Transaction
    * Main parameterized Transaction class constructor. Opens a transaction.
    * @param a_date DateTime* The transaction open date.
    * @param a_signal double The signal while opening transaction.
    * @param a_share double The number of shares to open position with.
    * @param a_price double The price at which the transaction is opened.
    * @author Salil Maharjan
    * @date 5/22/20.
*/
Transaction::Transaction(DateTime* a_date, double a_signal, double a_share, double a_price)
{
    m_TAOpenDate = a_date;
    m_TAOpenSignal = a_signal;
    m_currentSharesCount = a_share;
    m_TAOpenPrice = a_price;
    m_inPosition = 0;

    // Set long position flag.
    if(a_signal>0)
        m_longFlag = true;
    else
        m_longFlag = false;

    m_closedFlag = false;
}

// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************

/**
    * Transaction::NextDayInPosition
    * Method to update the number of days in position in each trading day.
    * @author Salil Maharjan
    * @date 5/22/20.
*/
void Transaction::NextDayInPosition()
{
    // Simply add to the number of days in position.
    // (More functionality can be added later)
    m_inPosition++;
}

/**
    * Transaction::CloseTransaction
    * Function to close transaction and return the PNL from the completed transaction.
    * @param a_date DateTime* The transaction close date.
    * @param a_signal double The signal while closing transaction.
    * @param a_price double The price at which the transaction is closed.
    * @return double The profit/loss from completing the transaction.
    * @author Salil Maharjan
    * @date 5/22/20.
*/
double Transaction::CloseTransaction(DateTime* a_date, double a_signal, double a_price)
{
    m_TACloseDate = a_date;
    m_TACloseSignal = a_signal;
    m_TAClosePrice = a_price;

    m_closedFlag = true;

    return ((m_currentSharesCount*m_TAClosePrice) - (m_currentSharesCount*m_TAOpenPrice));
}

// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************
