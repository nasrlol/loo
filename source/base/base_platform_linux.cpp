
internal void linux_sig_handler(int signal)
{
	u64 COUNT = 4096;
	void *buffer[COUNT] = {0};
	int size = backtrace(buffer, COUNT);
	for(int index = 0; index < size; ++index) {
		Dl_info info = {0};
		struct link_map *map = 0;
		if(!dladdr1(buffer[index], &info, (void **)&map, RTLD_DL_LINKMAP) || !info.dli_fname) continue;
		if(info.dli_fname[0] != '/' && info.dli_fname[0] != '.') continue;
		
		char cmd[COUNT] = {0};
		u64 address = (u64)((u8 *)buffer[index] - (u8 *)map->l_addr) - 1;
		Fmt(cmd, sizeof(cmd), "llvm-symbolizer -f -e %s 0x%lx", info.dli_fname, address);
		
		FILE *f = popen(cmd, "r");
		if(!f) continue;
		char out[COUNT] = {0};
		char loc[COUNT] = {0};
		if(fgets(out, ArrayCount(out), f) && fgets(loc, ArrayCount(loc), f)) {
			Log("backtrace %s\t%s", out, loc);
		}
		pclose(f);
	}
	_exit(1);
}

internal String8 linux_get_env(const char *var)
{
    AssertLog(environ, "Environment not loaded");
	
    String8 buffer  = {0};
    u64     index   = 0;
    char   *current = environ[index];
    while (environ[index] != NULL) {
        if(CstringAnyMatch(environ[index], var)) {
            buffer = str8(environ[index]);
			
            u64 equal_sign_pos   = 0;
            b32 found_equal_sign = str8_find(&buffer, 0, str8("="), &equal_sign_pos);
			
            if(equal_sign_pos) {
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
internal SharedLibrary linux_load_shared_library(Arena *arena, String8 path)
{
    SharedLibrary lib  = {0};
    ScratchArena scratch = scratch_start(arena);
    void *data = dlopen(null_terminate(scratch.arena, &path), RTLD_LAZY | RTLD_LOCAL);
    AssertLog(data, "failed to load shared object " Str8Fmt, path); // Assert and print the error that get's reported to dlerror
    scratch_end(&scratch);
    lib.mem = data;
    return lib;
}

internal void *linux_shared_library_get_proc_from_name(SharedLibrary *library, const char *name)
{
    void *symbol = dlsym(library->mem, name);
    AssertLog(symbol, "failed to load symbol %s: %s", name, dlerror());
    return symbol;
}

internal int linux_shared_library_close(SharedLibrary *library)
{
    int result = dlclose(library->mem);
    AssertLog(result == 0, "failed to close the library: %s", dlerror());
    library->mem = NULL;
	
    return result;
}

//- implementations
internal b32 linux_file_exists(Arena *arena, String8 path)
{
    ScratchArena scratch = scratch_start(arena);
    cstring null_terminated_path = null_terminate(scratch.arena, &path);
    b32 ok = (access(null_terminated_path, F_OK) == 0);
    scratch_end(&scratch);
    return ok;
}

internal b32 linux_delete_file(Arena *arena, String8 path) {
    ScratchArena scratch = scratch_start(arena);
    int ok = unlink(null_terminate(scratch.arena, &path));
    scratch_end(&scratch);
    return (ok == 0);
}

internal b32 linux_relative_path(Arena *arena, String8 *path)
{
    String8 forward_slash    = str8("./");
    b32     insertion_result = str8_insert(arena, path, &forward_slash, 0);
    if(!insertion_result) {
        breakpoint();
    }
    return insertion_result;
}

internal String8 linux_file_load(Arena *arena, String8 path)
{
    if(path.size <= 0) {
        Log("Can't find the file we we're supposed to open. This is an issue because we we're supposed to break way earlier");
        return {};
    }
	
    String8     result = {0};
    struct stat sbuf   = {0};
	
    if(!path.data) {
        Log("Invalid path file");
        return str8_zero();
    }
	
	
    ScratchArena scratch = scratch_start(arena);
    const char  *null_terminated_path = null_terminate(scratch.arena, &path);
	
    s32 file = open(null_terminated_path, O_RDONLY);
    scratch_end(&scratch);
    if(file == -1) {
        Log("File was not loaded: " Str8Fmt "", Str8Arg(path));
        return str8_zero();
    }
	
    if(fstat(file, &sbuf) == -1) {
        close(file);
        return str8_zero();
    }
	
    result = PushString8(arena, (u64)sbuf.st_size);
	
    u64 file_size = (u64)sbuf.st_size;
    result.size = file_size;
	
    u64 total_bytes_read = 0;
	
    while (total_bytes_read < file_size) {
        s64 bytes = read(file, result.data + total_bytes_read, file_size - total_bytes_read);
        if(bytes == -1) {
            Log("Failed, reading file was interrupted");
            return str8_zero();
        }
		
        if(bytes == 0) { break; }
        total_bytes_read += (u64)bytes;
    }
	
    result.size = total_bytes_read;
	
    close(file);
    return result;
}

// TODO: why was this returning a str8 before?
internal b32 linux_file_write(String8 data, String8 path)
{
    s32 file = open((const char *)path.data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(file == -1) {
        return false;
    }
	
    u64 written = 0;
    while (written < data.size) {
        s64 err = write(file, data.data + written, data.size - written);
        if(err == -1) {
            close(file);
            return false;
        }
        written += err;
    }
	
    close(file);
    return true;
}
internal void linux_log_with_args(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    vprintf(format, arguments);
	putc('\n', stdout);
    fflush(stdout);
    va_end(arguments);
}

internal String8 linux_read_self(Arena *arena) {
    char exe_path[PATH_MAX] = {};
	u64 length = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    AssertLog((length > 0), "failed to read /proc/self/exe\n");
    String8 view = {(u8 *)exe_path, (u64)length};
    return str8_clone(arena, &view);
}


// TODO: rewrite this
internal String8List *linux_open_dir(Arena *arena, String8 *path)
{
    ScratchArena scratch = scratch_start(arena);
    cstring      cpath   = null_terminate(scratch.arena, path);
    DIR         *dir     = opendir(cpath);
    if(!dir) {
        Log("opendir(%s) failed", cpath);
    }
    scratch_end(&scratch);
	
    String8List *files_list = PushStructZero(arena, String8List);
    files_list->first       = &nil_str8_node;
    files_list->last        = &nil_str8_node;
	
    if(!dir) {
        return files_list;
    }
	
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_REG) { // TODO: should symbolic files be handles aswell?
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

