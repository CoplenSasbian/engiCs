module;
#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <mutex>
#include <algorithm>
#include <ranges>
#endif


module nx.concurrency.utils.processor_info;
import nx.concurrency.error_code;

#ifdef WIN32

struct ProcessorInfo
{
    ULONG64 mask;
    WORD group;
    BYTE EfficiencyClass;
};

static nx::Result<std::vector<ProcessorInfo> > _CollectProcessInfo() noexcept
{
    static std::vector<ProcessorInfo> result;
    static nx::Error err;
    static std::once_flag once_flag;

    std::call_once(once_flag, []
    {
        DWORD bufferSize = 0;
        BOOL res = GetLogicalProcessorInformationEx(
            RelationProcessorCore, nullptr, &bufferSize);
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            err = nx::make_system_error();
            return;
        }

        auto buffer = std::make_unique<std::byte[]>(bufferSize);
        auto ptr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.get());

        res = GetLogicalProcessorInformationEx(
            RelationProcessorCore, ptr, &bufferSize);
        if (!res)
        {
            err = nx::make_system_error();
            return;
        }

        auto currentPtr = ptr;
        DWORD offset = 0;

        while (offset < bufferSize)
        {
            if (currentPtr->Relationship == RelationProcessorCore)
            {
                BYTE efficiencyClass = currentPtr->Processor.EfficiencyClass;
                for (WORD i = 0; i < currentPtr->Processor.GroupCount; i++)
                {
                    WORD group = currentPtr->Processor.GroupMask[i].Group;
                    DWORD_PTR affinityMask = currentPtr->Processor.GroupMask[i].Mask;

                    for (int bit = 0; bit < sizeof(DWORD_PTR) * 8; bit++)
                    {
                        if (affinityMask & (1ULL << bit))
                        {
                            result.emplace_back(
                                1ULL << bit,
                                group,
                                efficiencyClass
                            );
                        }
                    }
                }
            }

            offset += currentPtr->Size;
            currentPtr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(
                reinterpret_cast<BYTE*>(currentPtr) + currentPtr->Size);
        }
        std::ranges::sort(result, std::ranges::greater{}, &ProcessorInfo::EfficiencyClass);
    });


    if (err)
        return  nx::UnExpected(err.value());

    return result;
}


nx::Result<std::span<uint64_t>> nx::GetCoreMask(CoreEfficiencyClass c) noexcept
{
    static std::vector<uint64_t> high;
    static std::vector<uint64_t> low;
    static std::once_flag once_flag;
    static Error err;
    std::call_once(once_flag, []
    {
        auto ret = _CollectProcessInfo();
        if (!ret)
        {
            err = ret.error();
            return;
        }

        auto info = ret.value();
        high.append_range(
            info
            | std::ranges::views::take_while([ef1 = info.front().EfficiencyClass](auto& i)
            {
                return i.EfficiencyClass == ef1;
            })
            | std::views::transform(&ProcessorInfo::mask)
        );

        auto dropNums = high.size() == info.size() ? info.size() / 2 : high.size();

        low.append_range(
            info
            | std::views::drop(dropNums)
            | std::views::transform(&ProcessorInfo::EfficiencyClass)
        );
    });

    if ( err)
    {
        return nx::UnExpected(err.value());
    }

    return c == High ? high : low;
}

nx::Error nx::SetCurrentThreadAffinity(uint64_t mask) noexcept
{
    ::SetThreadAffinityMask(GetCurrentThread(), mask);
    return nx::NoError;
}
#endif
