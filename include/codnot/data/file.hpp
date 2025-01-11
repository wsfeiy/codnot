/**
 * @file file.hpp
 * @brief 文件操作类
 * @details
 * @mainpage
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-01
 * @copyright © 2024 codnot
 */
#pragma once
#include <codnot/data/data.hpp>
#include <filesystem>
#include <fstream>
namespace codnot {
namespace data {

namespace fs = std::filesystem;

class _CODNOT_DECLARE_ t_file;

/**
 * @class t_file
 * @brief 表示一个文件，并提供基本的文件操作。
 */
class t_file {
public:
  /**
   * @brief 使用给定的文件路径构造一个 t_file 对象。
   * @param path 文件路径，以字符串形式表示。
   */
  t_file(const std::string &path);

  /**
   * @brief 使用给定的文件路径构造一个 t_file 对象。
   * @param path 文件路径，以 std::filesystem::path 对象形式表示。
   */
  t_file(const fs::path &path);

  /**
   * @brief 删除 t_file 的拷贝构造函数。
   * @param file 要拷贝的 t_file 对象。
   */
  t_file(const t_file &file) = delete;

  /**
   * @brief t_file 的移动构造函数。
   * @param file 要移动的 t_file 对象。
   */
  t_file(t_file &&file);

  /**
   * @brief 检查文件是否存在。
   * @return 如果文件存在，返回 true；否则返回 false。
   */
  bool exists() const;

  /**
   * @brief 检查路径是否为常规文件。
   * @return 如果路径是常规文件，返回 true；否则返回 false。
   */
  bool is_file();

  /**
   * @brief 如果文件不存在，则创建文件。
   * @throws std::runtime_error 如果文件或目录创建失败。
   */
  void create_file_not_exits();

  /**
   * @brief 使用指定模式打开文件。
   * @param mode 文件打开模式（例如，std::ios::in, std::ios::out）。
   * @return 当前 t_file 对象的引用。
   * @throws std::runtime_error 如果文件无法打开。
   */
  t_file &open_file(std::ios::openmode mode);

  /**
   * @brief 将数据写入文件。
   * @param data 要写入文件的数据。
   * @return 当前 t_file 对象的引用。
   * @throws std::runtime_error 如果文件未打开用于写入。
   */
  t_file &write(const std::string &data);

  /**
   * @brief 从文件中读取数据。
   * @return 文件内容的字符串。
   * @throws std::runtime_error 如果文件未打开用于读取。
   */
  std::string read();

  /**
   * @brief 检查文件是否以二进制模式打开。
   * @return 如果文件以二进制模式打开，返回 true；否则返回 false。
   */
  bool is_binary_mode() const;

  /**
   * @brief 关闭文件。
   */
  void close_file();

  ~t_file();

private:
  fs::path path_;           ///< 文件路径。
  std::fstream fs_;         ///< 文件流。
  std::ios::openmode mode_; ///< 文件打开模式。
};
} // namespace data

} // namespace codnot
