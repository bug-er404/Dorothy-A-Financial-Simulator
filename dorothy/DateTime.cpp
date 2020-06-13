/**
 *  DateTime.cpp
 *  Implementation of DateTime.hpp
 *
 *
 *  Created by Salil Maharjan on 3/22/20.
 *  Copyright © 2020 Salil Maharjan. All rights reserved.
 */

#include "DateTime.h"
#include "PrefixHeader.pch"

// Initialize static variables.
int DateTime::m_FakeTodayValue = 0;
int DateTime::dayPreMonth[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
int DateTime::daysInMonth[13]= { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************

/**
    * DateTime::Set
    * Date set function for General date format parameterized constructor (YYYY MM DD)
    * @param a_year int Year
    * @param a_month int Month
    * @param a_day int Date
    * @author Salil Maharjan
    * @date 3/22/20.
*/
void DateTime::Set( int a_year, int a_month, int a_day )
{
    // Checking values are in range
    assert(checkDateValueRanges(a_year, a_month, a_day));

    // Set date
    m_date = a_year*10000 + a_month*100 + a_day;
    // Take 11 days skipped in September 1752 into consideration?
}

/**
    * DateTime::Set
    * Date set function for Internal date format parameterized constructor
    * @param a_date int Date passed in internal format YYYYMMDD
    * @author Salil Maharjan
    * @date 3/24/20.
*/
void DateTime::Set( int a_date )
{
    // Checking values are in range
    assert(checkDateValueRanges( ((int)(a_date/10000)), ((int)((a_date/100)%100)), ((int)(a_date%100)) ));

    // Set date
    m_date = a_date;
}


/**
    * DateTime::checkDateValueRanges
    * Function to assert date value ranges
    * @param a_year int Year
    * @param a_month int Month
    * @param a_day int Date
    * @return bool If passed values mark a valid date.
    * @author Salil Maharjan
    * @date 3/24/20.
*/
bool DateTime::checkDateValueRanges ( int a_year, int a_month, int a_day )
{
    // Asserting values in range
    assert(a_year > 0 && a_year <= 99999);
    assert(a_month > 0 && a_month < 13);
    assert(a_day > 0 && a_day < 32);

    // Month specific day range check:
    // February and leap year check.
    if(a_month==2)
    {
        // Leap year 29 days else 28 days in February.
        if(a_year%4==0)
            assert(a_day<30);
        else
            assert(a_day<29);
    }
        // Months that have 30 days. ( 4 6 9 11 )
    else if(a_month==4 || a_month==6 || a_month==9 || a_month==11)
        assert(a_day<31);
    // Months that have 31 days. ( 1 3 5 7 8 10 12 ) * Range is already checked *

    return true;
}

/**
    * DateTime::SetToday
    * Record today's local date in this object -  uses faked today's date if set.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void DateTime::SetToday( )
{
    // Not set.
    if(m_FakeTodayValue == 0)
        SetActualToday();
    else
    {
        m_date = m_FakeTodayValue;
        m_FakeTodayValue = 0;
    }
}

/**
    * DateTime::SetToday
    * Record today's local date in this object.  Does not use faked today's date.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
void DateTime::SetActualToday(  )
{
    time_t t_now = time(0);
    tm *t_localTime = localtime(&t_now);
    m_date = (1900 + t_localTime->tm_year)*10000 + (1 + t_localTime->tm_mon)*100 + (t_localTime->tm_mday);
}

/**
    * DateTime::operator =
    * Assignment operator overload.
    * @param a_date DateTime DateTime object to assign.
    * @return DateTime& New DateTime that is assigned.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
DateTime& DateTime::operator =(int a_date )
{
    // Checking values are in range
    assert(checkDateValueRanges( ((int)(a_date/10000)), ((int)((a_date/100)%100)), ((int)(a_date%100)) ));
    this->m_date = a_date;
    return *this;
}

// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

/**
    * DateTime::reverseOffset
    * Reverse date offset
    * @param a_days int Number of days to reverse offset.
    * @return DateTime The date after reversing a_days offset.
    * @author Salil Maharjan
    * @date 5/12/20.
*/
DateTime DateTime::reverseOffset( int a_days )
{
    /// Change the return value!
    if(a_days < 0)
    {
        std::cerr << "Value out of range error!\n";
        return *this;
    }

    DateTime t_date;

    int temp_day = 0;
    int temp_year = 0;

    while(1)
    {
        if(temp_year % 4 == 0)
        {
            if((temp_day + 366) > a_days) break;
            temp_day += 366;
        }
        else
        {
            if((temp_day + 365) > a_days) break;
            temp_day += 365;
        }
        temp_year ++;
    }

    int temp_month = 1;

    while(1)
    {
        if((temp_day + daysInMonth[temp_month]) > a_days) break;
        temp_day += daysInMonth[temp_month];
        temp_month ++;
    }

    t_date.Set(temp_year, temp_month, (a_days - temp_day));

    return t_date;
}

// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************