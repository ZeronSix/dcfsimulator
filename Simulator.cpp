//
// Created by zeronsix on 5/12/19.
//

#include "Simulator.h"

Time Simulator::m_simTime = 0;
Time Simulator::m_nowTime = 0;
std::multimap<Time, EventHandler> Simulator::m_eventQueue;
std::mt19937 Simulator::m_engine;

void Simulator::Configure(Time simTime, int seed) {
    m_simTime = simTime;
    m_engine.seed(seed);
}

void Simulator::Schedule(Time time, EventHandler handler) {
    LOG_FUNCTION();

    m_eventQueue.insert(std::pair<Time, EventHandler>{m_nowTime + time, handler});
}

Time Simulator::Now() {
    return m_nowTime;
}

void Simulator::Run() {
    while (!m_eventQueue.empty() && m_nowTime <= m_simTime) {
        auto it = m_eventQueue.begin();
        if (it->first > m_simTime) {
            m_eventQueue.clear();
            break;
        }

        m_nowTime = it->first;
        it->second();
        m_eventQueue.erase(it);
    }
}

int Simulator::GenerateUniformRandomVariable(int min, int max) {
    std::uniform_int_distribution<> distribution{min, max};

    return distribution(m_engine);
}

