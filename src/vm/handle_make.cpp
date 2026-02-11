#include "vm.hpp"
#include "../models/models.hpp"
#include "opcode/opcode.hpp"

namespace kiz {

// 辅助函数
auto Vm::make_pos_info() -> std::vector<std::pair<std::string, err::PositionInfo>> {
    size_t frame_index = 0;
    std::vector<std::pair<std::string, err::PositionInfo>> positions;
    std::string path;
    for (const auto& frame: call_stack) {
        if (const auto m = dynamic_cast<model::Module*>(frame->owner)) {
            path = m->path;
        }
        err::PositionInfo pos{};
        bool cond = frame_index == call_stack.size() - 1;
        if (cond) {
            pos = frame->code_object->code.at(frame->pc).pos;
        } else {
            pos = frame->code_object->code.at(frame->pc - 1).pos;
        }
        positions.emplace_back(path, pos);
        ++frame_index;
    }
    return positions;
}

void Vm::make_list(size_t len) {
    size_t elem_count = len;

    // 校验：栈中元素个数 ≥ 要打包的个数
    if (op_stack.size() < elem_count) {
        assert(false && ("MAKE_LIST: 栈元素不足（需" + std::to_string(elem_count) +
                        "个，实际" + std::to_string(op_stack.size()) + "个）").c_str());
    }

    // 弹出栈顶 elem_count 个元素（栈是LIFO，弹出顺序是 argN → arg2 → arg1）
    std::vector<model::Object*> elem_list;
    elem_list.reserve(elem_count);  // 预分配空间，避免扩容
    for (size_t i = 0; i < elem_count; ++i) {
        model::Object* elem = fetch_stack_top();

        // 校验：元素不能为 nullptr
        if (elem == nullptr) {
            assert(false && ("MAKE_LIST: 第" + std::to_string(i) + "个元素为nil（非法）").c_str());
        }

        // 关键：List 要持有元素的引用，所以每个元素 make_ref（引用计数+1）
        elem->make_ref();
        elem_list.push_back(elem);
    }

    // 反转元素顺序（恢复原参数顺序：arg1 → arg2 → ... → argN）
    std::reverse(elem_list.begin(), elem_list.end());

    // 创建 List 对象，压入栈
    auto* list_obj = new model::List(elem_list);
    push_to_stack(list_obj);

    DEBUG_OUTPUT("make_list: 打包 " + std::to_string(elem_count) + " 个元素为 List，压栈成功");
}

void Vm::make_dict(size_t len) {
    DEBUG_OUTPUT("exec make_dict...");

    size_t elem_count = len;
    const size_t total_elems = elem_count * 2;

    // 校验：栈中元素个数 ≥ 要打包的个数
    if (op_stack.size() < total_elems) {
        assert(false && "Stack underflow in MAKE_DICT: insufficient elements");
    }

    // 栈中顺序是 [key1, val1, key2, val2,...]（栈底→栈顶）
    std::vector<std::pair<
        dep::BigInt, std::pair< model::Object*, model::Object* >
    >> elem_list;
    elem_list.reserve(elem_count);

    for (size_t i = 0; i < elem_count; ++i) {
        model::Object* value = fetch_stack_top();
        if (!value) {
            throw NativeFuncError("DictMadeError", "Null value in dictionary entry");
        }
        value->make_ref();

        model::Object* key = fetch_stack_top();
        key->make_ref();

        // 修使用create_list创建临时参数，保存指针以便释放
        call_method(key, "__hash__", {});
        model::Object* hash_obj = fetch_stack_top();

        auto* hashed_int = dynamic_cast<model::Int*>(hash_obj);
        if (!hashed_int) {
            // 异常时提前释放临时对象，避免泄漏
            hash_obj->del_ref();
            key->del_ref();
            value->del_ref();
            throw NativeFuncError("TypeError", "__hash__ must return an integer");
        }
        assert(hashed_int != nullptr);

        elem_list.emplace_back(hashed_int->val, std::pair{key, value});

        // 释放临时参数和哈希对象
        hash_obj->del_ref();
    }

    auto* dict_obj = new model::Dictionary(dep::Dict(elem_list));
    dict_obj->make_ref();
    push_to_stack(dict_obj);
}
}