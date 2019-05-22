//
// Created by zeronsix on 5/12/19.
//

#include "Frame.h"

Time Frame::GetDuration(int mcs) const {
    LOG_FUNCTION();

    int r = mcs * 4;
    Time duration = 16 + ((m_payload * 8 + m_overhead * 8) / r + 1) * 4;
    return duration;
}

Frame::Frame(Type type, Node *sender, Node *receiver):
    m_type{type},
    m_sender{sender},
    m_receiver{receiver} {
    switch (type) {
        case Type::Data:
            m_payload = 1000;
            m_overhead = 22;
            break;
        case Type::Ack:
            m_payload = 0;
            m_overhead = 14;
            break;
    }
}

Node *Frame::GetSender() const {
    return m_sender;
}

Node *Frame::GetReceiver() const {
    return m_receiver;
}

Frame::Type Frame::GetType() const {
    return m_type;
}

std::size_t Frame::GetPayload() const {
    return m_payload;
}
