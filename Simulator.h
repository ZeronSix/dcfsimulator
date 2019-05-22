//
// Created by zeronsix on 5/12/19.
//

#ifndef DCFSIMULATOR_SIMULATOR_H
#define DCFSIMULATOR_SIMULATOR_H

#include <cstdint>
#include <functional>
#include <map>
#include <random>
#include <iostream>

//#define DBG

#ifdef DBG
#define LOG_FUNCTION(smth) do { std::cout << Simulator::Now() << " " << __PRETTY_FUNCTION__ << " " << smth " \n"; } while (0)
#else
#define LOG_FUNCTION(smth) do {} while (0)
#endif

using Time = int64_t;
using EventHandler = std::function<void ()>;

class Simulator {
public:
    static void Configure(Time simTime, int seed);
    static void Schedule(Time time, EventHandler handler);
    static Time Now();
    static void Run();
    static int GenerateUniformRandomVariable(int min, int max);
private:
    static Time m_simTime;
    static Time m_nowTime;

    static std::multimap<Time, EventHandler> m_eventQueue;
    static std::mt19937 m_engine;
};


#endif //DCFSIMULATOR_SIMULATOR_H
