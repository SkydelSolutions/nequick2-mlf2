#ifndef _STRING_STREAM_H_
#define _STRING_STREAM_H_

typedef struct string_stream_st
{
    const char *str;
    const char *curr_position;
} string_stream_t;

extern void string_stream_init(string_stream_t *stream, const char *str);
extern void string_stream_move_to_next_element(string_stream_t *stream);
extern const char* string_stream_curr_element(string_stream_t *stream);

#endif // _STRING_STREAM_H_
