#ifndef NOWAIT_H_
#define NOWAIT_H_

#include <thread>
#include <condition_variable>
#include <functional>
#include "napi.h"
#define NOWAIT_CLASS_NAME "Nowait"

namespace NOWAIT
{

class Manager{
    public : 
        Manager();
        ~Manager();

        template<typename F>
        void watch(unsigned int wait_ms, F callback);
        void unwatch();
        void notify();
    private :
        std::thread thread;
        std::mutex mtx;
        std::condition_variable_any cv;
        bool isWatching;
        unsigned int wait_ms;
        std::function<void()> onTerminateCallBack;
};

class Wrapper: public Napi::ObjectWrap<Wrapper>{
    public : 
        static Napi::Object Init(Napi::Env env, Napi::Object exports);

        Wrapper(const Napi::CallbackInfo& info);
        ~Wrapper();
        
        Napi::Value watch(const Napi::CallbackInfo& info);
        Napi::Value unwatch(const Napi::CallbackInfo& info);
    private :
        static Napi::FunctionReference constructor;
        static Napi::FunctionReference callback;
        thread_local static Manager* manager;
};
    


}

#endif