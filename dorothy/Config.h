/**
     *  Config.h
     *  Interface for the Config class.
     *  Loads configurations from file to use for simulation.
     *
     *
     *  Created by Salil Maharjan on 4/25/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#ifndef Config_hpp
#define Config_hpp

#pragma once

#include "PrefixHeader.pch"

class Config
{
public:

    // *********************************************************
    // ********************* CONSTRUCTORS **********************
    // *********************************************************

    /// Default constructor
    Config() {};

    /// Main class constructor
    Config( const char* a_file ){ Load(a_file, true); }

    /// Virtual deconstructor
    virtual ~Config() {};

    // *********************************************************
    // *********************** ACCESSORS ***********************
    // *********************************************************

    /// Template function to access parameters.
    template<class T>
    bool GetParameter( std::string a_parameter, T &a_value );
    /// Template function specializations to access parameters as char* and bool.
    bool GetParameter( std::string a_parameter, char *a_value );
    bool GetParameter( std::string a_parameter, bool &a_value );

    // *********************************************************
    // ***************** PUBLIC CLASS METHODS ******************
    // *********************************************************

    /// Load in data from a specified configuration file. Allows multiple calls.
    bool Load( const char *a_file, bool a_displayParameters = true );

    /// Test if a config file has a parameter for a given segment.
    // Note: A parameter is considered accessed if someone asks if it exists.
    bool ContainsParameter( std::string a_parameter )
    {
        std::string value;
        return getParameterValue( a_parameter, value );
    }

    /// Provides a list of the parameters that were not accessed.
    void GetUnaccessedParameters( std::vector<std::string> &a_paramNames );

    /// Clear the set of recorded parameters.
    void ClearConfigData() { m_ConfigData.clear(); }

    /// Display the parameters in alphabetical order.
    void DisplayParameters();

private:

    // *********************************************************
    // ******************** CLASS VARIABLES ********************
    // *********************************************************

    /// Struct to hold configuration values and access flag
    struct ConfigValue
    {
        std::string value;
        // Variable to note whether or not the parameter was accessed.
        bool isAccessed;

        ConfigValue() { isAccessed = false; }
    };

    /// Map of config values
    std::unordered_map<std::string, ConfigValue> m_ConfigData;

    // *********************************************************
    // *************** PRIVATE UTILITY METHODS *****************
    // *********************************************************

    /// Trim leading and trailing blanks.
    void trimBlanks( std::string &a_str );

    /// Get parameters by parsing line from config file.
    bool parseLine( const std::string &a_line, std::string &a_name, std::string &a_value );

    /// Get the value of a specified parameter.
    bool getParameterValue( std::string a_name, std::string &a_value );
};

/**
    * Config::GetParameter
    * Template function to get parameter value and pass it by reference. Template supports: (string, int, short, long, float, double)
    * Explicit instantiation of template function on header file to prevent linker error.
    * Separate template function specialization functions defined for (bool, char*)
    * @param a_parameter string Parameter name.
    * @param a_value T Variable to save parameter value. Template supported types.
    * @return bool Function execution status.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
template<class T>
bool Config::GetParameter( std::string a_parameter, T &a_value )
{
    // Validate parameter key/value:
    std::string t_value;
    if (!getParameterValue(a_parameter, t_value))
    {
        std::cerr << "Dorothy Error (GetParameter): Value not accessable. Parameter: " << a_parameter << std::endl;
        return false;
    }

    // Code for single template function in trash method in file (Config.cpp)

    // Convert value to T type
    std::stringstream ss(t_value);
    ss >> a_value;

    return true;
}

#endif /* Config_hpp */
