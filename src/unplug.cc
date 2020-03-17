#include "unplug.h"

using namespace v8;
namespace v8 {
namespace Unplug {

IsolateContext::IsolateContext(){
    // set target isolate.
    isolate = Isolate::GetCurrent();
    mcallback = [](Isolate* is, void* data){
        auto $this = (IsolateContext*)data;
        is->RemoveMicrotasksCompletedCallback($this->mcallback);
    };

    // timer_thread.
    timer_thread = std::thread([this](){
        while(true){
            // Wait until callstack fills
            this->cv.wait(this->mtx, 
                [this]{
                    return !this-> callStack.empty();
                }
            );

            auto currentTask = this -> callStack.top();
            auto stackLength = this->callStack.size();

            // Wait for given time or wake up early if stack has changed.
            this->cv.wait_for(
                this->mtx,std::chrono::milliseconds(currentTask->timeoutLeft())
                , [this, stackLength]{
                    return this->callStack.size() !=  stackLength;
                }
            );

            // If woke up by callstackChanges, measure time again for updated timeouts.
            if(this->callStack.size() !=  stackLength){
                continue;
            }

            // asume time has passed. should be atomic task from now on.
            // TODO : mark code with mutex.

            //If task has resolved, pop out task.
            if(currentTask->isResolved()){
                this->callStack.pop();
                delete currentTask;
                continue;
            }
            // Terminate current execution
            isolate -> TerminateExecution();
            
            // queue micro task of failure with request interrupt
            isolate->RequestInterrupt([](v8::Isolate* isolate, void* self){
                auto $this = (IsolateContext*)self;
                Task* task = $this->callStack.top();
                Local<Function> callback = Local<Function>::New(isolate, *(task->callback));
                // Create argument to forward callback.
                Local<Value> argv[1] = { String::NewFromUtf8(isolate, "Canceled due to timeout")};
                callback->Call(
                    isolate->GetCurrentContext()
                    , isolate->GetCurrentContext()->Global()
                    , 1
                    , argv
                );
                $this->callStack.pop();
            }, this);
        }
    });
}

IsolateContext::~IsolateContext(){
    // Clean all callstack
    while(!this->callStack.empty()){
        delete this->callStack.top();
        this->callStack.pop();
    }
}

void IsolateContext::watch(Task* task){
    this->callStack.push(task);
        // add success callback for task
    isolate->AddMicrotasksCompletedCallback(this->mcallback, task);
    isolate->EnqueueMicrotask(task->task->Get(isolate));
}

Task::Task(Local<Function> *task, unsigned int timeout_ms, Local<Function>* callback){
    this->task = new Persistent<Function>(Isolate::GetCurrent(), *task);
    this->callback = nullptr;

    if(callback != nullptr){
        this->callback = new Persistent<Function>(Isolate::GetCurrent(), *callback);
    }
    this->timeout_ms = timeout_ms;
}

Task::~Task(){
    // Clear task and callbacks
    delete this->task;
    if(this->callback!=nullptr){
        delete this->callback;
    }
}

unsigned long Task::timeoutLeft(){
    using namespace std::chrono;
    return timeout_ms - duration_cast<milliseconds>(steady_clock::now() - begin_time_point).count();
}

bool Task::isResolved(){
    return false;
}

}
}