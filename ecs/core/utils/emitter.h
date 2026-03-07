#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <atomic>


namespace nx {
    template<typename Payload>
    class TypeEmitter {
    private:
        struct Listener {
            uint64_t id;
            std::function<void(const Payload&)> fun;
        };
        using ListenerVec = std::pmr::vector<Listener>;
        using ListenerVecPtr = std::shared_ptr<ListenerVec>;

        std::atomic<ListenerVecPtr> m_listeners;
        std::atomic<uint64_t> m_nextId{0};   // 用于生成唯一 ID

    public:
        class Token {
            friend class TypeEmitter;
            TypeEmitter* m_emitter = nullptr;
            uint64_t m_id = 0;
            Token(TypeEmitter* em, uint64_t id) : m_emitter(em), m_id(id) {}
        public:
            Token() = default;
            Token(const Token&) = delete;
            Token& operator=(const Token&) = delete;
            Token(Token&& other) noexcept
                : m_emitter(other.m_emitter), m_id(other.m_id) {
                other.m_emitter = nullptr;
                other.m_id = 0;
            }
            Token& operator=(Token&& other) noexcept {
                if (this != &other) {
                    Reset();
                    m_emitter = other.m_emitter;
                    m_id = other.m_id;
                    other.m_emitter = nullptr;
                    other.m_id = 0;
                }
                return *this;
            }
            ~Token() {
                if (m_emitter) m_emitter->RemoveListener(m_id);
            }
            void Reset() {
                if (m_emitter) m_emitter->RemoveListener(m_id);
                m_emitter = nullptr;
                m_id = 0;
            }
        };

        TypeEmitter() {
            m_listeners.store(MakeSharedWidthSynchronizedCache<ListenerVec>());
        }
        [[nodiscard]]
        Token AddListener(std::function<void(const Payload&)>&& fun) {
            uint64_t id = m_nextId.fetch_add(1, std::memory_order_relaxed);
            ListenerVecPtr old, newVec;
            do {
                old = m_listeners.load(std::memory_order_acquire);
                newVec = MakeSharedWidthSynchronizedCache<ListenerVec>();
                newVec->reserve(old->size() + 1);
                newVec->assign_range(*old);
                newVec->emplace_back(id, std::move(fun));
            } while (!m_listeners.compare_exchange_weak(old, newVec,
                        std::memory_order_release, std::memory_order_acquire));
            return Token(this, id);
        }

        void RemoveListener(uint64_t id) {
            ListenerVecPtr old, newVec;
            do {
                old = m_listeners.load(std::memory_order_acquire);

                auto it = std::lower_bound(old->begin(), old->end(), id,
                    [](const Listener& l, uint64_t val) { return l.id < val; });

                if (it == old->end() || it->id != id) return;

                newVec = MakeSharedWidthSynchronizedCache<ListenerVec>();
                newVec->reserve(old->size() - 1);
                newVec->insert(newVec->end(), old->begin(), it);
                newVec->insert(newVec->end(), std::next(it), old->end());

            } while (!m_listeners.compare_exchange_weak(old, newVec,
                        std::memory_order_release, std::memory_order_acquire));
        }

        void Emit(const Payload& pl) {
            auto listeners = m_listeners.load(std::memory_order_acquire);
            for (auto& l : *listeners) {
                l.fun(pl);
            }
        }

        static TypeEmitter& Instance() {
            static TypeEmitter inst;
            return inst;
        }
    };

    class Emitter {
    public:
        template<typename Payload>
        [[nodiscard]]
        static auto RegisterListener(std::function<void(const std::remove_cvref_t<Payload>&)>&& f) {
            using PlType = std::remove_cvref_t<Payload>;
            return TypeEmitter<PlType>::Instance().AddListener(std::move(f));
        }

        template<typename Payload>
        static void Emit(const Payload& payload) {
            using PlType = std::remove_cvref_t<Payload>;
            TypeEmitter<PlType>::Instance().Emit(payload);
        }
    };
}

