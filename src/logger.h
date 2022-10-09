#pragma once

#include "defines.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


// Disable debug and trace logging for release builds.


enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
};
void* platform_copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size);
}

void* kcopy_memory(void* dest, const void* source, u64 size) {
    return platform_copy_memory(dest, source, size);
}

void platform_console_write(const char* message, u8 colour) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
}
void platform_console_write_error(const char* message, u8 colour) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
}

template<typename T> i32 string_format_v(char* dest, const char* format, T t) {
    if (dest) {
        // Big, but can fit on the stack.
        char buffer[32000];
        i32 written = vsnprintf(buffer, 32000, format, t );
        buffer[written] = 0;
        kcopy_memory(dest, buffer, written + 1);
        
        return written;
    }
    return -1;
}

i32 string_format(char* dest, const char* format, ...) {
    if (dest) {
        __builtin_va_list arg_ptr;
        va_start(arg_ptr, format);
        i32 written = string_format_v(dest, format, arg_ptr);
        va_end(arg_ptr);
        return written;
    }
    return -1;
}

void log_output(log_level level, const char* message, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    b8 is_error = level < LOG_LEVEL_WARN;
    
    // Technically imposes a 32k character limit on a single log entry, but...
    // DON'T DO THAT!
    char out_message[32000];
    //kzero_memory(out_message, sizeof(out_message));
    memset(out_message,0,  sizeof(out_message));
    // Format original message.
    // NOTE: Oddly enough, MS's headers override the GCC/Clang va_list type with a "typedef char* va_list" in some
    // cases, and as a result throws a strange error here. The workaround for now is to just use __builtin_va_list,
    // which is the type GCC/Clang's va_start expects.
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    string_format_v(out_message, message, arg_ptr);
    va_end(arg_ptr);
    
    // Prepend log level to message.
    string_format(out_message, "%s%s\n", level_strings[level], out_message);
    
    // Print accordingly
    if (is_error) {
        platform_console_write_error(out_message, level);
    } else {
        platform_console_write(out_message, level);
    }
}

// Logs a fatal-level message.
#define KFATAL( message,  ...) //log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

// Logs an error-level message.
#define KERROR(message, ...) //log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);


#define KWARN(message,...) //log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);


#define KINFO(message, ...) /*log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);*/




#define KDEBUG(message, ...) //log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);



// Logs a trace-level message.
#define KTRACE(message, ...) //log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);






void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}

#define KASSERT(expr)                                                \
{                                                                \
if (expr) {                                                  \
} else {                                                     \
report_assertion_failure(#expr, "", __FILE__, __LINE__); \
debugBreak();                                            \
}                                                            \
}

#define KASSERT_MSG(expr, message)                                        \
{                                                                     \
if (expr) {                                                       \
} else {                                                          \
report_assertion_failure(#expr, message, __FILE__, __LINE__); \
debugBreak();                                                 \
}                                                                 \
}


#define KASSERT_DEBUG(expr)                                          \
{                                                                \
if (expr) {                                                  \
} else {                                                     \
report_assertion_failure(#expr, "", __FILE__, __LINE__); \
debugBreak();                                            \
}                                                            \
}
