module;
#include <memory>
#include <span>
export module nx.rhi.device;
export import nx.rhi.queue;
export import nx.rhi.shader;

export
namespace nx {



    class Device {
    public:
        virtual ~Device() = default;
        virtual void Initialize()=0;
        virtual Queue* GetQueue(QueueType type) = 0;

        virtual std::shared_ptr<Shader> CreateShader(std::span<std::byte> data)=0;

    };


} // namespace nx