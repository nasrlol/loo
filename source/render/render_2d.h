#ifndef RENDER_2D_H
#define RENDER_2D_H

//- colors
// TODO: should go in a header file later

typedef struct RGB RGB;
struct RGB { u8 r; u8 g; u8 b; };

#define BLACK   ((RGB){  0,   0,   0})
#define WHITE   ((RGB){255, 255, 255})
#define RED     ((RGB){255,   0,   0})
#define GREEN   ((RGB){  0, 255,   0})
#define BLUE    ((RGB){  0,   0, 255})
#define YELLOW  ((RGB){255, 255,   0})
#define CYAN    ((RGB){  0, 255, 255})
#define MAGENTA ((RGB){255,   0, 255})

#define GRAY       ((RGB){128, 128, 128})
#define LIGHT_GRAY ((RGB){192, 192, 192})
#define DARK_GRAY  ((RGB){ 64,  64,  64})

#define ORANGE ((RGB){255, 165,   0})
#define PURPLE ((RGB){128,   0, 128})
#define PINK   ((RGB){255, 192, 203})
#define BROWN  ((RGB){139,  69,  19})
#define NAVY   ((RGB){  0,   0, 128})
#define TEAL   ((RGB){  0, 128, 128})
#define OLIVE  ((RGB){128, 128,   0})
#define MAROON ((RGB){128,   0,   0})
#define LIME   ((RGB){ 50, 205,  50})
#define GOLD   ((RGB){255, 215,   0})


typedef struct Plane Plane;
struct Plane {
 Vec2 n;
 f32 d;
};


#endif // RENDER_2D_H