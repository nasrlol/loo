#ifndef BASE_PROFILE_H
#define BASE_PROFILE_H


typedef struct Profile Profile;
struct Profile {
    f32 start;
    f32 end;
    // TODO: is there a way to measure the cpu cycles?
    // frame time if graphics is enabled

#if GRAPHICS
    f32 frame_time;
#endif
};

#endif // BASE_PROFILE_H