/**
 *  main.cpp
 *  Main entry point for Dorothy financial simulator.
 *
 *
 *  Created by Salil Maharjan on 3/22/20.
 *  Copyright © 2020 Salil Maharjan. All rights reserved.
*/

#include "Simulator.h"

// *********************************************************
// **************** DOROTHY MAIN ENTRY *********************
// *********************************************************
int main(int argc, const char* argv[])
{
    Simulator dorothy(argc, argv);
    dorothy.StartSimulation();

    return 0;
}

// *********************************************************
// ****************** TRASH: TESTING CODE ******************
// *********************************************************

// ****************************
// CONFIG CLASS TESTING CODE:
// ****************************
//    //load config file on object creation -- display all parameters
//    Config configuration("config.txt");
//
//    cout << endl;
//    cout << "--------------------------------------------------------" << endl;
//    cout << "accessing two parameters in the configuration file:" << endl;
//
//    //display stop_loss parameters
//    string parameter = "stop_loss";
//    double stop_loss;
//    if (configuration.GetParameter(parameter, stop_loss))
//    {
//        cout << parameter << " = " << stop_loss;
//    }
//    else
//    {
//        cerr << "could not get value for " << parameter << endl;
//    }
//
//    cout << endl;
//
//    //display name of stock2
//    parameter = "stock2";
//    string name;
//    if (configuration.GetParameter(parameter, name))
//    {
//        cout << parameter << " = " << name;
//    }
//    else
//    {
//        cerr << "could not get value for " << parameter << endl;
//    }
//
//    cout << endl;
//    cout << "--------------------------------------------------------" << endl;
//    //display all unaccessed parameters stock 2 and stop_loss should not be there
//    cout << "All unaccessed parameters:" << endl;
//    vector<string> unacc_params;
//    configuration.GetUnaccessedParameters(unacc_params);
//    int count = 0;
//    for (auto i : unacc_params)
//        cout << ++count << ") "<< i << endl;
//
//    cout << endl;
//
//    //clear all data
//    configuration.clearConfigData();
//    cout << "All data has been cleared." << endl;
//
//    cout << endl;
//
//    system("pause");

// ****************************
// DATETIME CLASS TESTING CODE:
// ****************************
//    DateTime* time1 = new DateTime(20200430);
//    DateTime* time2 = new DateTime();
//    *time2 = 20200429;
//
//    std::cout<<*time1-*time2<<std::endl;
//    std::cout<<time1->GetASCIIDate()<<std::endl;
//    std::cout<<time2->GetASCIIDate()<<std::endl;
//
//    *time2 = *time1 - 375;
//    std::cout<<time2->GetASCIIDate()<<std::endl;
//
//    *time2 = *time1 + 120;
//    std::cout<<time2->GetASCIIDate()<<std::endl;
//
//    if(*time1 < *time2) std::cout << time1->GetASCIIDate() << " is before " << time2->GetASCIIDate() << std::endl;
//
//    time->setFakeTodayValue(19980102);
//    time->SetToday();
//    std::cout<<time->GetASCIIDate()<<std::endl;

//    std::cout<<time->getStringDOW(DateTime::DAY_OF_WEEK(-1))<<std::endl;
