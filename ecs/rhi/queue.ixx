//
// Created by futur on 2026/2/13.
//

export module nx.rhi.queue;
export

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