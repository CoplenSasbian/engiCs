module;
#include <span>
export module nx.concurrency.utils.processor_info;

export namespace nx {

    enum CoreEfficiencyClass {
        High,
        Low
    };

    std::span<uint64_t> GetCoreMask(CoreEfficiencyClass);
}



