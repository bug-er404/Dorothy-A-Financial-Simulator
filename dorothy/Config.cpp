/**
     *  Config.cpp
     *  Implementation of Config.h.
     *
     *
     *  Created by Salil Maharjan on 4/25/20.
     *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#include "Config.h"

// *********************************************************
// ***************** PUBLIC CLASS METHODS ******************
// *********************************************************

/**
    * Config::Load
    * Method to load configuration file. Uses parseLine method to parse lines.
    * @param a_file char* Config file name.
    * @param a_displayParameters bool Flag for display purposes
    * @return bool Function execution status.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
bool Config::Load( const char *a_file, bool a_displayParameters )
{
    // Check and open file:
    std::ifstream t_inStr(a_file);
    if (!t_inStr)
    {
        std::cerr << "Dorothy Error (Load): Cannot read file " << a_file << std::endl;
        return false;
    }

    // Placeholder string variables:
    std::string t_line, t_name, t_value;
    // Parse config file
    while (!t_inStr.eof())
    {
        // Get line and skip if it is a comment:
        getline(t_inStr, t_line);
        if (!parseLine(t_line, t_name, t_value))
            continue;

        // Construct and add config parameter:
        ConfigValue t_configVal;
        t_configVal.value = t_value;
        m_ConfigData[t_name] = t_configVal;
    }

    // If display flag is set:
    if (a_displayParameters)
    {
        std::cout<<"Dorothy: Configurations loaded successfully."<<std::endl;
        DisplayParameters();
    }

    return true;
}


/**
    * Config::GetParameter
    * Template function specialization of function (GetParameter) for returning parameter value as char*.
    * @param a_parameter string Parameter name.
    * @param a_value char* Variable to save parameter value.
    * @return bool Function execution status.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
bool Config::GetParameter( std::string a_parameter, char *a_value )
{
    // Validate parameter key/value:
    std::string t_value;
    if (!getParameterValue(a_parameter, t_value))
    {
        std::cerr << "Dorothy Error (GetParameter): Value not accessable. Parameter: " << a_parameter << std::endl;
        return false;
    }
    a_value = strcpy(new char[t_value.length() + 1], t_value.c_str());
    return true;
}

/**
    * Config::GetParameter
    * Template function specialization of function (GetParameter) for returning parameter value as booleans.
    * @param a_parameter string Parameter name.
    * @param a_value bool Variable to save parameter value.
    * @return bool Function execution status.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
bool Config::GetParameter( std::string a_parameter, bool &a_value )
{
    // Validate parameter key/value:
    std::string t_value;
    if (!getParameterValue(a_parameter, t_value))
    {
        std::cerr << "Dorothy Error (GetParameter): Value not accessable. Parameter: " << a_parameter << std::endl;
        return false;
    }

    transform(t_value.begin(), t_value.end(), t_value.begin(), ::toupper);
    //checking the value if true or false
    if (t_value == "1")
        a_value = true;
    else if (t_value == "0")
        a_value = false;
    else
    {
        std::cerr<<"Dorothy Error (GetParameter): Cannot determine bool val"<<std::endl;
        return false;
    }
    return true;
}


/**
    * Config::GetUnaccessedParameters
    * Method to provide the list of the parameters that were not accessed.
    * @param a_paramNames vector<string> Names of parameters that were not accessed.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void Config::GetUnaccessedParameters( std::vector<std::string> &a_paramNames )
{
    for(auto t_param : m_ConfigData)
    {
        if(!(t_param.second.isAccessed))
            a_paramNames.push_back(t_param.first);
    }
}

/**
    * Config::DisplayParameters
    * Method to display the parameters in alphabetical order.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void Config::DisplayParameters()
{
    // Using map to sort
    std::map<std::string, ConfigValue> t_map;
    t_map.insert(m_ConfigData.begin(), m_ConfigData.end());

    std::cout<<"******************************************"<<std::endl;
    std::cout<<"Dorothy - Configuration Parameters: "<<std::endl;
    int count = 0;
    for(auto t_param : t_map)
        std::cout<<"["<<++count<<"]   "<<t_param.first<<std::endl;

    std::cout<<"******************************************"<<std::endl;
}

// *********************************************************
// *************** PRIVATE UTILITY METHODS *****************
// *********************************************************

/**
    * Config::parseLine
    * Method to parse configuration file line into parameter name and value.
    * @param a_line string The line read from configuration file.
    * @param a_name string Variable to save parameter name by reference.
    * @param @a_value string Variable to save parameter value by reference.
    * @return bool Function execution status.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
bool Config::parseLine( const std::string &a_line, std::string &a_name, std::string &a_value )
{
    // Flag if comment is present in the line
    bool t_commentFlag = false;
    // String position variable
    size_t t_idx;

    // Check for valid configuration line
    if ((t_idx=a_line.find_first_of("=")) == std::string::npos
        || (std::count(a_line.begin(), a_line.end(), '=') > 1))
        return false;

    // Check for comments
    if(a_line.find_first_of("#") != std::string::npos)
    {
        // Skip whitespace and check if the line is only a comment
//        trimBlanks(a_line);
        if(a_line[0] == '#')
            return false;

        t_commentFlag = true;
    }

    // Get first half, remove whitespace and make upper.
    a_name = a_line.substr(0, t_idx);
    trimBlanks(a_name);
    transform(a_name.begin(), a_name.end(), a_name.begin(), ::toupper);


    // Get second half and remove white space
    a_value = a_line.substr(t_idx+1, (a_line.size()-t_idx));
    trimBlanks(a_value);

    // Check for trailing comments and removing it
    if(t_commentFlag)
    {
        t_idx = a_value.find("#");
        a_value = a_value.substr(0, t_idx);
    }

    return true;
}

/**
    * Config::getParameterValue
    * Private utility function to get parameter value as a string. Used by GetParameter.
    * @param a_name string Parameter name.
    * @param a_value bool Parameter value.
    * @return bool Function execution status.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
bool Config::getParameterValue( std::string a_name, std::string &a_value )
{
    // Validating key
    transform(a_name.begin(), a_name.end(), a_name.begin(), ::toupper);
    if(m_ConfigData.find(a_name) == m_ConfigData.end())
    {
        std::cerr<<"Dorothy Error (getParameterValue): Parameter not found ["<<a_name<<"]"<<std::endl;
        return false;
    }
    else
    {
        m_ConfigData[a_name].isAccessed = true;
        a_value = m_ConfigData[a_name].value;
        return true;
    }
}

/**
    * Config::trimBlanks
    * Method to trim leading and trailing whitespaces.
    * @param a_str string Reference to string to trim white spaces.
    * @author Salil Maharjan
    * @date 4/30/20.
*/
void Config::trimBlanks( std::string &a_str )
{
    if(a_str.empty())
    {
        std::cerr<<"Dorothy Error (Config::trimBlanks): Configuration file is not complete"<<std::endl;
        exit(1);
    }
    size_t t_lead = a_str.find_first_not_of(" \t\n");
    a_str = a_str.substr(t_lead, a_str.find_last_not_of(" \t\n") - t_lead + 1);
}

// *********************************************************
// ******************** TRASH METHODS **********************
// *********************************************************

//    // --------------------------------------------------------------------------------------------------------
//    // CODE WRITTEN FOR A SINGLE TEMPLATE FUNCTION THAT WOULD NOT REQUIRE TEMPLATE SPECIALIZATION
//    // WORKS IN LINUX ENVIRONMENT. UNSURE ABOUT CROSS-PLATFORM COMPATIBILITY - LEFT UNUSED.
//    // Check if boolean is passed as T class
//    if(strncmp(typeid(a_value).name(), "b", 1)==0)
//    {
//        transform(t_value.begin(), t_value.end(), t_value.begin(), ::toupper);
//        //checking the value if true or false
//        if (t_value == "TRUE")
//            a_value = true;
//
//        else if (t_value == "FALSE")
//            a_value = false;
//        else
//        {
//            cerr<<"Dorothy Error (GetParameter): Cannot determine bool val"<<endl;
//            return false;
//        }
//        return true;
//    }
//
//    // Check if char* is passed as T class
//    if(strncmp(typeid(a_value).name(), "Pc", 2)==0)
//    {
//        a_value = strcpy(new char[t_value.length() + 1], t_value.c_str());
//        return true;
//    }
//    // --------------------------------------------------------------------------------------------------------