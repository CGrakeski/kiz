#pragma once
#include "models/models.hpp"

namespace io_lib {

model::Object* init_module(model::Object* self, const model::List* args);

model::Object* fast_read(model::Object* self, const model::List* args);
model::Object* fast_write(model::Object* self, const model::List* args);

}

