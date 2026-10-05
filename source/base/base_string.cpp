internal b32 is_nil_str8(String8 *a) { return (a == &nil_str8); }

internal b32 is_nil_str8_node(String8Node *a) { return (a == &nil_str8_node); }

internal b32 is_nil_str16(String16 *a) { return (a == &nil_str16); }

internal b32 is_nil_str16_node(String16Node *a) { return (a == &nil_str16_node); }

internal b32 is_nil_str32(String32 *a) { return (a == &nil_str32); }

internal b32 is_nil_str32_node(String32Node *a) { return (a == &nil_str32_node); }

internal void str8_trim_left(String8 *buffer)
{
    u64 index = 0;
    while (cstring_is_whitespace(buffer->data[index])) {
        ++index;
    }

    str8_chop_left(*buffer, index);
}

internal void str16_trim_left(String16 *buffer)
{
    u64 index = 0;
    while (cstring_is_whitespace(buffer->data[index])) {
        ++index;
    }

    str16_chop_left(*buffer, index);
}

internal void str32_trim_left(String32 *buffer)
{
    u64 index = 0;

    while (cstring_is_whitespace(buffer->data[index])) {
        ++index;
    }

    str32_chop_left(*buffer, index);
}

internal void str8_trim_right(String8 *buffer)
{
    u64 index = buffer->size;
    while (cstring_is_whitespace(buffer->data[index])) {
        --index;
    }

    str8_chop_right(*buffer, index);
}

internal b32 str8_is_match(String8 a, String8 b)
{
    if (a.size != b.size) {
        return false;
    }

    if(a.size == 0) { // no need to check if b.size is 0 because both sizes are already equal
        return true;
    }

    return (b32)(memcmp(a.data, b.data, b.size) == 0);
}

internal b32 str8_find_all(MemArena *arena, String8 *buffer, u64 start_pos, String8 element, String8List *out)
{
    u64 count     = 0; // checks if any we're found to pass that as a error handling mechanism
    u64 found_pos = 0;

    while (str8_find(buffer, start_pos, element, &found_pos)) {
        String8Node *node = PushStruct(arena, String8Node);
        *node             = ToString8Node(str8_create_substr(buffer, found_pos, element.size));
        str8_list_push_node(out, node);

        ++count;
        start_pos = found_pos + element.size;
    }

    if (!count)
        return false;
    return true;
}

// TODO: does this work like it should?
internal b32 str8_find(String8 *buffer, u64 start_pos, String8 element, u64 *out)
{
    for (u64 index = start_pos; index < buffer->size - element.size; ++index) {
        String8 view = str8_chop_left(*buffer, index);
        view = str8_chop_right(view, view.size - element.size);
        if (str8_is_match(view, element)) {
            *out = index;
            return true;
        }
    }

    return false;
}

internal b32 str8_replace(String8 buffer, String8 element, u64 start_pos) {

    u64 offset = 0;
    b32 result = str8_find(&buffer, start_pos, element, &offset);
    if(!result) return false;

    for(u64 index = 0; index < element.size; ++index) {
        buffer.data[start_pos + index] = element.data[index];
    }

    return true;
}

internal b32 str8_trim(String8 buffer) {

    return str8_replace(buffer, str8("\n"), 0);
}


internal b32 str8_is_digit(String8 a, u64 start_pos)
{
    for (u64 index = start_pos; index < a.size; ++index) {
        if (!cstring_is_digit(a.data[index]))
            return false;
    }

    return true;
}

internal void str8_copy(String8 *dest, String8 *src)
{
    MemCpy(dest->data, src->data, src->size);
    Assert(memcmp(dest->data, src->data, src->size) == 0);
    dest->size = src->size;
}

internal String8 str8_clone(MemArena *arena, String8 *src)
{
    String8 clone = PushString8(arena, src->size);
    str8_copy(&clone, src);
    return clone;
}

// you cant just do this you have to reallocat that thing no?
internal b32 str8_append_char(String8 *dest, u8 src)
{
    if (is_nil_str8(dest) || src == 0)
        return false;
    dest->data[dest->size] = src;
    dest->size += 1;
    return true;
}

internal int str8_to_int(String8 *buffer)
{

    int result = 0;

    for (u64 index = 0; index < buffer->size; ++index) {

        if (buffer->data[index] < 'Z' && buffer->data[index] > 'A') {
            result = buffer->data[index] - '0';
            result *= 10;
        }
    }
    return result;
}

internal b32 str8_append(String8 *dest, String8 src)
{
    if (is_nil_str8(dest) || is_nil_str8(&src))
        return false;
    MemCpy(dest->data + dest->size, src.data, src.size);
    dest->size += src.size;
    return true;
}

internal String8 str8_zero() { return String8{0}; }

// TODO: error handling
internal b32 str8_chop_left_ptr(String8 *buffer, u64 count)
{
    if (buffer->size < count) return false;

    buffer->data += count;
    buffer->size -= count;
    return true;


}

internal b32 str8_chop_right_ptr(String8 *buffer, u64 count)
{
    if (buffer->size < count) return false;

    buffer->size -= count;
    return true;
}

internal String8 str8_chop_left(String8 buffer, u64 count)
{
    if (buffer.size < count) {
        return buffer;
    }

    buffer.data += count;
    buffer.size -= count;

    return buffer;
}

internal String8 str8_chop_right(String8 buffer, u64 count)
{
    if (buffer.size < count) {
        return buffer;
    }

    buffer.size -= count;
    return buffer;
}

internal void str8_list_push_node(String8List *list, String8Node *node)
{
    if (list) {
        if (list->first == NULL) {
            list->first = node;
            list->last  = node;
        } else {
            list->last->next = node;
            list->last       = node;
        }
        list->count += 1;
    }
}

internal String8 str8_create_substr(String8 *buffer, u64 left_offset, u64 right_offset)
{
    return str8_chop_right(str8_chop_left(*buffer, left_offset),
                           (buffer->size - left_offset) -
                           right_offset); // we need to remove all of the excess data to only
    // retain the substring we want
}

internal String16 str16_create_substr(String16 *buffer, u64 left_offset, u64 right_offset)
{
    return str16_chop_right(str16_chop_left(*buffer, left_offset),
                            (buffer->size - left_offset) - right_offset);
}

internal String32 str32_create_substr(String32 *buffer, u64 left_offset, u64 right_offset)
{
    return str32_chop_right(str32_chop_left(*buffer, left_offset),
                            (buffer->size - left_offset) - right_offset);
}

internal String16 str16_chop_left(String16 buffer, u64 count)
{
    if (buffer.size < count) {
        return buffer;
    }

    for (u64 idx = 0; idx < count; ++idx) {
        buffer.data++;
    }
    return buffer;
}

internal String32 str32_chop_left(String32 buffer, u64 count)
{
    if (buffer.size < count) {
        return buffer;
    }

    for (u64 idx = 0; idx < count; ++idx) {
        buffer.data++;
    }
    return buffer;
}

internal String16 str16_chop_right(String16 buffer, u64 count)
{
    if (buffer.size < count) {
        return buffer;
    }

    buffer.size -= count;
    return buffer;
}

internal String32 str32_chop_right(String32 buffer, u64 count)
{
    if (buffer.size < count) {
        return buffer;
    }

    buffer.size -= count;
    return buffer;
}

internal b32 str8_prefix(MemArena *arena, String8 *buffer, String8 *prefix) {
    b32 result = str8_insert(arena, buffer, prefix, 0);
    return result;
}

internal b32 str8_starts_with(String8 buffer, String8 prefix) {

    if(buffer.size < prefix.size) {
        return false;
    }

    return str8_is_match(String8{buffer.data, prefix.size}, prefix);
}

internal b32 str8_is_empty(String8 *buffer)
{
    if (cstring8_length(buffer->data) == 0 || buffer->size == 0)
        return true; // the values can't be lower than 0
    return false;
}

internal b32 str8_insert(MemArena *arena, String8 *dest, String8 *src, u64 pos)
{
    if (dest->size < pos)
        return false;

    String8 result = PushString8(arena, dest->size + src->size);
    result.size    = 0;

    String8 head = str8_create_substr(dest, 0, pos);
    String8 tail = str8_create_substr(dest, pos, dest->size);

    if (!str8_append(&result, head))
        return false;
    if (!str8_append(&result, *src))
        return false;
    if (!str8_append(&result, tail))
        return false;

    *dest = result;
    return true;
}

internal cstring str8_to_cstring(MemArena *arena, String8 *buffer)
{

    cstring null_terminated_buffer = null_terminate(arena, buffer);
    return (cstring)null_terminated_buffer;
}

//- cstring implementations

internal b32 cstring_is_digit(u8 point) { return (point >= '0' && point <= '9'); }

internal b32 cstring_is_alpha(u8 point)
{
    return ((point >= 'a' && point <= 'z') || (point >= 'A' && point <= 'Z') || (point == '_'));
}

internal b32 cstring_is_alpha_num(u8 point)
{
    return (cstring_is_alpha(point) || cstring_is_digit(point));
}

internal b32 cstring_is_whitespace(u8 point) { return (point == ' ' || point == '\t'); }

internal b32 cstring_is_slash(u8 point) { return (point == '/' || point == '\\'); }

internal b32 cstring_is_newline(u8 point) { return (point == '\n' || point == '\r'); }

internal b32 cstring_is_match(u8 *a, u8 *b)
{
    u64 as = cstring8_length(a);
    u64 bs = cstring8_length(b);

    if (as != bs) {
        return false;
    }

    for (u64 idx = 0; idx < as; ++idx) {
        if (a[idx] != b[idx]) {
            return false;
        }
    }
    return true;
}


// NOTE: ignores the biggest buffer size
internal b32 cstring_any_match(u8 *a, u8 *b)
{



    u64 lenght = cstring8_length(b);
    for (u64 idx = 0; idx < lenght; ++idx) {
        if (a[idx] != b[idx]) {
            return false;
        }
    }
    return true;
}

internal u64 cstring8_length(u8 *c)
{
    u8 *p = c;
    if (!c)
        return 0;
    for (; *c != 0; ++c)
    ;
    return (u64)(c - p);
}

internal u64 cstring16_length(u16 *c)
{
    u16 *p = c;

    if (!c)
        return 0;
    for (; *c != 0; ++c)
    ;
    return (u64)(p - c);
}

internal u64 cstring32_length(u32 *c)
{

    u32 *p = c;

    if (!c)
        return 0;
    for (; *c != 0; ++c)
    ;
    return (u64)(p - c);
}

internal cstring null_terminate(MemArena *arena, String8 *buffer)
{
    char *text = (char *)arena_alloc(arena, buffer->size + 1, 0);
    MemCpy(text, buffer->data, buffer->size);
    text[buffer->size] = '\0';
    return (cstring)text;
}

internal String8 remove_null_terminator(MemArena *arena, String8 c)
{
    Assert(arena);
    if ((c.size > 0) && (c.data[c.size - 1] == '\0')) {
        String8 chopped = {c.data, c.size - 1};
        return chopped;
    }

    return c;
}

// returns the index of where to find it. if not found returns 0.
internal u64 cstring8_find(u8 *buffer, u8 *element)
{
    u8 *c = buffer;
    while (c != 0) {
        if (!cstring_is_match(buffer, element)) {
            return c - buffer;
        } else {
            ++c;
        }
    }

    return 0;
}

// returns the index of where to find it. if not found returns 0.
internal u64 cstring8_find_new_line(u8 *buffer)
{
    u8 *c = buffer;
    while (*c != 0) {
        if (cstring_is_newline(*c)) {
            return c - buffer;
        } else {
            ++c;
        }
    }

    return 0;
}

// returns the index of where to find it. if not found returns 0.
internal u64 cstring8_find_needle(u8 *buffer, u8 needle)
{
    u8 *c = buffer;
    while (*c != 0) {
        if (*c == needle) {
            return c - buffer;
        } else {
            ++c;
        }
    }

    return 0;
}
