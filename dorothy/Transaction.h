/**
     *  Transaction.h
     *  Class that handles the record details of transactions for trading stocks.
     *
     *
     *  Created by Salil Maharjan on 05/22/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#ifndef DOROTHY_TRANSACTION_H
#define DOROTHY_TRANSACTION_H

#include "PrefixHeader.pch"
#include "DateTime.h"

class Transaction
{
public:

    // *********************************************************
    // ********************* CONSTRUCTORS **********************
    // *********************************************************

    /// Main parameterized Transaction class constructor.
    Transaction(DateTime* a_date, double a_signal, double a_share, double a_price);

    // *********************************************************
    // *********************** ACCESSORS ***********************
    // *********************************************************

    const double GetOpenedPrice() { return m_TAOpenPrice; }
    const double GetClosedPrice() { return m_TAClosePrice; }
    const int GetDaysInPosition() { return m_inPosition; }
    DateTime * GetPositionOpenDate() { return m_TAOpenDate; }
    DateTime* GetPositionCloseDate() { return m_TACloseDate; }
    const double GetOpenSignal() { return m_TAOpenSignal; }
    const double GetCloseSignal() { return m_TACloseSignal; }
    const double GetNumSharesHeld() { return m_currentSharesCount; }
    const bool IsLongPosition() { return m_longFlag; }
    const bool IsClosed() { return m_closedFlag; }

    // ********************************************************
    // **************** PUBLIC CLASS METHODS  *****************
    // ********************************************************

    /// Method to close constructed transaction objects.
    double CloseTransaction(DateTime* a_date, double a_signal, double a_price);

    /// Transaction method to update the number of days in position of a stock on each trading day.
    void NextDayInPosition();

private:

    /// Empty default constructor.
    Transaction(){};

    // *********************************************************
    // ******************** CLASS VARIABLES ********************
    // *********************************************************

    /// Transaction open date
    DateTime* m_TAOpenDate;
    /// Transaction close date
    DateTime* m_TACloseDate;
    /// Transaction open signal
    double m_TAOpenSignal;
    /// Transaction close signal
    double m_TACloseSignal;
    /// Shares in transaction
    double m_currentSharesCount;
    /// Transaction open price
    double m_TAOpenPrice;
    /// Transaction close price
    double m_TAClosePrice;
    /// # of days in position
    int m_inPosition;
    /// Flag if the transaction is for a long/short position
    bool m_longFlag;
    /// Flag if transaction is closed
    bool m_closedFlag;

};


#endif //DOROTHY_TRANSACTION_H
