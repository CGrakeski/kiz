#include "../../src/models/models.hpp"

namespace model {

Object* file_handle_flush(Object* self, const List* args) {
    kiz::Vm::assert_argc(0, args);

    auto f_obj = dynamic_cast<FileHandle*>(self);
    assert(f_obj);

    if (f_obj->is_closed) {
        throw NativeFuncError("FileError", "Cannot flush closed file handle");
    }
    if (!f_obj->file_handle || !f_obj->file_handle->good()) {
        throw NativeFuncError("FileError", "Invalid or corrupted file handle");
    }

    f_obj->file_handle->flush();

    if (f_obj->file_handle->bad()) {
        throw NativeFuncError("FileError", "Flush failed due to stream error");
    }

    return load_nil();
}

Object* file_handle_read(Object* self, const List* args) {
    kiz::Vm::assert_argc(0, args);
    auto f_obj = dynamic_cast<FileHandle*>(self);
    assert(f_obj);

    if (f_obj->is_closed) {
        throw NativeFuncError("FileError", "Cannot read from closed file handle");
    }
    if (!f_obj->file_handle || !f_obj->file_handle->good()) {
        throw NativeFuncError("FileError", "Invalid or corrupted file handle");
    }

    // 清除 EOF 等错误状态，并将读指针移至文件开头
    f_obj->file_handle->clear();
    f_obj->file_handle->seekg(0, std::ios::beg);

    // 读取整个文件内容
    std::ostringstream oss;
    oss << f_obj->file_handle->rdbuf();

    // 检查读取是否成功（可选）
    if (f_obj->file_handle->fail() && !f_obj->file_handle->eof()) {
        throw NativeFuncError("FileError", "Read failed");
    }
    return new String(oss.str());
}

Object* file_handle_write(Object* self, const List* args) {
    kiz::Vm::assert_argc(1, args);

    // 类型转换并校验
    auto f_obj = dynamic_cast<FileHandle*>(self);
    assert(f_obj);

    // 校验文件句柄状态
    if (f_obj->is_closed) {
        throw NativeFuncError("FileError", "Cannot write to closed file handle");
    }
    if (!f_obj->file_handle || !f_obj->file_handle->good()) {
        throw NativeFuncError("FileError", "Invalid or corrupted file handle");
    }

    // 提取要写入的字符串内容
    std::string content = kiz::Vm::obj_to_str(args->val[0]);

    // 写入内容并刷新缓冲区
    *f_obj->file_handle << content;
    f_obj->file_handle->flush();

    // 返回空对象（无返回值）
    return load_nil();
}

Object* file_handle_readline(Object* self, const List* args) {
    kiz::Vm::assert_argc(1, args);

    auto f_obj = dynamic_cast<FileHandle*>(self);
    assert(f_obj);

    if (f_obj->is_closed) {
        throw NativeFuncError("FileError", "Cannot read from closed file handle");
    }
    if (!f_obj->file_handle || !f_obj->file_handle->good()) {
        throw NativeFuncError("FileError", "Invalid or corrupted file handle");
    }

    // 安全转换参数
    auto lineno_obj = cast_to_int(args->val[0]);
    size_t lineno = lineno_obj->val.to_unsigned_long_long();

    std::string target_line;
    std::string current_line;
    int64_t current_lineno = 0;

    f_obj->file_handle->seekg(0, std::ios::beg);
    f_obj->file_handle->clear();

    while (std::getline(*f_obj->file_handle, current_line)) {
        ++current_lineno;
        if (current_lineno == lineno) {
            target_line = current_line;
            // 若文件未结束（即该行后有内容），补回换行符
            if (f_obj->file_handle->peek() != EOF) {
                target_line += "\n";
            }
            break;
        }
    }

    return new String(target_line);
}

Object* file_handle_close(Object* self, const List* args) {
    kiz::Vm::assert_argc(0, args);

    // 类型转换并校验
    auto f_obj = dynamic_cast<FileHandle*>(self);
    assert(f_obj);

    if (f_obj->is_closed) {
        return load_nil();
    }

    // 关闭文件句柄并释放资源
    if (f_obj->file_handle) {
        f_obj->file_handle->close();
        delete f_obj->file_handle;
        f_obj->file_handle = nullptr;
    }

    // 标记为已关闭
    f_obj->is_closed = true;

    return load_nil();
}

}