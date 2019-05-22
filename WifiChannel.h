//
// Created by zeronsix on 5/12/19.
//

#ifndef DCFSIMULATOR_WIFICHANNEL_H
#define DCFSIMULATOR_WIFICHANNEL_H

#include "Simulator.h"
#include "Frame.h"
#include <vector>

class Node;

class WifiChannel {
public:
    enum class State {
        Idle,
        Busy,
        Collision
    };

    WifiChannel();
    void SetState(WifiChannel::State state);
    State GetState() const;
    Time GetLastStateChangeTime() const;
    Time GetLastIdleChangeTime() const;
    bool WasIdleDuring(Time time) const;
    void Send(FramePtr frame);
    void EndTx(FramePtr frame);
    void AddListener(Node *node);
private:
    State m_state;
    State m_prevState;
    Time m_lastStateChangeTime;
    Time m_lastIdleChangeTime;
    std::vector<Node*> m_listeners;
};


#endif //DCFSIMULATOR_WIFICHANNEL_H
