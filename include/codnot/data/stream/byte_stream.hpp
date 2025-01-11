/**
 * @file byte_stream.hpp
 * @brief
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-02
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <codnot/data/stream/buffer.hpp>
#include <codnot/codnot_export.h>
namespace codnot
{
  namespace data
  {
    namespace stream
    {
      /**
       * @brief 判断当前平台是否为小端序
       * @return bool
       */
      bool is_little_endian();
      /**
       * @brief 交换字节序
       * @param std::vector<t_byte> &bytes
       */
      void swap_endian(std::vector<t_byte> &bytes);
      /// @brief 字节流类
      class t_byte_stream;
      /// @brief 字节数量枚举
      enum class et_byte_num;

      enum class et_byte_num
      {
        num_1 = 1,
        num_2 = 2,
        num_3 = 3,
        num_4 = 4,
        num_5 = 5,
        num_6 = 6,
        num_7 = 7,
        num_8 = 8,
        num_9 = 9
      };
      /**
       * @brief 字节流
       */
      class t_byte_stream : public t_base_buffer<t_byte>
      {
        bool is_little_endian_ = is_little_endian();

      public:
        t_byte_stream() = default;

        t_byte_stream(std::string str);
        /**
         * @brief 写入字节，处理字节序
         *
         * @param bytes
         */
        void write_bytes(std::vector<t_byte> &bytes);
        /**
         * @brief 读取指定长度的字节，处理字节序
         *
         * @param size
         * @return std::vector<t_byte>
         */
        std::vector<t_byte> read_bytes(t_xl_size size);
        /// @brief 写入小尺寸（1字节）
        void write_s_size(t_s_size val);
        /// @brief 读取小尺寸
        t_s_size read_s_size();
        /// @brief 写入中尺寸（2字节）
        void write_m_size(t_m_size val);
        /// @brief 读取中尺寸
        t_m_size read_m_size();
        /// @brief 写入大尺寸（4字节）
        void write_l_size(t_l_size val);
        /// @brief 读取大尺寸
        t_l_size read_l_size();
        /// @brief 写入超大尺寸（8字节）
        void write_xl_size(t_xl_size val);
        /// @brief 读取超大尺寸
        t_xl_size read_xl_size();
        ~t_byte_stream() = default;
      };
    } // namespace stream
  } // namespace data
} // namespace codnot