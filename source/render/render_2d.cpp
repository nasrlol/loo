
// normalized device coordintates
internal Vec2 NDC(s32 width, s32 height, Vec2 pos) {
 s32 center_x = (s32)((pos.x + 1.0f) * 0.5f * width);
 s32 center_y = (s32)((1.0f - pos.y) * 0.5f * height);
 return Vec2(center_x - width / 2, center_y - height / 2);

}

