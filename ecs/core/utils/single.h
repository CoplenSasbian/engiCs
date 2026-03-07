#pragma once
#include <algorithm>
#include <functional>
#include <shared_mutex>
#include <vector>
#include <memory>


namespace  nx {



    template<typename... Args>
    class Signal {
    public:
        using Callback = std::function<void(Args...)>;

        using Element = std::pair<uint64_t, Callback>;
        using Vector  = std::pmr::vector<Element>;

        Signal()
            : m_callbacks{MakeShared<Vector>()}
            , m_nextId{1}   // 从 1 开始，0 保留表示无效
        {}

        // 不可拷贝，可移动
        Signal(const Signal&) = delete;
        Signal& operator=(const Signal&) = delete;

        Signal(Signal&& other) noexcept
            : m_callbacks{other.m_callbacks.load()}
            , m_nextId{other.m_nextId.load()}
        {
            // 将 other 置于空状态
            other.m_callbacks.store(MakeShared<Vector>());
            other.m_nextId.store(1);
        }

        Signal& operator=(Signal&& other) noexcept {
            if (this != &other) {
                m_callbacks.store(other.m_callbacks.load());
                m_nextId.store(other.m_nextId.load());
                other.m_callbacks.store(MakeShared<Vector>());
                other.m_nextId.store(1);
            }
            return *this;
        }

        [[nodiscard]]
        uint64_t Add(Callback cb) {
            uint64_t id = m_nextId.fetch_add(1, std::memory_order_relaxed);
            std::shared_ptr<Vector> old, new_vec;
            do {
                old = m_callbacks.load(std::memory_order_acquire);

                new_vec = MakeShared<Vector>();
                new_vec->reserve(old->size() + 1);
                new_vec->insert(new_vec->end(), old->begin(), old->end());
                new_vec->emplace_back(id, std::move(cb));
            } while (!m_callbacks.compare_exchange_weak(
                old, new_vec,
                std::memory_order_release,
                std::memory_order_acquire
            ));
            return id;
        }

        Signal& operator+=(Callback cb) {
            [[maybe_unused]]auto i = Add(std::move(cb));
            return *this;
        }

        bool Remove(uint64_t token) noexcept {
            if (token == 0) return false;   // 0 不是有效令牌
            std::shared_ptr<const Vector> old, new_vec;
            do {
                old = m_callbacks.load(std::memory_order_acquire);
                auto it = std::find_if(old->begin(), old->end(),
                    [token](const Element& e) { return e.first == token; });
                if (it == old->end()) {
                    return false;   // 令牌不存在，直接退出（无需重试）
                }
                // 构造新 vector，跳过该元素
                new_vec = MakeShared<Vector>();
                new_vec->reserve(old->size() - 1);
                std::for_each(old->begin(), old->end(),
                    [&new_vec, token]( Element& e) {
                        if (e.first != token) {
                            new_vec->push_back(e);   // 拷贝 pair（回调是 move-only，此处必须拷贝？）
                        }
                    });
            } while (!m_callbacks.compare_exchange_weak(
                old, new_vec,
                std::memory_order_release,
                std::memory_order_acquire
            ));
            return true;
        }

        //
        void operator()(Args... args)  {
            auto callbacks = m_callbacks.load(std::memory_order_acquire);
            if (callbacks) {
                for (auto& [id, cb] : *callbacks) {
                    cb(args...);
                }
            }
        }

        [[nodiscard]]
        size_t Size() const noexcept {
            auto callbacks = m_callbacks.load(std::memory_order_acquire);
            return callbacks ? callbacks->size() : 0;
        }

        void Clear() {
            m_callbacks.store(MakeShared<Vector>(),
                              std::memory_order_release);
        }

    private:
        std::atomic<std::shared_ptr<Vector>> m_callbacks;
        std::atomic<uint64_t> m_nextId;
    };

}