
internal String8 unix_get_env(const char *var)
{
    AssertLog(environ, "Environment not loaded");

    String8 buffer  = {0};
    u64     index   = 0;
    char   *current = environ[index];
    while (environ[index] != NULL) {
        if (CstringAnyMatch(environ[index], var)) {
            buffer = str8(environ[index]);

            u64 equal_sign_pos   = 0;
            b32 found_equal_sign = str8_find(&buffer, 0, str8("="), &equal_sign_pos);

            if (equal_sign_pos) {
                buffer = str8_chop_left(buffer, equal_sign_pos + 1);
            }

            return buffer;
        }
        ++index;
    }

    AssertLog(buffer.size != 0, "env value not found");
    return {0};
}


//-
internal SharedLibrary unix_load_shared_library(MemArena *arena, cstring name)
{
    SharedLibrary lib  = {0};
    void         *data = dlopen(name, RTLD_LAZY | RTLD_LOCAL);
    AssertLog(data, "failed to load shared object %s", name); // Assert and print the error that get's reported to dlerror
    lib.mem = data;
    return lib;
}

internal void *unix_shared_library_get_proc_from_name(SharedLibrary *library, const char *name)
{
    void *symbol = dlsym(library->mem, name);
    return symbol;
}

internal int unix_shared_library_close(SharedLibrary *library)
{
    int result = dlclose(library);
    AssertLog(result, "failed to close the library");

    return result;
}

//- implementations
internal b32 unix_file_exists(String8 path)
{
    return access((const char *)path.data, F_OK) == 0;
}

internal b32 unix_relative_path(MemArena *arena, String8 *path)
{
    String8 forward_slash    = str8("./");
    b32     insertion_result = str8_insert(arena, path, &forward_slash, 0);
    if (!insertion_result) {
        breakpoint();
    }
    return insertion_result;
}

internal String8 unix_file_load(MemArena *arena, String8 path)
{
    if (path.size <= 0) {
        Log("Can't find the file we we're supposed to open. This is an issue because we we're supposed to break way earlier");
        return {};
    }

    String8     result = {0};
    struct stat sbuf   = {0};

    if (!path.data) {
        Log("Invalid path file");
        return str8_zero();
    }


    const char *null_terminated_path = null_terminate(arena, &path);

    s32 file = open(null_terminated_path, O_RDONLY);
    if (file == -1) {
        Log("File was not loaded: " Str8Fmt "", Str8Arg(path));
        return str8_zero();
    }

    if (fstat(file, &sbuf) == -1) {
        close(file);
        return str8_zero();
    }

    result = PushString8(arena, (u64)sbuf.st_size);

    u64 file_size = (u64)sbuf.st_size;
    result.size   = file_size;

    u64 total_bytes_read = 0;

    while (total_bytes_read < file_size) {
        s64 bytes = read(file, result.data + total_bytes_read, file_size - total_bytes_read);
        if (bytes == -1) {
            Log("Failed, reading file was interrupted");
            return str8_zero();
        }

        if (bytes == 0)
            break;
        total_bytes_read += (u64)bytes;
    }

    result.size = total_bytes_read;

    close(file);
    return result;
}

// TODO: why was this returning a str8 before?
internal b32 unix_file_write(String8 data, String8 path)
{
    s32 file = open((const char *)path.data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1) {
        return false;
    }

    u64 written = 0;
    while (written < data.size) {
        s64 err = write(file, data.data + written, data.size - written);
        if (err == -1) {
            close(file);
            return false;
        }
        written += err;
    }

    close(file);
    return true;
}

internal void unix_log_with_args(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vprintf(format, arguments);
    putc('\n', stdout);
    fflush(stdout);
    va_end(arguments);
}


// TODO: rewrite this
internal String8List *unix_open_dir(MemArena *arena, String8 *path)
{
    String8List *files_list = PushStructZero(arena, String8List);
    files_list->first       = &nil_str8_node;
    files_list->last        = &nil_str8_node;

    cstring cpath = null_terminate(arena, path);
    DIR    *dir   = opendir(cpath);
    if (!dir) {
        Log("opendir(%s) failed", cpath);
        return files_list;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) { // TODO: should symbolic files be handles aswell?
            continue;
        }

        String8      name      = str8(entry->d_name);
        String8Node *file_node = PushStruct(arena, String8Node);
        *file_node             = ToString8Node(str8_clone(arena, &name));
        str8_list_push_node(files_list, file_node);
    }

    closedir(dir);
    return files_list;
}