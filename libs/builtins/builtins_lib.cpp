#include "include/builtins_lib.hpp"

namespace builtins_lib {
model::Object* init_module(model::Object* self, const model::List* args) {
    auto mod = new model::Module("builtins_lib");

    for (size_t i = 0; i < kiz::Vm::builtin_names.size(); ++i) {
        auto builtin_obj = kiz::Vm::builtins[i];
        mod->attrs_insert(kiz::Vm::builtin_names[i], builtin_obj);
    }

    return mod;
}
}