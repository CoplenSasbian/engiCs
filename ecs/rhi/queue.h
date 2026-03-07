#pragma once

namespace nx {

    enum QueueType {
        Direct,
        Compute,
        Transfer,
        Present
    };


    class Queue {
    public:
        using enum QueueType;
        virtual ~Queue() = default;

    };
}