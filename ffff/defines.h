#pragma once

#include <stdint.h>
#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))

#define VK_CHECK(result)                    \
    if (result != VK_SUCCESS) {             \
        printf("Vulkan Error: %d", result); \
        __builtin_trap();                   \
    }

typedef uint32_t u32;

#define global_variable static
#define internal static
#define local_persist static
#define PINLINE static inline