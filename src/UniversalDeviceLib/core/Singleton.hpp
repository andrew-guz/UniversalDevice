#pragma once

template<typename T>
class Singleton {
protected:
    Singleton() = default;

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    virtual ~Singleton() = default;

    static T* Instance() {
        static T s_instance;
        return &s_instance;
    }
};
