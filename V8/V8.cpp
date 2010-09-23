
#include <string>
#include <sstream>
#include <vector>
#include <ruby.h>
#include "v8.h"

#ifdef __cplusplus
extern "C" {
#endif

std::vector<std::string> errors;

void mc(v8::Handle<v8::Message> message, v8::Handle<v8::Value> data)
{
  std::stringstream error;
  error << *v8::String::AsciiValue(message->Get());
  error << std::string(" on line number: ");
  error << message->GetLineNumber();
  
  errors.push_back(error.str());// + message->GetLineNumber());
  //printf("Error '%s' on line %d\n", *v8::String::AsciiValue(message->Get()), message->GetLineNumber());
}

static VALUE compile(VALUE mod, VALUE filename_rb)
{
  char * filename_c = rb_string_value_cstr(&(filename_rb));
  
  FILE * fp = fopen(filename_c, "rb");
  if (fp == NULL) return rb_str_new2("invalid filename\n");
  
  fseek(fp, 0, SEEK_END);
  int filesize = ftell(fp);
  rewind(fp);
  
  char * filedata = (char*)malloc(filesize+1);
  int result = fread (filedata, 1, filesize, fp);
  filedata[filesize] = '\0';
  
  errors.clear();
  v8::HandleScope handle_scope;
  v8::Persistent<v8::Context> context = v8::Context::New();
  v8::Context::Scope context_scope(context);
  v8::Handle<v8::String> source = v8::String::New(filedata);
  v8::Handle<v8::Script> script = v8::Script::Compile(source);
  //Handle<Value> result = script->Run();
  context.Dispose();
  free(filedata);
  
  if (errors.size() > 0)
  {
    VALUE ary = rb_ary_new();
    int i;
    for (i = 0 ; i < errors.size() ; i++)
    {
      rb_ary_push(ary, rb_str_new2(errors[i].c_str()));
    }
    return ary;
  }
  
  return 0;
}

void Init_V8()
{
  VALUE V8 = rb_define_module("V8");
  rb_define_singleton_method(V8, "compile", (VALUE (*)(...))compile, 1);
  
  v8::V8::AddMessageListener(mc);
}

#ifdef __cplusplus
}
#endif
