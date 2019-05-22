//
// Created by zeronsix on 5/12/19.
//

#ifndef DCFSIMULATOR_NODE_H
#define DCFSIMULATOR_NODE_H


#include "WifiChannel.h"

struct DcfParams {
    static const Time BACKOFF_SLOT = 9;
    static const Time SIFS = 16;
    static const Time PIFS = SIFS + BACKOFF_SLOT;
    static const Time DIFS = SIFS + BACKOFF_SLOT * 2;
    static const Time EIFS = SIFS + 36 + DIFS;
    static const Time ACK_TIMEOUT = EIFS; //SIFS + BACKOFF_SLOT + 25;
    static const int CW0 = 15;
    static const int CW_MAX = 2047;
};

class Node {
public:
    enum class State {
        Idle,
        Backoff,
        Tx
    };

    Node(WifiChannel *channel, int mcs);

    void Send(Node *receiver);
    void Receive(FramePtr frame);
    void HandleAckTimeout();
    void HandleBackoffSlot();
    void UpdateBackoffCounter();
    int GetMcs() const;
    std::size_t GetRxBytes() const;
    std::size_t GetTxBytes() const;
    void NotifyChannelStateChange(WifiChannel::State newState, WifiChannel::State prevState);
private:
    int m_id;
    int m_backoffCounter;
    int m_lastBackoffSlotTime;
    int m_cw;
    WifiChannel *m_channel;
    int m_mcs;
    std::size_t m_rxBytes;
    std::size_t m_txBytes;
    State m_state;
    Time m_lastEifsEnd;
    Node *m_currentReceiver;

    static int nodeCounter;
    void DoSend(Node *receiver);
};


#endif //DCFSIMULATOR_NODE_H
