/**
 * @file ebt.hpp
 * @brief
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-02
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <codnot/data/file.hpp>
#include <codnot/data/sery/sery.hpp>
#include <variant>
namespace codnot {
namespace data {
/// @brief EBT数据类型空间
namespace ebt {

namespace sery = codnot::data::sery;
/// @brief EBT数据对象
class _CODNOT_DECLARE_ t_ebt;
typedef t_ebt EBT;
typedef t_ebt ebt;
/// @brief 可序列化接口
using it_serializable = data::sery::it_serializable;
/// @brief ebt工厂函数
using ebt_factory = std::function<std::unique_ptr<t_ebt>()>;
/// @brief 文件类型
using file = codnot::data::t_file;
/// @brief ebt魔数
constexpr uint32_t EBT_MAGIC_NUMBER =
    ('E' << 24) | ('B' << 16) | ('T' << 8) | '!';
/// @brief ebt版本
constexpr uint32_t EBT_VERSION = 1;
/**
 * @brief ebt文件头结构
 *
 */
struct st_ebt_header {
  uint32_t magic_number_;
  uint32_t version_;
  uint32_t size_;
};
/**
 * @brief 序列化ebt
 *
 * @param ebt
 * @return std::string
 */
std::string _CODNOT_DECLARE_ ebt_serialize(t_ebt &ebt);
/**
 * @brief 反序列化ebt
 *
 * @param str
 * @return t_ebt
 */
t_ebt _CODNOT_DECLARE_ ebt_deserialize(std::string str);
/**
 * @brief 允许的vector类型
 *
 * @tparam T
 */
template <typename T>
concept allowed_vector_types =
    std::is_same_v<T, int> || std::is_same_v<T, std::string> ||
    std::is_same_v<T, double> || std::is_same_v<T, bool> ||
    std::is_same_v<T, const char *> || std::is_same_v<T, float> ||
    std::is_same_v<T, it_serializable>;

/**
 * @brief 允许写入t_ebt对象的数据类型，包括int, string, double, bool, t_ebt
 * @todo 添加对写入数组值的支持（std::vector）；需要对ebt数据头进行优化，涉及字节流的问题。@ref data::byte_stream
 * @tparam T
 */
template <typename T>
concept allowed_types =
    std::is_same_v<T, int> || std::is_same_v<T, std::string> ||
    std::is_same_v<T, double> || std::is_same_v<T, bool> ||
    std::is_same_v<T, t_ebt> || std::is_same_v<T, const char *> ||
    std::is_same_v<T, float> || allowed_vector_types<T>;
    
/**
 * @brief ebt数据类型
 * @details
 * 因为ebt支持数据类型不会很多，所以用8位无符号整数标识类型来序列化和反序列化。
 */
class t_ebt : public it_serializable {

  using val_type = std::variant<int, std::string, double, bool, t_ebt>;

  std::unordered_map<std::string, val_type> ebt_map_;

public:
  t_ebt() = default;
  t_ebt(file &file);
  void serialize(sery::t_serializer &ts) override;
  /**
   * @brief t_ebt反序列化
   * @details 先读取键
   *
   * @param ds
   */
  void deserialize(sery::t_deserializer &ds) override;
  /**
   * @brief 添加键值对
   *
   * @tparam T
   * @param key
   * @param value
   */
  template <allowed_types T> void add(const std::string &key, T value) {
    if (ebt_map_.find(key) != ebt_map_.end()) {
      throw std::runtime_error("Key already exists");
    }
    ebt_map_[key] = value;
  }

  /**
   * @brief 设置键值对
   *
   * @tparam T
   * @param key
   * @param value
   */
  template <allowed_types T> void set(const std::string &key, T value) {
    ebt_map_[key] = value;
  }
  /**
   * @brief 移除键值对
   *
   * @param key
   */
  void remove(const std::string &key);
  /**
   * @brief 获取键值对
   *
   * @tparam T
   * @param key
   * @return T
   */
  template <allowed_types T> T get(const std::string &key) const {
    auto it = ebt_map_.find(key);
    if (it != ebt_map_.end()) {
      if (std::holds_alternative<T>(it->second)) {
        return std::get<T>(it->second);
      } else {
        throw std::runtime_error("Type mismatch for key: " + key);
      }
    } else {
      throw std::runtime_error("Key not found");
    }
  }

  /**
   * @brief 判断是否存在键
   *
   * @param key
   * @return true
   * @return false
   */
  bool has_key(const std::string &key) const;
  /**
   * @brief 保存到文件
   *
   * @param file
   */
  void save_to_file(file &file);
  ~t_ebt() = default;
};
REG_SERY_CLASS(t_ebt)
} // namespace ebt

} // namespace data

} // namespace codnot
