/**
     *  Utilities.cpp
     *  Implementation of Utilities.h.
     *
     *
     *  Created by Salil Maharjan on 5/04/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/


#include "Utilities.h"
#include <cmath>

/**
    * Utilities::RoundOff
    * Function to round off value to integer value. Uses floor.
    * @param a_value double Value to round off.
    * @return int Integral value of a_value left rounded.
    * @author Salil Maharjan
    * @date 5/04/20.
*/
int Utilities::RoundOff( double a_value )
{
    // Simply uses floor function currently.
    return ((int)floor(a_value));
}

/**
    * Utilities::GetAverage
    * Function to get average of a list with doubles.
    * @param a_list vector<double> List of doubles
    * @return double The mean of the list.
    * @author Salil Maharjan
    * @date 5/04/20.
*/
double Utilities::GetAverage( std::vector<double> a_list )
{
    double t_sum = 0;
    for (double val : a_list)
        t_sum += val;
    return t_sum / a_list.size();
}

/**
    * Utilities::GetStandardDeviation
    * Function to get the standard deviation of a list with doubles.
    * @param a_list vector<double> List of doubles.
    * @param a_average double The mean of the list.
    * @return double The standard deviation of the list.
    * @author Salil Maharjan
    * @date 5/04/20.
*/
double Utilities::GetStandardDeviation( std::vector<double> a_list, double a_average)
{
    // get square distance
    double t_diff;

    for (double t_val : a_list)
        t_diff += std::pow(t_val - a_average, 2);

    t_diff /= a_list.size();

    return std::sqrt(t_diff);
}

/**
    * Utilities::trimBlanks
    * Method to trim leading and trailing whitespaces.
    * @param a_str string Reference to string to trim white spaces.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void Utilities::trimBlanks( std::string &a_str )
{
    size_t t_lead = a_str.find_first_not_of(" \t\n");
    a_str = a_str.substr(t_lead, a_str.find_last_not_of(" \t\n") - t_lead + 1);
}