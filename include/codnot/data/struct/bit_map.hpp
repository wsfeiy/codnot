/**
 * @file bit_map.hpp
 * @brief 位图
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-01
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <bitset>
#include <codnot/data/data.hpp>

namespace codnot {
namespace data {
/// @brief 数据结构空间
namespace structure {
/// @brief 尺寸类型
using t_xl_size = codnot::data::t_xl_size;
/**
 * @class t_bit_map
 * @brief 固定大小的位图类
 * @details 提供了一个固定大小的位图类，用于管理位的分配和释放。
 * @tparam N 位图的大小
 */
template <t_xl_size N> class t_bit_map {
public:
  /**
   * @brief 默认构造函数
   * @details 初始化位图，所有位都设置为 0。
   */
  t_bit_map() = default;

  /**
   * @brief 删除拷贝构造函数
   * @details 禁止拷贝构造函数，确保位图对象不可拷贝。
   */
  t_bit_map(const t_bit_map &) = delete;

  /**
   * @brief 删除赋值运算符
   * @details 禁止赋值运算符，确保位图对象不可赋值。
   */
  t_bit_map &operator=(const t_bit_map &) = delete;

  /**
   * @brief 检查位图是否已满
   * @details 检查位图中的所有位是否都已设置为 1。
   * @return 如果所有位都已设置为 1，返回 true；否则返回 false。
   */
  bool out_of_range() { return m_bit_map_.all(); }

  /**
   * @brief 获取位图的大小
   * @details 返回位图的大小，即位图中位的数量。
   * @return 位图的大小。
   */
  t_xl_size size() { return N; }

  /**
   * @brief 释放指定索引的位
   * @details 将指定索引的位设置为 0。
   * @param index 要释放的位的索引。
   */
  void free(t_xl_size index) { m_bit_map_[index] = 0; }

  /**
   * @brief 分配一个空闲的位
   * @details 查找并分配一个空闲的位（即值为 0 的位），并将其设置为 1。
   * @return 分配到的位的索引。
   * @throw std::out_of_range 如果没有空闲的位，抛出此异常。
   */
  t_xl_size allocate() {
    for (t_xl_size i = 0; i < N; i++) {
      if (!m_bit_map_[i]) {
        m_bit_map_[i] = true;
        return i;
      }
    }
    throw std::out_of_range("No free bit");
  }

private:
  std::bitset<N> m_bit_map_; ///< 存储位的位图
};
} // namespace structure

} // namespace data

} // namespace codnot
