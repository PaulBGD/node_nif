#include <napi.h>

#include <filesystem>
#include <iostream>
#include "lib/NIF/NifFile.h"

static Napi::Value getNifDetails(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
                .ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string file = info[0].As<Napi::String>().ToString();

    std::filesystem::path filePath = std::filesystem::absolute(file);
    NifFile nif;
    int error = nif.Load(filePath.string());
    if (error) {
        Napi::TypeError::New(env, "Failed to load nif")
                .ThrowAsJavaScriptException();
        return env.Null();
    }
    Napi::Array toReturn = Napi::Array::New(env);
    uint32_t objIndex = 0;
    for (const auto &shape : nif.GetShapes()) {
        NiShader* shader = nif.GetShader(shape);
        if (shader) {
            std::vector<std::string> texFiles;
            for (int i = 0; i < 20; i++) {
                std::string texture;
                nif.GetTextureSlot(shader, texture, i);
                if (!texture.empty()) {
                    texFiles.push_back(texture);
                }
            }
            if (!texFiles.empty()) {
                BoundingSphere sphere = shape->GetBounds();
                Napi::Object obj = Napi::Object::New(env);
                obj.Set("radius", Napi::Number::New(env, sphere.radius));
                Napi::Array textures = Napi::Array::New(env);
                for (uint32_t i = 0; i < texFiles.size(); i++) {
                    textures.Set(i, Napi::String::New(env, texFiles.at(i)));
                }
                obj.Set("textures", textures);
                toReturn.Set(objIndex++, obj);
            }
        }
    }

    return toReturn;
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "getNifDetails"), Napi::Function::New(env, getNifDetails));
    return exports;
}

NODE_API_MODULE(getDetails, Init)
