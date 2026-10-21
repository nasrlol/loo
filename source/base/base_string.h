#ifndef BASE_STRING_H
#define BASE_STRING_H

//- cstring

typedef char *cstring;

// - str8
typedef struct String8  String8;
typedef struct String16 String16;
typedef struct String32 String32;

typedef struct String8Node  String8Node;
typedef struct String16Node String16Node;
typedef struct String32Node String32Node;

typedef struct String8List  String8List;
typedef struct String16List String16List;
typedef struct String32List String32List;

//- strings

struct String8 {
    u8 *data;
    u64 size;
};

struct String16 {
    u16 *data;
    u64  size;
};

struct String32 {
    u32 *data;
    u64  size;
};

//- nodes
struct String8Node {
    String8Node *next;
    String8      value;
};

struct String16Node {
    String16Node *next;
    String16      string;
};

struct String32Node {
    String32Node *next;
    String32      value;
};

//- lists
struct String8List {
    String8Node *first;
    String8Node *last;
    u64          count;
};

struct String16List {
    String16Node *first;
    String16Node *last;
    u64           count;
};

struct String32List {
    String32Node *first;
    String32Node *last;
    u64           count;
};

//- nil / zero-value checks and constructors
internal b32     is_nil_str8(String8 *a);
internal b32     is_nil_str8_node(String8Node *a);
internal b32     is_nil_str16(String16 *a);
internal b32     is_nil_str16_node(String16Node *a);
internal b32     is_nil_str32(String32 *a);
internal b32     is_nil_str32_node(String32Node *a);
internal String8 str8_zero();

//- str8 copy / clone / append
internal String8 str8_clone(MemArena *arena, String8 *src);
internal void    str8_copy(String8 *dest, String8 *src);
internal b32     str8_append(String8 *dest, String8 src);
internal b32     str8_append_char(String8 *dest, u8 src);
internal b32     str8_insert(MemArena *arena, String8 *dest, String8 *src, u64 pos);

//- str8 trim / chop
internal void    str8_trim_left(String8 *buffer);
internal void    str8_trim_right(String8 *buffer);
internal String8 str8_chop_left(String8 buffer, u64 count);
internal String8 str8_chop_right(String8 buffer, u64 count);
internal b32     str8_chop_left_ptr(String8 *buffer, u64 count);
internal b32     str8_chop_right_ptr(String8 *buffer, u64 count);

//- str8 substrings, matching, search
internal String8 str8_create_substr(String8 *buffer, u64 left_offset, u64 right_offset);
internal b32     str8_is_match(String8 a, String8 b);
internal b32     str8_find(String8 *buffer, u64 start_pos, String8 element, u64 *out);
internal b32     str8_find_needle(String8 *buffer, u64 start_pos, u8 *needle, u64 *pos);
internal b32     str8_is_digit(String8 a, u64 start_pos);
internal b32     str8_is_empty(String8 *buffer);

//- str8 list
internal void str8_list_push_node(String8List *list, String8Node *node);

//- str16 / str32 chop and substrings
internal String16 str16_chop_left(String16 buffer, u64 count);
internal String16 str16_chop_right(String16 buffer, u64 count);
internal String16 str16_create_substr(String16 *buffer, u64 left_offset, u64 right_offset);
internal String32 str32_chop_left(String32 buffer, u64 count);
internal String32 str32_chop_right(String32 buffer, u64 count);
internal String32 str32_create_substr(String32 *buffer, u64 left_offset, u64 right_offset);

//- null-termination helpers
internal cstring null_terminate(MemArena *arena, String8 *c);
internal String8 remove_null_terminator(MemArena *arena, String8 c);

//- convert

internal cstring str8_to_cstring(String8 *buffer);

//- cstring character classification
internal b32 cstring_is_digit(u8 point);
internal b32 cstring_is_alpha(u8 point);
internal b32 cstring_is_alpha_num(u8 point);
internal b32 cstring_is_whitespace(u8 point);
internal b32 cstring_is_slash(u8 point);
internal b32 cstring_is_newline(u8 point);

#define CstringIsMatch(a, b) cstring_is_match((u8 *)(a), (u8 *)(b))
#define CstringAnyMatch(a, b) cstring_any_match((u8 *)(a), (u8 *)(b))


//- cstring matching, length, search
internal b32 cstring_is_match(u8 *a, u8 *b);
internal b32 cstring_any_match(u8 *a, u8 *b);
internal u64 cstring8_length(u8 *c);
internal u64 cstring16_length(u16 *c);
internal u64 cstring32_length(u32 *c);
internal u64 cstring8_find(u8 *buffer, u8 *element);
internal u64 cstring8_find_new_line(u8 *buffer);
internal u64 cstring8_find_needle(u8 *buffer, u8 needle);

//-

#if COMPILER_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

//-
ReadOnly global_variable String8  nil_str8  = {NULL, 0};
ReadOnly global_variable String16 nil_str16 = {NULL, 0};
ReadOnly global_variable String32 nil_str32 = {NULL, 0};

ReadOnly global_variable String8Node  nil_str8_node  = {NULL, {}};
ReadOnly global_variable String16Node nil_str16_node = {NULL, {}};
ReadOnly global_variable String32Node nil_str32_node = {NULL, {}};
ReadOnly global_variable String8List  nil_str8_list  = {NULL, {}};
ReadOnly global_variable String16List nil_str16_list = {NULL, {}};
ReadOnly global_variable String32List nil_str32_list = {NULL, {}};

//- macros
#define PushString8(arena, count) {(u8 *)(PushArrayZero(arena, u8, (count))), (u64)(count)}
#define PushString16(arena, count) {(u16 *)(PushArrayZero(arena, u16, (count))), (u64)(count)}
#define PushString32(arena, count) {(u32 *)(PushArrayZero(arena, u32, (count))), (u64)(count)}

// TODO: is this null terminated? this is generally a concept i struggle with. when is the string
// null terminated and when not. because in thi case I am the one passing a string "/" or something
// like that. is that null terminated or not?
#define ToString8(data)                                  \
                                                           String8                                              \
                                                           {                                                    \
                                                           (u8 *)(data), (u64)(cstring8_length((u8 *)data)) \
                                                           }
                                                           #define ToString16(data)                                    \
                                                           String16                                                \
                                                           {                                                       \
                                                           (u16 *)(data), (u64)(cstring16_length((u16 *)data)) \
                                                           }
                                                           #define ToString32(data)                                    \
                                                           String32                                                \
                                                           {                                                       \
                                                           (u32 *)(data), (u64)(cstring32_length((u32 *)data)) \
                                                           }

                                                           #define str8(buffer) ToString8(buffer)
                                                           #define str16(buffer) ToString16(buffer)
                                                           #define str32(buffer) ToString32(buffer)

                                                           #define ToString8Size(data, size) \
                                                           String8                       \
                                                           {                             \
                                                           (u8 *)(data), (u64)(size) \
                                                           }
                                                           #define ToString16Size(data, size) \
                                                           String16                       \
                                                           {                              \
                                                           (u16 *)(data), (u64)(size) \
                                                           }
                                                           #define ToString32Size(data, size) \
                                                           String32                       \
                                                           {                              \
                                                           (u32 *)(data), (u64)(size) \
                                                           }

                                                           #define ToString8Node(string)  \
                                                           String8Node                \
                                                           {                          \
                                                           &nil_str8_node, string \
                                                           }
                                                           #define ToString16Node(string) \
                                                           String16Node               \
                                                           {                          \
                                                           &nil_str8_node, string \
                                                           }
                                                           #define ToString32Node(string) \
                                                           String32Node               \
                                                           {                          \
                                                           &nil_str8_node, string \
                                                           }

                                                           #define str8_node(text) ToString8Node(str8(text))
                                                           #define str16_node(text) ToString16Node(str16(text))
                                                           #define str32_node(text) ToString32Node(str32(text))

                                                           #define LOOP(expr, exec) for (; expr; exec)

                                                           // arbitratry amount of parameters can be passed to this function
                                                           // TODO: put the index name as a paramter so that the reference can be made later without it being
                                                           // so hidden

                                                           #define FOR_STR8_CHOP_LEFT(index, buffer, count) FOR_STR8_CHOP(index, buffer, count, str8_chop_left)

                                                           #define FOR_STR8_CHOP_RIGHT(index, buffer, count) \
                                                           FOR_STR8_CHOP(index, buffer, count, str8_chop_right)

                                                           // NOTE: we face an issue with returning an rvalue. to solve this we should only use the pointer
                                                           // functions that we create (str8_chop_left_ptr and str8_chop_right_ptr)
                                                           #define FOR_STR8_CHOP(index, buffer, count, function)                         \
                                                           for (u64 index = 0; (index < (buffer.size)) && ((buffer.size) > (count)); \
                                                           function((buffer), (count)), ++index)

                                                           #define FOR_STR8_ARB_FN(expr, function, ...) LOOP(expr, function(__VA_ARGS__))

                                                           #define FOR_STR8_MATCH(buffa, buffb) LOOP(str8_is_match(buffa, buffb), str8_chop_left(buffa, 1))

                                                           #define FOR_STR8_NODE(str8_list, current)                                     \
                                                           for (String8Node *current = str8_list->first; !is_nil_str8_node(current); \
                                                           current              = current->next)

internal b32 is_nil_str8(String8 *a);
internal b32 is_nil_str8_node(String8Node *a);
internal b32 is_nil_str16(String16 *a);
internal b32 is_nil_str16_node(String16Node *a);
internal b32 is_nil_str32(String32 *a);
internal b32 is_nil_str32_node(String32Node *a);

#if COMPILER_GCC
#pragma GCC diagnostic pop
#endif

#define Str8Fmt "%.*s"
#define Str8Arg(buffer) (u64)((buffer).size), (char *)((buffer).data)

#endif /* BASE_STRING_H */
