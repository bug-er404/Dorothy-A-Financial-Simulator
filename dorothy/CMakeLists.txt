cmake_minimum_required(VERSION 3.16)
project(dorothy)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_VERBOSE_MAKEFILE ON)


add_executable(dorothy
        main.cpp
        PrefixHeader.pch
        DateTime.cpp
        DateTime.h
        Config.cpp
        Config.h
        DataAccess.cpp
        DataAccess.h
        TickerData.cpp
        TickerData.h
        Simulator.cpp
        Simulator.h TradingStock.cpp TradingStock.h Utilities.cpp Utilities.h Transaction.cpp Transaction.h)