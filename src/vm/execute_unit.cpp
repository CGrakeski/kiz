#include "vm.hpp"
#include "../../libs/builtins/include/builtin_functions.hpp"
#include "../opcode/opcode.hpp"

///| 核心执行单元
namespace kiz {
void Vm::execute_unit(const Instruction& instruction) {
    switch (instruction.opc) {
    case Opcode::OP_ADD: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        call_method(a, "__add__", {b});
        // 释放使用后的操作数
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_SUB: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        call_method(a, "__sub__", {b});
        // 释放使用后的操作数
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_MUL: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        call_method(a, "__mul__", {b});
        // 释放使用后的操作数
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_DIV: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        call_method(a, "__div__", {b});
        // 释放使用后的操作数
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_MOD: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        call_method(a, "__mod__", {b});
        // 释放使用后的操作数
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_POW: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        call_method(a, "__pow__", {b});
        // 释放使用后的操作数
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_NEG: {
        auto a = fetch_stack_top();
        call_method(a, "__neg__", {});
        // 释放使用后的操作数
        a->del_ref();
        break;
    }

    case Opcode::OP_EQ: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();

        call_method(a, "__eq__", {b});
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_GT: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();

        call_method(a, "__gt__", {b});
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_LT: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();

        call_method(a, "__lt__", {b});
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_GE: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();

        call_method(a, "__eq__", {b});
        call_method(a, "__gt__", {b});

        // 统一使用封装的栈操作获取结果
        auto eq_result = fetch_stack_top();
        auto gt_result = fetch_stack_top();

        // 压入最终结果
        if (is_true(gt_result) or is_true(eq_result)) {
            push_to_stack(model::load_true());
        } else {
            push_to_stack(model::load_false());
        }

        // 释放所有使用后的对象
        a->del_ref();
        b->del_ref();
        eq_result->del_ref();
        gt_result->del_ref();
        break;
    }

    case Opcode::OP_LE: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();

        // 调用__eq__方法
        call_method(a, "__eq__", {b});

        // 调用__lt__方法
        call_method(a, "__lt__", {b});

        // 获取结果
        auto eq_result = fetch_stack_top();
        auto lt_result = fetch_stack_top();

        // 压入最终结果
        if (is_true(lt_result) or is_true(eq_result)) {
            push_to_stack(model::load_true());
        } else {
            push_to_stack(model::load_false());
        }

        // 释放所有使用后的对象
        a->del_ref();
        b->del_ref();
        eq_result->del_ref();
        lt_result->del_ref();
        break;
    }

    case Opcode::OP_NE: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();

        // 调用__eq__方法
        call_method(a, "__eq__", {b});

        // 获取比较结果
        auto eq_result = fetch_stack_top();

        // 压入取反结果
        push_to_stack(model::load_bool(
            is_true(eq_result)
        ));

        // 释放所有使用后的对象
        a->del_ref();
        b->del_ref();
        eq_result->del_ref();
        break;
    }

    case Opcode::OP_NOT: {
        auto a = fetch_stack_top();
        bool result = !is_true(a);
        push_to_stack(model::load_bool(result));

        // 释放使用后的操作数
        a->del_ref();
        break;
    }

    case Opcode::OP_IS: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        // 压入比较结果
        push_to_stack(model::load_bool(a == b));
        // 释放使用后的操作数
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::OP_IN: {
        auto for_check = fetch_stack_top();
        auto item = fetch_stack_top();

        // 调用contains方法，参数为item
        call_method(for_check, "contains", {item});

        // 释放使用后的对象
        for_check->del_ref();
        item->del_ref();
        break;
    }

    case Opcode::MAKE_LIST: {
        make_list(instruction.opn_list[0]);
        break;
    }

    case Opcode::MAKE_DICT: {
        make_dict(instruction.opn_list[0]);
        break;
    }

    case Opcode::CREATE_CLOSURE: {
        model::Function* func_obj = dynamic_cast<model::Function*>(op_stack.back());

        auto& upvalues = func_obj->code->upvalues;
        std::vector<model::Object*> free_vars {};

        for (const auto& [distance_from_curr, idx] : upvalues) {
            auto frame = call_stack[ call_stack.size() - distance_from_curr];
            size_t loc_based = frame->bp;

            auto var = op_stack[loc_based + idx];
            var->make_ref();
            free_vars.push_back( var );
        }

        func_obj->free_vars = free_vars;
        break;
    }


    case Opcode::CALL: {
        model::Object* func_obj = fetch_stack_top();
        func_obj->make_ref();
        // 弹出栈顶-1元素 : 参数列表
        model::Object* args_obj = fetch_stack_top();
        handle_call(func_obj, args_obj, nullptr);
        args_obj->del_ref();
        func_obj->del_ref();
        break;
    }

    case Opcode::RET: {
        auto frame = call_stack.back();
        call_stack.pop_back();
        call_stack.back()->bp = frame->last_bp;
        call_stack.back()->pc = frame->return_to_pc;

        auto return_val = op_stack.back();
        assert(return_val);
        return_val->make_ref();

        while (frame->bp < op_stack.size()) {
            op_stack.back()->del_ref();
            op_stack.pop_back();
        }

        push_to_stack(return_val);

        delete frame;
        break;
    }

    case Opcode::CALL_METHOD: {
        auto obj = fetch_stack_top();

        // 弹出栈顶-1元素 : 参数列表
        model::Object* args_obj = fetch_stack_top();

        std::string attr_name = get_attr_name_by_idx(instruction.opn_list[0]);

        auto func_obj = get_attr(obj, attr_name);

        handle_call(func_obj, args_obj, obj);

        func_obj->del_ref();
        args_obj->del_ref();
        obj->del_ref();
        break;
    }

    case Opcode::GET_ATTR: {
        model::Object* obj = fetch_stack_top();
         std::string attr_name = get_attr_name_by_idx(instruction.opn_list[0]);

        model::Object* attr_val = get_attr(obj, attr_name);
        push_to_stack(attr_val);
        obj->del_ref();
        break;
    }

    case Opcode::SET_ATTR: {
        model::Object* attr_val = fetch_stack_top();
        attr_val = model::copy_or_ref(attr_val);

        model::Object* obj = fetch_stack_top();
        std::string attr_name = get_attr_name_by_idx(instruction.opn_list[0]);

        //检查原有同名属性，存在则释放引用
        auto attr_it = obj->attrs.find(attr_name);
        if (attr_it) {
            attr_it->value->del_ref();
        }

        obj->attrs_insert(attr_name, attr_val);
        attr_val->del_ref();
        obj->del_ref();
        break;

    }

    case Opcode::GET_ITEM: {
        model::Object* obj = fetch_stack_top();
        auto args_list = model::cast_to_list(fetch_stack_top());

        call_method(obj, "__getitem__", args_list->val);
        args_list->del_ref();
        obj->del_ref();
        break;
    }

    case Opcode::SET_ITEM: {
        model::Object* value = fetch_stack_top();
        model::Object* arg = fetch_stack_top();
        model::Object* obj = fetch_stack_top();

        // 获取对象自身的 __setitem__
        call_method(obj, "__setitem__", {arg, value});

        // 释放所有临时对象
        value->del_ref();
        arg->del_ref();
        obj->del_ref();
        break;
    }

    case Opcode::LOAD_VAR: {
        auto val = op_stack[call_stack.back()->bp + instruction.opn_list[0]];
        push_to_stack(val);
        break;
    }

    case Opcode::LOAD_CONST: {
        size_t const_idx = instruction.opn_list[0];
        model::Object* const_val = const_pool[const_idx];
        push_to_stack(const_val);
        break;
    }

    case Opcode::LOAD_BUILTINS: {
        auto obj = builtins[ instruction.opn_list[0] ];
        push_to_stack(obj);
        break;
    }

    case Opcode::LOAD_FREE_VAR: {
        auto func = dynamic_cast<model::Function*>(call_stack.back()->owner);
        assert(func != nullptr);
        push_to_stack(func->free_vars[ instruction.opn_list[0] ]);
        break;
    }

    case Opcode::SET_LOCAL: {
        model::Object* value = fetch_stack_top();

        size_t offset = call_stack.back()->bp + instruction.opn_list[0];
        if (op_stack[offset]) {
            op_stack[offset]->del_ref();
        }
        op_stack[offset] = model::copy_or_ref(value);
        value->del_ref();
        break;
    }


    case Opcode::SET_GLOBAL: {
        auto offset = instruction.opn_list[0];

        if (op_stack[offset]) {
            op_stack[offset]->del_ref();
        }

        auto value = fetch_stack_top();
        op_stack[offset] = model::copy_or_ref(value);
        value->del_ref();
        break;
    }

    case Opcode::SET_NONLOCAL: {
        auto upvalue = call_stack.back()->code_object->upvalues[ instruction.opn_list[0] ];
        auto frame = call_stack[ call_stack.size() - upvalue.distance_from_curr];
        size_t loc_based = frame->bp;

        auto value = fetch_stack_top();
        if (op_stack[loc_based + upvalue.idx]) {
            op_stack[loc_based + upvalue.idx]->del_ref();
        }
        op_stack[loc_based + upvalue.idx] = value;
        value->del_ref();
        break;
    }


    case Opcode::ENTER_TRY: {
        size_t catch_start = instruction.opn_list[0];
        size_t finally_start = instruction.opn_list[1];
        call_stack.back()->try_blocks.emplace_back(false, catch_start, finally_start);
        break;
    }

    case Opcode::JUMP_IF_FINISH_HANDLE_ERROR: {
        bool finish_handle_error = call_stack.back()->try_blocks.back().handle_error;
        // 弹出TryFrame
        call_stack.back()->try_blocks.pop_back();
        if (finish_handle_error) {
            call_stack.back()->pc = instruction.opn_list[0];
        } else {
            call_stack.back()->pc ++;
        }
        break;
    }

    case Opcode::MARK_HANDLE_ERROR: {
        call_stack.back()->try_blocks.back().handle_error = true;
        break;
    }

    case Opcode::THROW: {
        const auto top = fetch_stack_top();
        top->make_ref();

        // 替换前释放旧错误对象
        if (call_stack.back()->curr_error) {
            call_stack.back()->curr_error->del_ref();
        }
        call_stack.back()->curr_error = top;
        handle_throw();
        break;
    }

    case Opcode::LOAD_ERROR: {
        push_to_stack(call_stack.back()->curr_error);
        break;
    }

    case Opcode::JUMP: {
        size_t target_pc = instruction.opn_list[0];
        call_stack.back()->pc = target_pc;
        break;
    }

    case Opcode::JUMP_IF_FALSE: {
        model::Object* cond = fetch_stack_top();
        if (! is_true(cond)) {
            // 跳转逻辑
            call_stack.back()->pc = instruction.opn_list[0];
        } else {
            call_stack.back()->pc++;
        }
        cond->del_ref();
        break;
    }

    case Opcode::IS_CHILD: {
        auto b = fetch_stack_top();
        auto a = fetch_stack_top();
        push_to_stack(builtin::check_based_object(a, b));
        // 释放使用后的a/b对象，计数对称
        a->del_ref();
        b->del_ref();
        break;
    }

    case Opcode::CREATE_OBJECT: {
        auto obj = new model::Object();
        obj->make_ref(); // 创建即计数
        obj->attrs_insert("__parent__", model::based_obj);
        push_to_stack(obj); // push自动make_ref()，计数变为3，栈释放后回到2
        break;
    }

    case Opcode::IMPORT: {
        std::string module_path = get_attr_name_by_idx(instruction.opn_list[0]);
        handle_import(module_path);
        break;
    }

    case Opcode::CACHE_ITER: {
        auto iter = op_stack.back();
        iter->make_ref();

        call_stack.back()->iters.push_back(iter);
        break;
    }

    case Opcode::GET_ITER: {
        push_to_stack(
            call_stack.back()->iters.back()
        );
        break;
    }

    case Opcode::POP_ITER: {
        auto iter_obj = call_stack.back()->iters.back();
        iter_obj->del_ref();
        call_stack.back()->iters.pop_back();
        break;

    }

    case Opcode::JUMP_IF_FINISH_ITER: {
        auto obj = fetch_stack_top();
        size_t target_pc = instruction.opn_list[0];
        if (obj == model::stop_iter_signal) {
            call_stack.back()->pc = target_pc;
            obj->del_ref(); // 修复：释放使用后的obj
            return;
        }
        call_stack.back()->pc ++;
        obj->del_ref(); // 修复：释放使用后的obj
        break;
    }

    case Opcode::COPY_TOP: {
        auto obj = fetch_stack_top();
        push_to_stack(obj);
        push_to_stack(obj);
        break;
    }

    case Opcode::STOP: {
        running = false;
        break;
    }

    default: throw NativeFuncError("FutureError", "execute_instruction meet unknown opcode");
    }

}
}
