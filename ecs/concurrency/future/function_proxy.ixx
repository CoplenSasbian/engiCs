module;
#include <stdexec/execution.hpp>

#include <future>
#include <memory>
#include <utility>
#include <type_traits>
#include <coroutine>
#include <print>

export module nx.concurrency.promise;
import nx.core.utils.function_traits;
namespace std::execution {
    using namespace stdexec;
}


namespace nx {
    template<class Register, class... Args>
    struct CallbackAwaitable;

    export
    template<typename Register, class... Args>
    struct CallbackPromise;


    template<std::execution::receiver R, class Register, class... Args>
    struct ProxyOps {
        R r_;
        CallbackPromise<Register, Args...> promise_;

        ProxyOps(R&& r,CallbackPromise<Register,Args...>&& p)
            :r_(std::move(r)),promise_(std::move(p)) {}

        void start()noexcept;

    };


    export
    template<typename Register, class... Args>
    class CallbackPromise {
        Register _register;

    public:
        CallbackPromise(Register &&reg) : _register(std::forward<Register>(reg)) {
        }

        template<class Callback>
            requires (std::invocable<Register &, Callback &> && std::invocable<Callback &, Args...>)
        void operator()(Callback &&re) {
            _register(std::forward<Callback>(re));
        }

        CallbackAwaitable<Register, Args...> operator co_await() noexcept;

        auto get_completion_signatures() const noexcept {
            return std::execution::completion_signatures<
                std::execution::set_value_t(),
                std::execution::set_error_t(std::exception_ptr),
                std::execution::set_stopped_t()
            >{};
        }
        using is_sender = void;

        template<std::execution::receiver R>
        ProxyOps<R, Register, Args...> connect(R&& r)  noexcept {
            return {std::forward<R>(r),std::move(_register)};
        }

        bool operator==(const CallbackPromise & oth) const {
            return _register == oth._register;
        }
    };


    export
    template<class... Args, class Register>
    auto MakeCallbackPromise(Register &&reg) {
        return CallbackPromise<std::decay_t<Register>, Args...>(std::forward<Register>(reg));
    }


    template<class Register, class... Args>
    struct CallbackAwaitable {
        CallbackPromise<Register, Args...> &promise_;

        std::optional<std::tuple<Args...> > result_;

        CallbackAwaitable(CallbackPromise<Register, Args...> &promise) : promise_(promise) {
        }

        constexpr bool await_ready() const noexcept {
            return false;
        }

        constexpr void await_suspend(std::coroutine_handle<> h) const noexcept {
            promise_(
                [this,h](Args &&... args) {
                    if constexpr (sizeof...(Args) > 0) {
                        result_ = std::make_tuple(std::forward<Args>(args)...);
                    }
                    h.resume();
                });
        }

        constexpr auto await_resume() const noexcept {
            if constexpr (sizeof...(Args) > 1) {
                return result_.value();
            } else if constexpr (sizeof...(Args) == 1) {
                return std::get<0>(result_.value());
            } else {
                return;
            }
        }
    };

    template<std::execution::receiver R, class Register, class ... Args>
    void ProxyOps<R, Register, Args...>::start() noexcept {
        try {
            auto stopToken = std::execution::get_stop_token(std::execution::get_env(r_));
            if (stopToken.stop_requested()) {
                std::execution::set_stopped(std::move(r_));
                return;
            }

            promise_([this](Args &&... args) {
               std::execution::set_value(std::move(r_), std::forward<Args>(args)...);
            });

        }catch (...) {
            std::execution::set_error(std::move(r_), std::current_exception());
        }
    }

    template<typename Register, class... Args>
    CallbackAwaitable<Register, Args...> CallbackPromise<Register, Args...>::operator co_await() noexcept {
        return {*this};
    }




} // namespace nx
