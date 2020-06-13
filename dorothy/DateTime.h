/**
     *  DateTime.h
     *  Interface for the DateTime class.
     *  General date time class.  Dates represented internally as: ( year * 10000 + 100 * month + day )
     *
     *  Created by Salil Maharjan on 3/22/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/


#ifndef DateTime_h
#define DateTime_h

#pragma once
#include "PrefixHeader.pch"

class DateTime
{
public:

    // *********************************************************
    // ********************* CONSTRUCTORS **********************
    // *********************************************************

    /// Default constructor
    DateTime()
    {
        m_date = 0;
    }

    /// General date format parameterized constructor (YYYY MM DD)
    DateTime( int a_year, int a_month, int a_day )
    {
        Set( a_year, a_month, a_day );
    }

    /// Internal date format parameterized constructor
    DateTime( int a_date )
    {
        Set( a_date );
    }

    /// Copy constructor
    DateTime( const DateTime &a_date )
    {
        Set( a_date ) ;
    }

    // *********************************************************
    // ******************** CLASS CONSTANTS ********************
    // *********************************************************

    // Blank date const variable.
    static const int BlankDate = 0;

    // Defines the day of the week.
    // Char value since the max value is 7.
    enum class DAY_OF_WEEK : char
    {
        SUNDAY = 0,
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY,
        UNDEF_DOW
    };

    // *********************************************************
    // *********************** ACCESSORS ***********************
    // *********************************************************

    int GetYear() const { return m_date / 10000; }
    int GetMonth() const { return m_date / 100 % 100; }
    int GetDay() const { return m_date % 100; }

    /// Gets the date as one value.
    int Get() const { return m_date; }

    /// Gets the date as component values.
    void Get( int &a_year, int &a_month, int &a_day ) const
    {
        a_year = GetYear();
        a_month = GetMonth();
        a_day = GetDay();
    }

    /// Get date as an ASCII string in format "MM/DD/YYYY".
    std::string GetASCIIDate() { return ( std::to_string((m_date/100)%100)+ "/" + std::to_string(m_date%100) + "/" + std::to_string(m_date/10000) ); }

    /// Get day of the week as a string.
    static std::string GetStringDOW( DAY_OF_WEEK a_dow )
    {
        static std::string sdow[8] =
                { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Undefined" };

        if( a_dow < DAY_OF_WEEK::SUNDAY || a_dow > DAY_OF_WEEK::UNDEF_DOW )
            return "OutOfRange";

        return sdow[(int)a_dow];
    }

    /// Gets the Day of the week as integral value
    //  This algorithm works from 1901 until 2100...
    int GetDayOfWeek() const {

        int year = m_date / 10000;
        int mday = (year * 365) + ( (year - 1) / 4) + getJulianDay() + 6;
        return mday % 7;
    }

    // *********************************************************
    // *********************** MUTATORS ************************
    // *********************************************************

    void Set( int a_year, int a_month, int a_day );
    void Set( int a_date );
    void Set( const DateTime &a_date ) { m_date = a_date.Get(); }

    /// Function to assert date value ranges
    bool checkDateValueRanges ( int a_year, int a_month, int a_day );

    /// Set the fake today value.  This will be used instead of the real today.
    static void setFakeTodayValue( int a_val ) { m_FakeTodayValue = a_val; }

    /// Record today's local date in this object - uses faked today's date if set.
    void SetToday();

    /// Record today's local date in this object.  Does not use faked today's date.
    void SetActualToday() ;

    // *********************************************************
    // ***************** PUBLIC CLASS METHODS ******************
    // *********************************************************

    /// Determines if the year recorded here is a leap year.
    // (Professor's note: This code will work only between 1901 and 2099? (General version?))
    bool isLeapYear() const { return (m_date / 10000) % 4 == 0; }

    /// Gets the Julian day from this date.  Julian day vary from 0 to 365.
    int getJulianDay() const
    {
        int month = m_date / 100 % 100;
        int day = m_date % 100;
        int incre = (isLeapYear() && month > 2 ) ? 1 : 0;
        return dayPreMonth[month-1] + day + incre;
    }

    /// Computes the calendar difference between two date.
    // In order to optimize speed, this only works for dates between 1901 and 2099 inclusive.
    int CalendarDiffDates( const DateTime &a_nearDate, const DateTime &a_farDate )
    {
        int diff = dateOffset(a_nearDate) - dateOffset(a_farDate) - 1;
        return diff;
    }

    // *********************************************************
    // ****************** OPERATOR OVERLOADS *******************
    // *********************************************************

    /// Conversion operators.
    operator int() const { return m_date; }

    /// Assignment operators.
    DateTime& operator =(const DateTime &a_date)
    {
        this->m_date = a_date.m_date;
        return *this;
    }
    DateTime& operator =(int a_date);

    /// Finds the difference between two dates.
    int operator -(const DateTime &a_date) { return CalendarDiffDates(*this, a_date); }

    /// Subtracts a specified number of days to the date.
    DateTime operator -(int a_days) { return reverseOffset(dateOffset(*this)-a_days); }

    /// Adds a specified number of days to the date.
    DateTime operator +(int a_days) { return reverseOffset(dateOffset(*this)+a_days); }

    /// Comparison operator to compare two dates:
    bool operator ==(const DateTime &a_date) { return dateOffset(*this) == dateOffset(a_date); }
    bool operator ==(int a_date)
    {
        DateTime *t_date = new DateTime(a_date);
        return dateOffset(*this) == dateOffset(*t_date);
    }

    bool operator !=(const DateTime &a_date) { return dateOffset(*this) != dateOffset(a_date); }
    bool operator !=(int a_date)
    {
        DateTime *t_date = new DateTime(a_date);
        return dateOffset(*this) != dateOffset(*t_date);
    }

    bool operator <(const DateTime& a_date) { return dateOffset(*this) < dateOffset(a_date); }
    bool operator <(int a_date)
    {
        DateTime *t_date = new DateTime(a_date);
        return dateOffset(*this) < dateOffset(*t_date);
    }

    bool operator <= (const DateTime& a_date) { return dateOffset(*this) <= dateOffset(a_date); }
    bool operator <=(int a_date)
    {
        DateTime *t_date = new DateTime(a_date);
        return dateOffset(*this) <= dateOffset(*t_date);
    }

    bool operator >(const DateTime& a_date) { return dateOffset(*this) > dateOffset(a_date); }
    bool operator >(int a_date)
    {
        DateTime *t_date = new DateTime(a_date);
        return dateOffset(*this) > dateOffset(*t_date);
    }

    bool operator >=(const DateTime& a_date) { return dateOffset(*this) >= dateOffset(a_date); }
    bool operator >=(int a_date)
    {
        DateTime *t_date = new DateTime(a_date);
        return dateOffset(*this) >= dateOffset(*t_date);
    }

    /// Unary minus operators. (PREFIX)
    DateTime& operator --()
    {
        int mday = reverseOffset(dateOffset(*this)-1);
        Set(mday);
        return *this;
    }

    /// Unary minus operators. (POSTFIX)
    DateTime operator --(int)
    {
        DateTime d = *this;
        operator --();
        return d;
    }

    /// Unary plus operators. (PREFIX)
    DateTime& operator ++()
    {
        int mday = reverseOffset(dateOffset(*this)+1);
        Set(mday);
        return *this;
    }

    /// Unary plus operators. (POSTFIX)
    DateTime operator ++(int)
    {
        DateTime d = *this;
        operator ++();
        return d;
    }

private:

    // *********************************************************
    // ******************** CLASS VARIABLES ********************
    // *********************************************************

    /// Date stored as ( year * 10000 + 100 * month + day )
    int m_date;
    /// Fake date value of today.
    static int m_FakeTodayValue;
    /// The number of days since the beginning of the year to a given month.  (non-leap year)
    static int dayPreMonth[13];
    /// The number of days in a month. (non-leap year)
    static int daysInMonth[13];

    // *********************************************************
    // *************** PRIVATE UTILITY METHODS *****************
    // *********************************************************

    /// Get the offset from the year 0.
    int dateOffset( const DateTime &a_date )
    {  // This has a flaw
        int year = a_date.m_date / 10000;
        int offset = (year * 365) + ( (year - 1) / 4) + a_date.getJulianDay();
        return offset;
    }
    /// Reverse date offset
    DateTime reverseOffset( int a_days );
};

#endif /* DateTime_h */
