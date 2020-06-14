#ifndef _SINGLETON_HPP
#define _SINGLETON_HPP

#include <iostream>

template<typename T>
class Singleton
{
public:
    static T& get_instance() noexcept(std::is_nothrow_constructible<T>::value)
    {
        static T instance{token()};
        return instance;
    }
    virtual ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator = (const Singleton&) = delete;
protected:
    struct token{};
    Singleton() noexcept = default;
};

#endif // !_SINGLETON_HPP