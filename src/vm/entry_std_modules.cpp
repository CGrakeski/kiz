#include "../models/models.hpp"
#include "../libs/io/include/io_lib.hpp"

namespace kiz {

void Vm::entry_std_modules() {
    std_modules.insert("io", new model::NativeFunction(
        io_lib::init_module
    ));
}

} // namespace model