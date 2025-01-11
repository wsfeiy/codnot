#pragma once

#if defined(__WIN__) || defined(__WIN32) || defined(__WIN64)
    // Windows 平台导出宏定义
#   ifdef CODNOT_BUILD
#       define _CODNOT_DECLARE_ __declspec(dllexport)
#   else
#       define _CODNOT_DECLARE_ __declspec(dllimport)
#   endif
/// @brief Windows 系统
#define _WIN_SYS_ 1

#elif defined(__linux__) || defined(__gnu_linux__)
    // Linux 平台导出宏定义
#   define _CODNOT_DECLARE_ __attribute__((visibility("default")))

/// @brief Linux 系统
#define _LINUX_SYS_ 1

#elif defined(__APPLE__) || defined(__MACH__) || defined(__MACOS__)
    // macOS 平台导出宏定义
#   define _CODNOT_DECLARE_ __attribute__((visibility("default")))

/// @brief macOS 系统
#define _MACOS_SYS_ 1

#elif defined(__ANDROID__)
    // Android 平台导出宏定义
#   define _CODNOT_DECLARE_ __attribute__((visibility("default")))

/// @brief Android 系统
#define _ANDROID_SYS_ 1
#else
    // 默认情况下（其他平台）
#   define _CODNOT_DECLARE_
#endif