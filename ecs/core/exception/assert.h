#ifndef ECS_ASSERT_H
#define ECS_ASSERT_H
#include <cassert>

inline
bool Message(auto message,bool  condition)
{
    return condition;
};

#define Assert(esp,MES)\
    assert(Message(MES,esp))


#endif //ECS_ASSERT_H