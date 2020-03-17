// #include <v8.h>
#include "wrapper.h"
// #include "unplug.h"
#include "debugger.h"

namespace Nan {
namespace Unplug {

/**
 * 
 */
void Init(v8::Local<v8::Object> exports){
    DBG::printf("Init");
    auto context = exports ->CreationContext();
    exports->Set(
        context
        , Nan::New("wrapper").ToLocalChecked()
        , Nan::New<v8::FunctionTemplate>(wrapperMethod)->GetFunction(context).ToLocalChecked()
    );
}

/**
 * Format : wrapperMethod(callable, timeout_ms, onError=undefined);
 */
void wrapperMethod(const Nan::FunctionCallbackInfo<v8::Value>& info){
    DBG::printf("Wrapper");
    if(info.Length()< 2){
        Nan::ThrowError("arguement required 2 but received less");
        return;
    }
    
    if(!info[0]->IsFunction()) {
        Nan::ThrowError("Callable required : [0]");
        return;
    }
    
    if(!info[1]->IsNumber() || info[1]->Int32Value(info.GetIsolate()->GetCurrentContext()).FromMaybe(-1) < 0) {
        Nan::ThrowError("nonzero number arguement required 1");
        return;
    } 
    v8::Local<v8::Function> callback;
    if(info.Length() > 2) {
        if(!info[2]->IsFunction()){
            Nan::ThrowError("Callback function required 2");
            return;
        }
        callback = info[2].As<v8::Function>();
    }
    unsigned int timeout_ms = info[1]->Int32Value(info.GetIsolate()->GetCurrentContext()).FromMaybe(-1);
    v8::Local<v8::Function> task = info[0].As<v8::Function>();
    // v8::Unplug::IsolateContext::ctx.watch(new v8::Unplug::Task(&task,timeout_ms, &callback));
}

}
}
