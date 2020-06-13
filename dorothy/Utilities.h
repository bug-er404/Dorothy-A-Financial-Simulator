/**
     *  Utilities.h
     *  Namespace with utility functions for the simulator.
     *
     *
     *  Created by Salil Maharjan on 5/04/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#ifndef DOROTHY_UTILITIES_H
#define DOROTHY_UTILITIES_H
#include "PrefixHeader.pch"

namespace Utilities
{
    /// Function to round off value to the lower integral value.
    int RoundOff( double a_value );

    /// Function to get average of a list with doubles.
    double GetAverage( std::vector<double> a_list );

    /// Function to get the standard deviation of a list with doubles.
    double GetStandardDeviation( std::vector<double> a_list, double a_average );

    /// Method to trim leading and trailing blanks while reading data.
    void trimBlanks( std::string &a_str );
};


#endif //DOROTHY_UTILITIES_H


// *********************************************************
// ******************** CLASS CONSTANTS ********************
// *********************************************************

// *********************************************************
// ******************** CLASS VARIABLES ********************
// *********************************************************



// *********************************************************
// *********************** ACCESSORS ***********************
// *********************************************************

// *********************************************************
// *********************** MUTATORS ************************
// *********************************************************

// *********************************************************
// ********************* CONSTRUCTORS **********************
// *********************************************************

// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************

// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************