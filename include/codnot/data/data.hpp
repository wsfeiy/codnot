/**
 * @file data.hpp
 * @brief 数据
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-01
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <iostream>
#include <codnot/codnot_export.h>
namespace codnot {
/// @brief 数据空间
namespace data {
/// @brief 字节类型
typedef uint8_t t_byte;
/// @brief 小尺寸定义，支持255B
typedef uint8_t t_s_size;
/// @brief 中尺寸定义，支持64KB
typedef uint16_t t_m_size;
/// @brief 大尺寸定义，支持4096MB
typedef uint32_t t_l_size;
/// @brief 超大尺寸定义，64位
typedef uint64_t t_xl_size;
/// @brief 小尺寸最大值
static const t_s_size t_s_size_max = 255;
/// @brief 中尺寸最大值
static const t_m_size t_m_size_max = 65535;
/// @brief 大尺寸最大值
static const t_l_size t_l_size_max = 4294967295;
/// @brief 超大尺寸最大值
static const t_xl_size t_xl_size_max = 18446744073709551615ULL;
} // namespace data
} // namespace codnot
