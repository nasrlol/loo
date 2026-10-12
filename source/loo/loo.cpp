#include "base/base_include.h"

ENTRYPOINT()
{
    MemArena *arena = ArenaCreate(MiB(100));
    Assert(arena);

    LinuxWindowParams *params = PushStruct(arena, LinuxWindowParams);
    params->name = (cstring)"Loo";

    wayland_entrypoint(arena, params);

    return 0;
}
