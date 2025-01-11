/**
 * @file engine.h
 * @brief 引擎头文件，包含引擎相关api
 * @details
 * 创建一个游戏对象，然后传递给引擎运行启动游戏
 * <br>
 * 使用示例：
 * @code {.cpp}
 *
 * @endcode
 *
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-10
 * @copyright © 2025 feiy. All rights reserved.
 */
#pragma once

#include "../codnot_export.h"
#include <iostream>
namespace codnot
{
    /// @brief 引擎相关api
    namespace engine
    {
        /// @brief 游戏配置
        struct game_options;
        /// @brief 引擎
        class engine;

        struct game_options
        {
            /// @brief 游戏名称
            std::string name = "";
            /// @brief 版本号
            std::string version = "v1.0.0";
            /// @brief 作者
            std::string author = "anyone";
            /// @brief 描述
            std::string description = "这是默认游戏描述！";
            /// @brief 使用多线程（仅作为建议，这会根据当前系统进行选择优化）
            bool enable_mutil_thread = false;
        };

        class engine
        {
        private:
            game_options options_;

        public:
            engine(game_options options) : options_(options)
            {
            }
            
        };

    }
}