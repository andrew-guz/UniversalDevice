#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template<typename T>
class Singleton {
protected:
    Singleton() = default;

public:
    virtual ~Singleton() = default;

    static T* Instance() {
        static T s_instance;
        return &s_instance;
    }
};

#endif //_SINGLETON_H_
