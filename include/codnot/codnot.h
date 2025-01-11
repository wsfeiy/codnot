/**
 * @file codnot.h
 * @brief
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2024-12-30
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <iostream>
#include "codnot_export.h"

namespace codnot{
        /**
     * @brief 获取版本以及相关信息
     *
     * @return void
     */
    _CODNOT_DECLARE_ void version();
}

#ifdef __cplusplus
extern "C"
{
#endif

_CODNOT_DECLARE_ void codnot_version(){
    codnot::version();
}

#ifdef __cplusplus
}
#endif