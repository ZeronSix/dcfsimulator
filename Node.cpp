//
// Created by zeronsix on 5/12/19.
//

#include "Node.h"

int Node::nodeCounter = 0;

Node::Node(WifiChannel *channel, int mcs):
    m_channel{channel},
    m_id{nodeCounter++},
    m_backoffCounter{0},
    m_cw{DcfParams::CW0},
    m_mcs{mcs},
    m_rxBytes{0},
    m_txBytes{0},
    m_state{State::Idle},
    m_lastEifsEnd{std::numeric_limits<Time>::min()},
    m_lastBackoffSlotTime{-DcfParams::BACKOFF_SLOT},
    m_currentReceiver{nullptr}
{
    m_channel->AddListener(this);
}

void Node::Send(Node *receiver) {
    LOG_FUNCTION();
    m_currentReceiver = receiver;
    m_state = State::Backoff;

    if (m_channel->GetState() != WifiChannel::State::Idle) {
        if (m_backoffCounter == 0) {
            UpdateBackoffCounter();
        }
        return;
    }

    if (m_channel->WasIdleDuring(DcfParams::DIFS) && m_backoffCounter == 0) {
        DoSend(receiver);
        return;
    }

    Time beenIdle = Simulator::Now() - m_channel->GetLastIdleChangeTime();
    if (beenIdle < DcfParams::DIFS) {
        if (m_backoffCounter == 0) {
            UpdateBackoffCounter();
        }
        Simulator::Schedule(DcfParams::DIFS - beenIdle, std::bind(&Node::Send, this, receiver));
        return;
    }

    //Simulator::Schedule(DcfParams::BACKOFF_SLOT, std::bind(&Node::HandleBackoffSlot, this));
    HandleBackoffSlot();
}

void Node::Receive(FramePtr frame) {
    LOG_FUNCTION(m_id << " rcv type:" << (int)frame->GetType()<< );
    if (frame->GetType() == Frame::Type::Data) {
        FramePtr ack = std::make_shared<Frame>(Frame::Type::Ack, this, frame->GetSender());
        Simulator::Schedule(DcfParams::SIFS, std::bind(&WifiChannel::Send, m_channel, ack));

        m_rxBytes += frame->GetPayload();
    }
    else if (frame->GetType() == Frame::Type::Ack) {
        m_state = State::Idle;
        m_cw = DcfParams::CW0;
        m_txBytes += 1000;
        Send(frame->GetSender());
        UpdateBackoffCounter();
    }
}

void Node::HandleAckTimeout() {
    LOG_FUNCTION();

    m_state = State::Backoff;
    int cwMax = DcfParams::CW_MAX;
    m_cw = std::min((m_cw + 1) * 2 - 1, cwMax);
    UpdateBackoffCounter();

    HandleBackoffSlot();
}

void Node::HandleBackoffSlot() {
    LOG_FUNCTION(m_id << " " << m_backoffCounter <<);

    if (Simulator::Now() < m_lastEifsEnd) {
        return;
    }

    if (!m_channel->WasIdleDuring(DcfParams::DIFS)) {
        return;
    }

    if (Simulator::Now() - m_lastBackoffSlotTime == 9 && m_backoffCounter > 0) {
        m_backoffCounter--;
    }

    if (m_backoffCounter == 0) {
        DoSend(m_currentReceiver);
        return;
    }

    m_lastBackoffSlotTime = Simulator::Now();
    Simulator::Schedule(DcfParams::BACKOFF_SLOT, std::bind(&Node::HandleBackoffSlot, this));
}

void Node::DoSend(Node *receiver) {
    LOG_FUNCTION();

    m_state = State::Tx;
    FramePtr frame = std::make_shared<Frame>(Frame::Type::Data, this, receiver);
    m_channel->Send(frame);
}

int Node::GetMcs() const {
    return m_mcs;
}

void Node::UpdateBackoffCounter() {
    m_backoffCounter = Simulator::GenerateUniformRandomVariable(0, m_cw);
}

std::size_t Node::GetRxBytes() const {
    return m_rxBytes;
}

std::size_t Node::GetTxBytes() const {
    return m_txBytes;
}

void Node::NotifyChannelStateChange(WifiChannel::State newState, WifiChannel::State prevState) {
    LOG_FUNCTION();

    if (m_state == State::Backoff) {
        if (prevState == WifiChannel::State::Busy && newState == WifiChannel::State::Idle) {
            Simulator::Schedule(DcfParams::DIFS, std::bind(&Node::HandleBackoffSlot, this));
        }
        else if (prevState == WifiChannel::State::Collision && newState == WifiChannel::State::Idle) {
            m_lastEifsEnd = Simulator::Now() + DcfParams::EIFS;
            Simulator::Schedule(DcfParams::EIFS, std::bind(&Node::HandleBackoffSlot, this));
        }
    }
    else if (m_state == State::Tx) {
        if (prevState == WifiChannel::State::Collision) {
            Simulator::Schedule(DcfParams::ACK_TIMEOUT, std::bind(&Node::HandleAckTimeout, this));
        }
    }
}
