//
// Created by zeronsix on 5/12/19.
//

#include "WifiChannel.h"
#include "Node.h"
#include <limits>

WifiChannel::WifiChannel():
    m_state{State::Idle},
    m_prevState{State::Idle},
    m_lastStateChangeTime{0},
    m_lastIdleChangeTime{0}
{

}

void WifiChannel::SetState(WifiChannel::State state) {
    LOG_FUNCTION("channel set to " << static_cast<int>(state) << );

    if (state == m_state) {
        return;
    }

    for (auto node : m_listeners) {
        //Simulator::Schedule(0, std::bind(&Node::NotifyChannelStateChange, node, state, m_state));
        node->NotifyChannelStateChange(state, m_state);
    }

    m_prevState = m_state;
    m_state = state;
    m_lastStateChangeTime = Simulator::Now();

    switch (state) {
        case State::Idle:
            m_lastIdleChangeTime = Simulator::Now();
            break;
        default:
            break;
    }
}

WifiChannel::State WifiChannel::GetState() const {
    return m_state;
}

Time WifiChannel::GetLastStateChangeTime() const {
    return m_lastStateChangeTime;
}

bool WifiChannel::WasIdleDuring(Time time) const {
    //LOG_FUNCTION();

    if (Simulator::Now() == 0) {
        return true;
    }

    if (m_state == State::Idle && (Simulator::Now() - m_lastIdleChangeTime) >= time) {
        return true;
    }
    else if (m_prevState == State::Idle && (Simulator::Now() == m_lastStateChangeTime) &&
             (Simulator::Now() - m_lastIdleChangeTime) >= time) {
        return true;
    }

    return false;
}

void WifiChannel::Send(FramePtr frame) {
    LOG_FUNCTION(" send type:" << (int)frame->GetType()<< );

    if (m_state == WifiChannel::State::Idle) {
        SetState(WifiChannel::State::Busy);
        Simulator::Schedule(frame->GetDuration(frame->GetSender()->GetMcs()),
                            std::bind(&WifiChannel::EndTx, this, frame));
    }
    else if (m_state == WifiChannel::State::Busy) {
        SetState(WifiChannel::State::Collision);
    }
}

void WifiChannel::EndTx(FramePtr frame) {
    LOG_FUNCTION();

    SetState(WifiChannel::State::Idle);
    if (m_prevState == WifiChannel::State::Busy) {
        frame->GetReceiver()->Receive(frame);
    }
}

void WifiChannel::AddListener(Node *node) {
    m_listeners.push_back(node);
}

Time WifiChannel::GetLastIdleChangeTime() const {
    return m_lastIdleChangeTime;
}
