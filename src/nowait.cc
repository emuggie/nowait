#include "nowait.h"
#include "debugger.h"
#include <chrono>
#include <v8.h>
#include <vector>
#include <limits>
#include "node.h"

using namespace v8;

namespace NOWAIT {
    Napi::FunctionReference Wrapper::constructor;
    Manager* Wrapper::manager= nullptr;

    Napi::Object Wrapper::Init(Napi::Env env, Napi::Object exports){
        Napi::HandleScope scope(env);
        Napi::Function func = DefineClass(env,
            NOWAIT_CLASS_NAME,{
                InstanceMethod("watch", &Wrapper::watch),
                InstanceMethod("unwatch", &Wrapper::unwatch)
            }
        );
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        exports.Set(NOWAIT_CLASS_NAME, func);
        return exports;
    }
    
    Wrapper::Wrapper(const Napi::CallbackInfo& info): Napi::ObjectWrap<Wrapper>(info){
        Napi::Env env = info.Env();
        Napi::HandleScope scope(env);
    
        try{
            if(manager ==  nullptr){
                manager = new Manager();
            }
        }catch(const char* message){
            Napi::Error::New(env, "Manager create Error : ERROR CODE : " + std::string(message)).ThrowAsJavaScriptException();
        }
        return;
    }

    Wrapper::~Wrapper(){
        // delete mana;
    }

    Napi::Value Wrapper::watch(const Napi::CallbackInfo& info){
        if(info.Length()< 2){
            Napi::Error::New(info.Env(),"arguement required 2 but received less").ThrowAsJavaScriptException();
            return info.Env().Undefined();
        }
        if(!info[0].IsNumber() || info[0].As<Napi::Number>().Int32Value() < 0) {
            Napi::Error::New(info.Env(),"nonzero number arguement required 0").ThrowAsJavaScriptException();
            return info.Env().Undefined();
        }
        if(!info[1].IsFunction()) {
            Napi::Error::New(info.Env(),"callback function required 1").ThrowAsJavaScriptException();
            return info.Env().Undefined();
        }
        callback = Napi::Persistent(info[1].As<Napi::Function>());
        printf("/???");
        Napi::Reference<Napi::Object> recv = Napi::Persistent(Napi::Object::New(info.Env()));
        printf("/???11111");
        manager->watch((unsigned int)info[0].As<Napi::Number>().Int32Value(),[&]()->void{
            printf("invoke reject\n");
            callback.MakeCallback(recv.Value(),std::vector<napi_value>{Napi::String::New(info.Env(),"Timeout Terminated.")});
        });
        return info.Env().Undefined();
    }

    Napi::Value Wrapper::unwatch(const Napi::CallbackInfo& info){
        manager->unwatch();
    }

    
    Manager::Manager(){
        v8::Isolate* is = v8::Isolate::GetCurrent();

        // is->AddBeforeCallEnteredCallback([](v8::Isolate* isolate){
        //     printf("bc :current :  %d,",isolate->GetCurrentContext());
        //     printf("entered : %d,",isolate->GetEnteredContext());
        //     printf("me : %d\n",isolate->GetEnteredOrMicrotaskContext());
        // });
        // is->AddCallCompletedCallback([](v8::Isolate* isolate){
        //     printf("cc :current :  %d,",isolate->GetCurrentContext());
        //     printf("entered : %d,",isolate->GetEnteredContext());
        //     printf("me : %d\n",isolate->GetEnteredOrMicrotaskContext());
        // });
        // is->AddMicrotasksCompletedCallback([](v8::Isolate* isolate, void* data){
        //     printf("msc context : %d\n",isolate->GetEnteredOrMicrotaskContext());
        // });

        thread = std::thread([this, is]{
            while(true){
                // Wait until watch event 
                printf("wait iswatch\n");
                this->cv.wait(this->mtx, [this]{return this-> isWatching;});
                // Wait for given time if keep watches or wake up
                printf("wait timemili : %d\n", this->wait_ms);
                this->cv.wait_for(this->mtx,std::chrono::milliseconds(this->wait_ms), [this]{return !this-> isWatching;});
                printf("waited: %d\n", this->wait_ms );
                // If woke up by watch cancel, skip termination. should be atomic
                // this->mtx.lock();
                if(!this->isWatching){
                    printf("resolved so next\n");
                    continue;
                }
                printf("Terminate\n");
                is->TerminateExecution();
                this->isWatching = false;
                is->RequestInterrupt([](v8::Isolate* isolate, void* self){
                    printf("Resolve\n");
                    // invoke callback
                    ((Manager*)self)->notify();
                }, this);
                // this->mtx.unlock();
            }
        });
    }

    template<typename F>
    void  Manager::watch(unsigned int wait_ms, F callback){
        // mtx.lock();
        printf("watch lock\n");
        isWatching = true;
        this->wait_ms = wait_ms;
        onTerminateCallBack = callback;
        printf("watch unlock\n");
        // mtx.unlock();
        cv.notify_one();
    }

    void Manager::unwatch(){
        // mtx.lock();
        printf("unwatch lock\n");
        isWatching = false;
        printf("unwatch lock\n");
        // mtx.unlock();
    }

    void Manager::notify(){
        onTerminateCallBack();
    }

    void enquee(Local<Function> f){
        Isolate * ci = Isolate::GetCurrent();
        //enqueue task.
        ci->EnqueueMicrotask(f);
        // add completed callback
        ci->AddMicrotasksCompletedCallback([](Isolate* is, void* data){
            //enqueue micro task with success callback
            //flag thread as success
        }, &f);

    }
} // namespace NODE