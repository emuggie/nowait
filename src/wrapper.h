#ifndef WRAPPER_H_
#define WRAPPER_H_
#define WRAPPER_CLASS_NAME "Unpluger"

#include <nan.h>

namespace Nan {
namespace Unplug {

void Init(v8::Local<v8::Object> exports);
void wrapperMethod(const Nan::FunctionCallbackInfo<v8::Value>& info);

}
}
#endif