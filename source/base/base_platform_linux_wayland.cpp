// TODO: replace unix_get_env with a generic macro

//- wayland
internal void wayland_display_connect(MemArena *arena) {


ScratchArena env_scratch = scratch_start(arena);


    String8 str8_xdg_runtime_dir = unix_get_env("XDG_RUNTIME_DIR");
    char *xdg_runtime_dir =  null_terminate(arena, &str8_xdg_runtime_dir);

u64 xdg_runtime_dir_len = str8_xdg_runtime_dir.size;

 sockaddr_un addr = {};
addr.sun_family = AF_UNIX;

		   Assert(xdg_runtime_dir_len <= cstring8_length((u8 *)addr.sun_path));

u64 socket_path_len = 0;
MemCpy(addr.sun_path, xdg_runtime_dir, str8_xdg_runtime_dir.size);
socket_path_len += xdg_runtime_dir_len;


addr.sun_path[socket_path_len++] = '/';
String8 str8_wayland_display_value = unix_get_env("WAYLAND_DISPLAY");
char *wayland_display = null_terminate(arena, &str8_wayland_display_value);
if(wayland_display == NULL)
{
const char *wayland_display_default = "wayland-0";
u64 wayland_display_default_len = cstring8_length((u8 *)wayland_display_default);
MemCpy(addr.sun_path + socket_path_len, wayland_display_default, wayland_display_default_len);
}
else
{
u64 wayland_display_len = cstring8_length((u8 *)wayland_display);
MemCpy(addr.sun_path + socket_path_len, wayland_display, wayland_display_len);
socket_path_len += wayland_display_len;
}

scratch_end(&env_scratch);

int fd = socket(AF_UNIX, SOCK_STREAM, 0);
if(fd == -1)
{
exit(1);
}

if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
{
exit(1);
}


}