#include "string_stream.h"
#include <ctype.h>
#include <stdio.h>

static const char* string_stream_skip_space(const char *str)
{
    const char *ptr = str;

    if (ptr)
    {
        while (*ptr && isspace(*ptr))
            ++ptr;
    }

    return ptr;
}

static const char* string_stream_skip_element(const char *str)
{
    const char *ptr = str;

    if (ptr)
    {
        while (*ptr && !isspace(*ptr))
            ++ptr;
    }

    return string_stream_skip_space(ptr);
}

void string_stream_init(string_stream_t *stream, const char *str)
{
    stream->str = str;
    stream->curr_position = string_stream_skip_space(str);
}

void string_stream_move_to_next_element(string_stream_t *stream)
{
    stream->curr_position = string_stream_skip_element(stream->curr_position);
}

extern const char* string_stream_curr_element(string_stream_t *stream)
{
    const char *ptr = stream->curr_position;
    return (*ptr == 0) ? 0 : ptr;
}
