
internal void
#if COMPILER_GCC || COMPILER_CLANG
win32_logf(const char *fmt, ...) PRINTF_FORMAT
#elif COMPILER_MSVC
win32_logf(_Printf_format_string_ const char *fmt, ...)
#endif
{
    char    buffer[4096];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len < 0)
        return;
    if ((u64)len >= sizeof(buffer))
        len = sizeof(buffer) - 1;

    HANDLE handle_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  written       = 0;
    WriteConsoleA(handle_stdout, buffer, (DWORD)len, &written, NULL);
}

internal void win32_log(String8List messages)
{
    HANDLE handle_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    u64    count         = 0;
    for (String8Node *node = messages.first; count < messages.count; node = node->next) {
        DWORD written = 0;
        WriteConsoleA(handle_stdout, node->value.data, (DWORD)(node->value.size), &written, NULL);
        ++count;
    }
}

// exit win32
// returns a boolean. i don't know what to do with the boolean if it fails hahahahhaa
// not like you can exit hahahahaha0
internal b32 win32_os_exit()
{
    ExitProcess(0);
    return true;
}

// TODO: remove FORMAT_MESSAGE_ALLOCATE_BUFFER flag
internal void win32_handle_error(MemArena *arena, DWORD error)
{
    ScratchArena scratch      = scratch_start(arena);
    String8      error_buffer = PushString8(arena, 255);
    DWORD        error_result = GetLastError();

    DWORD p_buffer = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                   NULL, error_result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   (LPTSTR)&error_buffer.data, 255, NULL);

    Log("Error code: %lu\n", error_result);
    Log("Error formatted message: %.*s\n", Str8Arg(error_buffer));
    scratch_end(&scratch);
}

internal u64 safe_truncate_64(u64 value)
{
    Assert(value <= (u64)~0);
    return (u64)value;
}

internal b32 win32_file_exists(u8 *path)
{
    DWORD attributes = GetFileAttributesA((LPCSTR)path);
    return ((attributes != INVALID_FILE_ATTRIBUTES) && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

internal b32 win32_file_write(String8 data, String8 path)
{
    b32 result = false;

    HANDLE file_handle = CreateFileA((LPCSTR)path.data, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

    if (file_handle != INVALID_HANDLE_VALUE) {
        DWORD bytes_written;
        if (WriteFile(file_handle, data.data, (DWORD)data.size, &bytes_written, 0)) {
            result = (bytes_written == data.size);
        } else {
            Log("Writing the file was unsuccessful");
        }
        CloseHandle(file_handle);
    } else {
        Log("File handle has the INVALID_HANDLE_VALUE flag.");
    }

    return result;
}

internal String8 win32_file_load(MemArena *arena, String8 file)
{
    String8 result = {0};
    HANDLE  file_handle =
        CreateFileA((LPCSTR)file.data, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    if (file_handle == INVALID_HANDLE_VALUE) {
        Log("Failed to open file\n");
        return result;
    }

    LARGE_INTEGER size;

    DWORD get_file_size_result = GetFileSizeEx(file_handle, &size);
    if (!get_file_size_result) {
        Log("Failed to get file size");
        win32_handle_error(arena, get_file_size_result);
        CloseHandle(file_handle);
        return result;
    }

    ScratchArena file_load_scratch = scratch_start(arena);
    u32          file_size32       = (u32)safe_truncate_64(size.QuadPart);
    result                         = PushString8(arena, file_size32);

    // return value in case of success should be true

    DWORD bytes_read = 0;
    ;
    DWORD read_file_result = ReadFile(file_handle, (LPSTR)result.data, file_size32, &bytes_read, 0);

    if (!read_file_result || (file_size32 != bytes_read)) {
        Log("Failed to read file\n");
        win32_handle_error(arena, read_file_result);
        CloseHandle(file_handle);
        return result;
    }

    result.size = file_size32;
    // TODO: find out if we want to free this or not.
    CloseHandle(file_handle);
    return result;
}

internal void *win32_load_library()
{
    // TODO: ...
}

internal void win32_library_get_proc()
{
    // TODO: ...
}

internal String8List win32_open_dir(MemArena *arena, String8 *original_path)
{
    // TODO: ...
}
