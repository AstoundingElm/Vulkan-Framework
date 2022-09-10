#pragma once
static bool vulkan_running = false;
#include <cstdint>
#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))
#include <stdio.h>

#define VK_CHECK(result)                    \
if (result != VK_SUCCESS) {             \
printf("Vulkan Error: %d", result); \
__builtin_trap();                   \
}\

typedef uint32_t u32;
typedef uint64_t u64;

#define global_variable static
#define internal static
#define local_persist static
#define PINLINE static inline

#define KB(x) ((uint64_t)1024 * x)
#define MB(x) ((uint64_t)1024 * KB(1))
#define GB(x) ((uint64_t)1024 * MB(1))

