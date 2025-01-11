/**
 * @file sery.hpp
 * @brief 序列化
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-01
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <any>
#include <codnot/data/stream/byte_stream.hpp>
#include <codnot/data/struct/bit_map.hpp>
#include <functional>
#include <memory>

namespace codnot
{
  namespace data
  {
    /// @brief 序列化空间
    namespace sery
    {
      /// @brief 序列化基类申明
      class t_serialize_base;
      /// @brief 序列化器申明
      class _CODNOT_DECLARE_ t_serializer;
      /// @brief 解序列化器申明
      class _CODNOT_DECLARE_ t_deserializer;
      /// @brief 可序列化接口
      class _CODNOT_DECLARE_ it_serializable;
      /// @brief 可序列化类注册表
      class _CODNOT_DECLARE_ t_serializable_class_registry;
      /// @brief 工厂函数和ID联合结构体
      struct st_union_class_id;
      /// @brief 字节流
      using byte_stream = codnot::data::stream::t_byte_stream;
      /// @brief 工厂函数
      using factory_func = std::function<std::unique_ptr<it_serializable>()>;
      /**
       * @brief 可序列化类注册表
       *
       */
      class t_serializable_class_registry
      {
      public:
        using bit_map_id_allocate = codnot::data::structure::t_bit_map<t_m_size_max>;
        /**
         * @brief 通过类型名称获取ID
         * @param name 类型名称
         * @return id
         */
        t_m_size get_id_by_name(const std::string &name);

        /**
         * @brief 通过ID获取类型信息
         *
         * @param id
         * @return const st_union_class_id&
         */
        const st_union_class_id &get_union_class_id(t_m_size id);

        /**
         * @brief 分配ID
         *
         */
        t_m_size allocate();

        /**
         * @brief 注册类型
         *
         * @param name
         * @param func
         */
        void reg_class(std::string name, factory_func func);

        /**
         * @brief 创建对象实例
         *
         * @param id
         * @return std::unique_ptr<it_serializable> 对象指针
         */
        std::unique_ptr<it_serializable> create_instance(t_m_size id);

        static t_serializable_class_registry &get_instance();

      private:
        bit_map_id_allocate id_bit_map_;
        std::unordered_map<std::string, st_union_class_id> map_;
        t_serializable_class_registry() = default;
      };
      class t_serialize_base
      {
      protected:
        t_serialize_base() = default;
        t_serialize_base(std::string str) : buffer_(str) {}
        virtual ~t_serialize_base() = default;

      public:
        byte_stream buffer_;
      };

      class t_serializer : public t_serialize_base
      {
      public:
        t_serializer() = default;

        void write_int(int val);
        void write_float(float val);
        void write_double(double val);
        void write_bool(bool val);
        /// @brief 写入字符串，只支持2的32次幂尺寸大小
        void write_string(std::string val);
        /// @brief 写入数组
        /// @tparam T
        /// @param vec
        template <typename T>
        void write_array(const std::vector<T> &vec)
        {
          write_int(vec.size());
          if constexpr (std::is_integral_v<T>)
          {
            for (T item : vec)
            {
              write_int(item);
            }
          }
          else if constexpr (std::is_same_v<T, float>)
          {
            for (T item : vec)
            {
              write_float(item);
            }
          }
          else if constexpr (std::is_same_v<T, double>)
          {
            for (T item : vec)
            {
              write_double(item);
            }
          }
          else if constexpr (std::is_same_v<T, std::string>)
          {
            for (const std::string &item : vec)
            {
              write_string(item);
            }
          }
          else if constexpr (std::is_base_of_v<it_serializable, T>)
          {
            for (T *item : vec)
            {
              item->serialize(*this);
            }
          }
          else
          {
            throw std::runtime_error("Not a serializable type in vector");
          }
        }
        std::string to_string();
      };

      class t_deserializer : public t_serialize_base
      {
      public:
        t_deserializer() = default;
        t_deserializer(std::string str) : t_serialize_base(str) {}
        int read_int();
        float read_float();
        double read_double();
        bool read_bool();
        std::string read_string();
        /// @brief 读取数组
        /// @tparam T
        /// @return std::vector<T>
        template <typename T>
        std::vector<T> read_array()
        {
          // 读取数组的大小
          int size = read_int();
          std::vector<T> vec(size);

          // 根据类型读取每个元素
          if constexpr (std::is_same_v<T, int>)
          {
            for (int i = 0; i < size; ++i)
            {
              vec[i] = read_int();
            }
          }
          else if constexpr (std::is_same_v<T, float>)
          {
            for (int i = 0; i < size; ++i)
            {
              vec[i] = read_float();
            }
          }
          else if constexpr (std::is_same_v<T, double>)
          {
            for (int i = 0; i < size; ++i)
            {
              vec[i] = read_double();
            }
          }
          else if constexpr (std::is_same_v<T, std::string>)
          {
            for (int i = 0; i < size; ++i)
            {
              vec[i] = read_string();
            }
          }
          else if constexpr (std::is_base_of_v<it_serializable, T>)
          {
            for (int i = 0; i < size; ++i)
            {
              auto instance_ptr =
                  t_serializable_class_registry::get_instance().create_instance(
                      read_int());
              if (!instance_ptr)
              {
                throw std::runtime_error(
                    "Failed to create instance during deserialization");
              }
              T *instance = dynamic_cast<T *>(instance_ptr.get());
              if (!instance)
              {
                throw std::runtime_error("Type mismatch during deserialization");
              }
              instance->deserialize(*this);
              vec[i] = *instance;
            }
          }
          else
          {
            throw std::runtime_error("Not a deserializable type in vector");
          }

          return vec;
        }
      };
      /**
       * @brief 可序列化接口
       *
       */
      class it_serializable
      {
      public:
        virtual ~it_serializable() = default;
        /**
         * @brief 序列化数据到缓冲
         *
         */
        virtual void serialize(t_serializer &ts) = 0;
        /**
         * @brief 从缓冲反序列化数据到成员
         *
         */
        virtual void deserialize(t_deserializer &ds) = 0;
      };

      struct st_union_class_id
      {
        t_m_size id;
        std::string name;
        factory_func func_;
      };

/**
 * @brief 注册实现了it_serializable的类
 *
 * @param T
 */
#define REG_SERY_CLASS(T)                                                                                        \
  namespace                                                                                                      \
  {                                                                                                              \
    struct reg_sery_class_##T                                                                                    \
    {                                                                                                            \
      reg_sery_class_##T()                                                                                       \
      {                                                                                                          \
        namespace sery = codnot::data::sery;                                                                     \
        sery::t_serializable_class_registry::get_instance().reg_class(                                           \
            typeid(T).name(), []() -> std::unique_ptr<sery::it_serializable> { return std::make_unique<T>(); }); \
      }                                                                                                          \
    } reg_sery_class_##T;                                                                                        \
  }
      /**
       * @brief 序列化类型概念检查
       *
       * @tparam T
       */
      template <typename T>
      concept serializable = std::is_base_of_v<it_serializable, T>;

      /**
       * @brief 序列化对象
       *
       * @tparam T
       * @param t_serializer ser
       * @return T
       */
      template <typename T>
        requires serializable<T>
      std::string serialize(t_serializer &ser, T &obj)
      {
        // 写入标识符
        ser.write_int(t_serializable_class_registry::get_instance().get_id_by_name(
            typeid(T).name()));
        // 写入长度
        // 写入载荷
        obj.serialize(ser);
        return ser.to_string();
      }
      /**
       * @brief 反序列化对象
       *
       * @tparam T
       * @param t_deserializer des
       * @return T
       */
      template <typename T>
        requires serializable<T>
      T deserialize(t_deserializer &des)
      {
        try
        {
          uint16_t id = static_cast<uint16_t>(des.read_int());

          auto instance_ptr =
              t_serializable_class_registry::get_instance().create_instance(id);
          if (!instance_ptr)
          {
            throw std::runtime_error(
                "Failed to create instance during deserialization");
          }

          T *instance = dynamic_cast<T *>(instance_ptr.get());
          if (!instance)
          {
            throw std::runtime_error("Type mismatch during deserialization");
          }

          instance->deserialize(des);

          return *instance;
        }
        catch (const std::exception &e)
        {
          // 处理异常
          std::cerr << "Deserialization error: " << e.what() << std::endl;
          throw;
        }
      }
    } // namespace sery
  } // namespace data
} // namespace codnot
