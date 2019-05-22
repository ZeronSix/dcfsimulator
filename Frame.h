//
// Created by zeronsix on 5/12/19.
//

#ifndef DCFSIMULATOR_FRAME_H
#define DCFSIMULATOR_FRAME_H


#include <cstddef>
#include <memory>
#include "Simulator.h"

class Node;

class Frame {
public:
    enum class Type {
        Data,
        Ack
    };

    Frame(Type type, Node *sender, Node *receiver);
    Time GetDuration(int mcs) const;
    Node *GetSender() const;
    Node *GetReceiver() const;
    Type GetType() const;
    std::size_t GetPayload() const;
private:
    std::size_t m_payload;
    std::size_t m_overhead;
    Node *m_sender;
    Node *m_receiver;
    Type m_type;
};

using FramePtr = std::shared_ptr<Frame>;


#endif //DCFSIMULATOR_FRAME_H
