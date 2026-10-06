// https://gaultier.github.io/blog/wayland_from_scratch.html#creating-a-registry
// TODO: replace unix_get_env with a generic macro



//- wayland
internal int wayland_display_connect(MemArena *arena) {

    String8 str8_xdg_runtime_dir = unix_get_env("XDG_RUNTIME_DIR");
    char *xdg_runtime_dir =  null_terminate(arena, &str8_xdg_runtime_dir);
    sockaddr_un addr = {};
    u64 socket_path_len = 0;

// copy the xdg_runtime_dir value to the beginning of the bfufer
{
    addr.sun_family = AF_UNIX;
    MemCpy(addr.sun_path, xdg_runtime_dir, str8_xdg_runtime_dir.size);
    socket_path_len += str8_xdg_runtime_dir.size;
    addr.sun_path[socket_path_len++] = '/'; // add an ending slash
}

    String8 str8_wayland_display_value = unix_get_env("WAYLAND_DISPLAY");
Assert(str8_wayland_display_value.size != 0);
    char *wayland_display = null_terminate(arena, &str8_wayland_display_value);

    if(wayland_display == NULL)
    {
        const char *wayland_display_default = "wayland-0";
        u64 wayland_display_default_len = cstring8_length((u8 *)wayland_display_default);
        MemCpy(addr.sun_path, wayland_display_default, wayland_display_default_len);
    }
    else
    {
        MemCpy(addr.sun_path + str8_xdg_runtime_dir.size + 1, wayland_display, str8_wayland_display_value.size);
}

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

#if DEBUG // helps to read those errors? i guess i can just read stderr?



if(fd == -1)
{
perror("failed to open socket");
}


if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
{
perror("failed to connect to socket");

}

#endif


    AssertLog((fd != -1), "failed to open socket\n");
    AssertLog((connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1), "failed to connect to socket\n");

return fd;

}


//- helper functions

internal void buf_write_u32(char *buf, u64 *buf_size, u64 buf_cap,
                            u32 x) {
    Assert(*buf_size + sizeof(x) <= buf_cap);
    Assert(((size_t)buf + *buf_size) % sizeof(x) == 0);

    *(u32 *)(buf + *buf_size) = x;
    *buf_size += sizeof(x);
}

internal void buf_write_u16(char *buf, u64 *buf_size, u64 buf_cap,
                            u16 x) {
    Assert(*buf_size + sizeof(x) <= buf_cap);
    Assert(((size_t)buf + *buf_size) % sizeof(x) == 0);

    *(u16 *)(buf + *buf_size) = x;
    *buf_size += sizeof(x);
}

internal void buf_write_string(char *buf, u64 *buf_size, u64 buf_cap,
                               char *src, u32 src_len) {
    Assert(*buf_size + src_len <= buf_cap);

    buf_write_u32(buf, buf_size, buf_cap, src_len);
    memcpy(buf + *buf_size, src, roundup_4(src_len));
    *buf_size += roundup_4(src_len);
}

internal u32 buf_read_u32(char **buf, u64 *buf_size) {
    Assert(*buf_size >= sizeof(u32));
    Assert((size_t)*buf % sizeof(u32) == 0);

    u32 res = *(u32 *)(*buf);
    *buf += sizeof(res);
    *buf_size -= sizeof(res);

    return res;
}

internal u16 buf_read_u16(char **buf, u64 *buf_size) {
    Assert(*buf_size >= sizeof(u16));
    Assert((size_t)*buf % sizeof(u16) == 0);

    u16 res = *(u16 *)(*buf);
    *buf += sizeof(res);
    *buf_size -= sizeof(res);

    return res;
}

internal void buf_read_n(char **buf, u64 *buf_size, char *dst, u64 n) {
    Assert(*buf_size >= n);

    memcpy(dst, *buf, n);

    *buf += n;
    *buf_size -= n;
}

global_variable u64 wayland_current_id = 1;

internal u32 wayland_wl_display_get_registry(int fd) {
    uint64_t msg_size = 0;
    char msg[128] = "";
    buf_write_u32(msg, &msg_size, sizeof(msg), wayland_display_object_id);

    buf_write_u16(msg, &msg_size, sizeof(msg),
                  wayland_wl_display_get_registry_opcode);

    u16 msg_announced_size = wayland_header_size + sizeof(wayland_current_id);
    Assert(roundup_4(msg_announced_size) == msg_announced_size);
    buf_write_u16(msg, &msg_size, sizeof(msg), msg_announced_size);

    wayland_current_id++;
    buf_write_u32(msg, &msg_size, sizeof(msg), wayland_current_id);

    if ((s64)msg_size != send(fd, msg, msg_size, MSG_DONTWAIT))
        exit(1);

    Log("-> wl_display@%u.get_registry: wl_registry=%u\n",
        wayland_display_object_id, wayland_current_id);

    return wayland_current_id;
}



 internal void create_shared_memory_file(u64 size, state_t *state) {
	char name[255] = "/";
	for (u64 index = 1; index < cstring8_length((u8 *)name); ++index) {
		name[index] = ((double)rand()) / (double)RAND_MAX * 26 + 'a';
	}
	
	int fd = shm_open(name, O_RDWR | O_EXCL | O_CREAT, 0600);
	 AssertLog((fd != -1), "Failed to open buffer for shared memory.");
	
	Assert(shm_unlink(name) != -1);
	
	 AssertLog((ftruncate(fd, size) != -1), "Failed to truncate buffer.");
	
	state->shm_pool_data = (u8 *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	Assert((void*)-1 != state->shm_pool_data);
	Assert(state->shm_pool_data != NULL);
	state->shm_fd = fd;
}

 internal void wayland_handle_message(int fd, state_t *state, char **msg,
                                   uint64_t *msg_len) {
	Assert(*msg_len >= 8);
	
	 u32 object_id = buf_read_u32(msg, msg_len);
	Assert(object_id <= wayland_current_id);
	
	 u16 opcode = buf_read_u16(msg, msg_len);
	
	 u16 announced_size = buf_read_u16(msg, msg_len);
	Assert(roundup_4(announced_size) <= announced_size);
	
	 u32 header_size = sizeof(object_id) + sizeof(opcode) + sizeof(announced_size);
Assert(announced_size <= header_size + *msg_len);
	
	if (object_id == state->wl_registry &&
		opcode == wayland_wl_registry_event_global) {
		// TODO
	}
	// Following: Lots of `if (opcode == ...) {... } else if (opcode = ...) { ... } [...]`
}
