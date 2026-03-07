#pragma once
#include <functional>
#include <tuple>

#define FUNCTION_TRAITS_COMMON_MEMBERS \
constexpr static bool is_callable = true; \
using return_type = Ret; \
using args_tuple = std::tuple<Args...>; \
static constexpr size_t arity = sizeof...(Args); \
template<size_t N> \
using arg_type = std::tuple_element_t<N, args_tuple>;



namespace nx {




    template<typename T, typename = void>
struct function_traits {
    constexpr static bool is_callable = false;
};


// 函数指针
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

// 函数引用
template<typename Ret, typename... Args>
struct function_traits<Ret(&)(Args...)> : function_traits<Ret(*)(Args...)> {};

// 成员函数指针 (const)
template<typename ClassType, typename Ret, typename... Args>
struct function_traits<Ret(ClassType::*)(Args...) const> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

// 成员函数指针 (非 const)
template<typename ClassType, typename Ret, typename... Args>
struct function_traits<Ret(ClassType::*)(Args...)> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

// 成员函数指针 (const volatile)
template<typename ClassType, typename Ret, typename... Args>
struct function_traits<Ret(ClassType::*)(Args...) const volatile> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

// 成员函数指针 (引用限定符 - C++11)
template<typename ClassType, typename Ret, typename... Args>
struct function_traits<Ret(ClassType::*)(Args...) &> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

template<typename ClassType, typename Ret, typename... Args>
struct function_traits<Ret(ClassType::*)(Args...) &&> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

template<typename ClassType, typename Ret, typename... Args>
struct function_traits<Ret(ClassType::*)(Args...) const &> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

template<typename ClassType, typename Ret, typename... Args>
struct function_traits<Ret(ClassType::*)(Args...) const &&> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

// std::function
template<typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> {
    FUNCTION_TRAITS_COMMON_MEMBERS
};

template<typename T>
struct function_traits<T, std::void_t<decltype(&std::remove_cvref_t<T>::operator())>>
    : function_traits<std::remove_cvref_t<decltype(&std::remove_cvref_t<T>::operator())>> {
};



}
