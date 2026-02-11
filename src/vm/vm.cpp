/**
 * @file vm.cpp
 * @brief 虚拟机（VM）核心实现
 * 用于运行IR
 * @author azhz1107cat
 * @date 2025-10-25
 */

#include "vm.hpp"

#include "../models/models.hpp"
#include "../opcode/opcode.hpp"

#include <algorithm>
#include <cassert>
#include <format>

#include "../kiz.hpp"

#include "../libs/builtins/include/builtin_methods.hpp"
#include "../libs/builtins/include/builtin_functions.hpp"

namespace kiz {

std::vector<model::Object*> Vm::builtins {};
std::vector<std::string> Vm::builtin_names {};
dep::HashMap<model::Module*> Vm::modules_cache {};
model::Module* Vm::main_module;
std::vector<model::Object*> Vm::op_stack {};
std::vector<CallFrame*> Vm::call_stack {};
model::Int* Vm::small_int_pool[201] {};
bool Vm::running = false;
std::string Vm::main_file_path;
std::vector<model::Object*> Vm::const_pool {};
dep::HashMap<model::Object*> Vm::std_modules {};
size_t Vm::bp = 0;


Vm::Vm(const std::string& file_path_) {
    main_file_path = file_path_;
    DEBUG_OUTPUT("entry builtin functions...");
    model::based_obj->make_ref();
    model::based_list->make_ref();
    model::based_function->make_ref();
    model::based_dict->make_ref();
    model::based_int->make_ref();
    model::based_bool->make_ref();
    model::based_nil->make_ref();
    model::based_str->make_ref();
    model::based_native_function->make_ref();
    model::based_error->make_ref();
    model::based_decimal->make_ref();
    model::based_module->make_ref();
    model::stop_iter_signal->make_ref();
    model::based_code_object->make_ref();

    for (dep::BigInt i = 0; i < 201; i+= 1) {
        auto int_obj = new model::Int{i};
        int_obj->make_ref(); // 创建
        int_obj->make_ref(); // 小整数池持有
        small_int_pool[i.to_unsigned_long_long()] = int_obj;
    }
    entry_builtins();
    entry_std_modules();
}

void Vm::set_main_module(model::Module* src_module) {
    DEBUG_OUTPUT("loading module...");
    // 合法性校验：防止空指针访问
    assert(src_module != nullptr);
    assert(src_module->code != nullptr);

    // 注册为main module
    main_module = src_module;
    src_module->make_ref();

    // 创建模块级调用帧（CallFrame）：模块是顶层执行单元，对应一个顶层调用帧
    op_stack.resize(src_module->code->local_count);
    auto module_call_frame = new CallFrame{
        .name = src_module->path,

        .owner = src_module,

        .pc = 0,
        .return_to_pc = src_module->code->code.size(),
        .last_locals_base_idx = 0,
        .bp = 0,
        .code_object = src_module->code,

        .try_blocks{},
        .iters{},
        .dyn_vars = dep::HashMap<model::Object*>(), // load slow

        .curr_error = nullptr,
    };

    // 将调用帧压入VM的调用栈
    call_stack.emplace_back(module_call_frame);

    // 初始化VM执行状态：标记为"就绪"
    running = true; // 标记VM为运行状态（等待exec触发执行）
    exec_curr_code();
}

void Vm::exec_curr_code() {
    // 循环执行当前调用帧下的所有指令
    while (!call_stack.empty() && running) {
        auto curr_frame = call_stack.back();
        // 检查当前帧是否执行完毕
        if (curr_frame->pc >= curr_frame->code_object->code.size()) {
            // 非模块帧则弹出，模块帧则退出循环
            if (call_stack.size() > 1) {
                call_stack.pop_back();
            } else {
                break;
            }
            continue;
        }

        // 执行当前指令
        const Instruction& curr_inst = curr_frame->code_object->code[curr_frame->pc];
        try {
            // std::cout << "current instr: " << opcode_to_string(curr_inst.opc) << std::endl;
            execute_unit(curr_inst);
            // std::cout << "finish handle instr: " << opcode_to_string(curr_inst.opc) << std::endl;
            // std::cout << "Stack:" << std::endl;
            // size_t j = 0;
            // for (auto st_mem: op_stack) {
            //     std::cout << j << ": " << st_mem->debug_string() << std::endl;
            //     ++j;
            // }
            // std::cout << "==End==" << std::endl;
            // std::cout << "bp=" << bp << std::endl;
        } catch (NativeFuncError& e) {
            instruction_throw(e.name, e.msg);
        }

        if (curr_inst.opc != Opcode::JUMP && curr_inst.opc != Opcode::JUMP_IF_FALSE &&
            curr_inst.opc != Opcode::RET && curr_inst.opc != Opcode::JUMP_IF_FINISH_HANDLE_ERROR
            && curr_inst.opc != Opcode::JUMP_IF_FINISH_ITER && curr_inst.opc != Opcode::THROW) {
            curr_frame->pc++;
        }
    }

    DEBUG_OUTPUT("call stack length: " + std::to_string(call_stack.size()));
}

CallFrame* Vm::fetch_curr_frame() {
    if ( !call_stack.empty() ) {
        return call_stack.back();
    }
    throw KizStopRunningSignal("Unable to fetch current frame");
}

model::Object* Vm::fetch_stack_top() {
    if (op_stack.empty()) return nullptr; // 先判断空栈
    auto stack_top = op_stack.back();
    if (stack_top) {
        stack_top->del_ref();
    }
    op_stack.pop_back();
    return stack_top;
}

void Vm::push_to_stack(model::Object* obj) {
    if (obj == nullptr) return;
    obj->make_ref(); // 栈成为新持有者，增加引用计数
    std::cout << "push: " << obj->debug_string() << std::endl;
    op_stack.push_back(obj);
}

void Vm::set_and_exec_curr_code(model::CodeObject* code_object) {
    assert(code_object != nullptr);
    assert(!call_stack.empty());

    // 获取全局模块级调用帧（REPL 共享同一个帧）
    auto* curr_frame = call_stack.back();
    // 对原有CodeObject调用del_ref(), 释放CallFrame的持有权
    if (curr_frame->code_object) {
        curr_frame->code_object->del_ref();
    }

    curr_frame->code_object = code_object;
    curr_frame->pc = 0;
    exec_curr_code();
}

std::string Vm::get_attr_name_by_idx(const size_t idx) {
    auto frame = fetch_curr_frame();
    return frame->code_object->attr_names[idx];
}


void Vm::assert_argc(size_t argc, const model::List* args) {
    if (argc == args->val.size()) {
        return;
    }
    throw NativeFuncError("ArgCountError", std::format(
        "expect {} arguments but got {} arguments", args->val.size(), argc
    ));
}

void Vm::assert_argc(const std::vector<size_t>& argcs, const model::List* args) {
    auto actually_count = args->val.size();
    for (size_t i : argcs) {
        if (i == actually_count) {
            return;
        }
    }

    std::string argc_str;
    size_t i = 0;
    for (size_t argc : argcs) {
        argc_str += std::to_string(argc);
        if (i != argcs.size()-1) {
            argc_str += " or ";
        }
        ++ i;
    }

    throw NativeFuncError("ArgCountError", std::format(
        "expect {} arguments but got {} arguments", argc_str, actually_count
    ));
}

} // namespace kiz