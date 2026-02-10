/**
 * @file vm.hpp
 * @brief 虚拟机(VM)核心定义
 * 执行IR
 * @author azhz1107cat
 * @date 2025-10-25
 */
#pragma once

#include <cassert>

#include "../../deps/hashmap.hpp"

#include <stack>
#include <tuple>
#include <utility>

#include "../kiz.hpp"
#include "../error/error_reporter.hpp"
namespace model {
class Module;
class CodeObject;
class Object;
class List;
class Int;
class Error;
}

namespace kiz {

enum class Opcode : uint8_t;

struct Instruction {
    Opcode opc;
    std::vector<size_t> opn_list;
    err::PositionInfo pos{};
    Instruction(Opcode o, std::vector<size_t> ol, err::PositionInfo& p) : opc(o), opn_list(std::move(ol)), pos(p) {}
};

struct TryFrame {
    bool handle_error = false;
    size_t catch_start = 0;
    size_t finally_start = 0;
};

struct CallFrame {
    std::string name;

    model::Object* owner;

    size_t pc = 0;
    size_t return_to_pc;
    size_t last_locals_base_idx;
    model::CodeObject* code_object;
    
    std::vector<TryFrame> try_blocks;
    std::vector<model::Object*> iters;
    dep::HashMap<model::Object*> dyn_vars; // load slow

    model::Object* curr_error;
};

class Vm {
public:
    static dep::HashMap<model::Module*> modules_cache;
    static model::Module* main_module;

    static std::vector<model::Object*> unique_op_stack;
    static std::vector<CallFrame*> call_stack;
    static size_t curr_locals_base_idx; // 包含头

    static model::Int* small_int_pool[201];
    static std::vector<model::Object*> const_pool;

    static dep::HashMap<model::Object*> builtins;
    static dep::HashMap<model::Object*> std_modules;

    static bool running;
    static std::string main_file_path;

    explicit Vm(const std::string& file_path_);

    ///| 核心执行循环
    static void set_main_module(model::Module* src_module);
    static void exec_curr_code();
    static void set_and_exec_curr_code(model::CodeObject* code_object);
    static void execute_unit(const Instruction& instruction);

    ///| 栈操作
    static CallFrame* fetch_curr_frame();
    static model::Object* fetch_stack_top();
    static void push_to_stack(model::Object* obj);
    static std::string get_attr_name_by_idx(size_t idx);

    ///| 如果新增了调用栈，执行循环仅处理新增的模块栈帧（call_stack.size() > old_stack_size），不影响原有调用栈
    static void call_function(model::Object* func_obj, std::vector<model::Object*> args, model::Object* self);

    ///| 运算符与普通方法分规则查找
    static void call_method(model::Object* obj, const std::string& attr_name, std::vector<model::Object*> args);

    ///| 如果用户函数则创建调用栈，如果内置函数则执行并压上返回值
    static void handle_call(model::Object* func_obj, model::Object* args_obj, model::Object* self=nullptr);

    ///| 处理import
    static void handle_import(const std::string& module_path);

    ///| 处理报错(设置到catch/finally pc)或者进行traceback
    static void handle_throw();

    ///| 注册内置对象
    static void entry_builtins();
    ///| 注册标注库
    static void entry_std_modules();

    ///| @utils
    static model::Object* get_attr(const model::Object* obj, const std::string& attr);
    static bool is_true(model::Object* obj);
    static std::string obj_to_str(model::Object* for_cast_obj);
    static std::string obj_to_debug_str(model::Object* for_cast_obj);
    static void instruction_throw(const std::string& name, const std::string& content);

    static auto make_pos_info() -> std::vector<std::pair<std::string, err::PositionInfo>>;
    static void make_list(size_t len);
    static void make_dict(size_t len);

    ///| @utils: 供builtins检查参数
    static void assert_argc(size_t argc, const model::List* args);
    static void assert_argc(const std::vector<size_t>& argcs, const model::List* args);
};

} // namespace kiz