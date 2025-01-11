/**
 * @file buffer.hpp
 * @brief 缓冲模板
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-01
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <codnot/data/stream/stream.hpp>
#include <stdexcept>
#include <vector>

namespace codnot {
namespace data {
namespace stream {

template <typename T> class t_base_buffer;

template <typename T> class t_base_buffer {
protected:
  std::vector<T> buffer_;
  uint64_t offset_;
  t_xl_size size_ = 1024;

public:
  t_base_buffer() { init(); };

  t_base_buffer(std::vector<T> stream) {
    buffer_ = stream;
    init();
  }

  t_base_buffer(t_xl_size size) : size_(size) { init(); }
  /**
   * @brief 初始化
   *
   */
  void init() {
    offset_ = 0;
    buffer_.reserve(size_);
  }
  /**
   * @brief 获取偏移量
   *
   * @return uint64_t 偏移量
   */
  uint64_t offset() { return offset_; }
  /**
   * @brief 设置偏移量
   *
   * @param uint64_t offset
   */
  void offset(uint64_t offset) { offset_ = offset; }
  /**
   * @brief 顺序写入一个数据
   *
   * @param const T &data_
   */
  void write(const T data_) { buffer_.push_back(data_); }
  /**
   * @brief 顺序写入多个数据
   *
   * @param data
   */
  void write_size(std::vector<T> &data) {
    buffer_.insert(buffer_.end(), data.begin(), data.end());
  }
  /**
   * @brief 顺序读取一个数据
   *
   * @return T
   */
  T read() { return buffer_[offset_++]; }
  /**
   * @brief 读取指定位置
   *
   * @param uint64_t index
   * @return T or 0
   */
  T read(uint64_t index) { return buffer_[index]; }
  /**
   * @brief 读取多个数据
   *
   * @param uint64_t size
   * @return std::vector<T> 读取的数据
   */
  std::vector<T> read_size(uint64_t size) {
    std::vector<T> data;
    for (uint64_t i = 0; i < size; i++) {
      data.push_back(buffer_[offset_++]);
    }
    return data;
  }
  /**
   * @brief 获取流
   *
   * @return std::vector<T>
   */
  std::vector<T> &get_buf() { return buffer_; }
  /**
   * @brief 获取流大小
   *
   * @return uint64_t
   */
  uint64_t size() { return buffer_.size(); }
  /**
   * @brief 清空流
   *
   */
  void clear() { buffer_.clear(); }
  virtual ~t_base_buffer() {};
};
} // namespace stream
} // namespace data
} // namespace codnot
