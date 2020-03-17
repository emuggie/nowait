#ifndef V8_UNPLUG_H_
#define V8_UNPLUG_H_

#include "v8.h"
#include <stack>
#include <thread>
#include <condition_variable>
#include <chrono>

using namespace v8;

namespace v8 {
namespace Unplug {

class Task {
    public :
        Task(Local<Function>* task, unsigned int timeout_ms, Local<Function>* callback = nullptr );
        ~Task();
        unsigned long timeoutLeft();
        bool isResolved();
        Persistent<Function>* task;
        Persistent<Function>* callback;

    private :
        unsigned int timeout_ms;
        std::chrono::steady_clock::time_point begin_time_point;
};

class IsolateContext {
    public :
        static thread_local IsolateContext ctx;
        IsolateContext();
        ~IsolateContext();
        void watch(Task* task);
        MicrotasksCompletedCallbackWithData mcallback;
    private:
        std::thread timer_thread;
        std::condition_variable_any cv;
        Isolate* isolate;
        std::stack<Task*> callStack = std::stack<Task*>{};
        std::mutex mtx;
};
// 

}
}
#endif