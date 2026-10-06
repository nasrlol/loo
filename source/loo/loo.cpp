#include "base/base_include.h"

ENTRYPOINT()
{
    MemArena *arena = ArenaCreate(MiB(100));
    Assert(arena);

int fd = wayland_display_connect(arena);

	state_t state = {0};
		state.wl_registry = wayland_wl_display_get_registry(fd),
	state.w = 117,
	state.h = 150,
	state.stride = 117 * color_channels,
	state.shm_pool_size = state.h * state.stride;


	while (1) {
		char read_buf[4096] = "";
		int64_t read_bytes = recv(fd, read_buf, sizeof(read_buf), 0);
		if (read_bytes == -1)
			exit(errno);
		
		char *msg = read_buf;
		uint64_t msg_len = (uint64_t)read_bytes;
		
		while (msg_len > 0)
			wayland_handle_message(fd, &state, &msg, &msg_len);
    }

    return 0;


}
