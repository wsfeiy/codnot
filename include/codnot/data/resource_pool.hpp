/**
 * @file pool.hpp
 * @brief 资源池模板
 * @details
 * @author 飞也
 * @version v1.0.0
 * @date 2025-01-01
 * @copyright © 2025 codnot. All rights reserved.
 */
#pragma once
#include <codnot/codnot_export.h>
#include <codnot/data/data.hpp>
#include <mutex>
#include <queue>
#include <vector>

namespace codnot {
namespace data {

template <typename ResourceType> class _CODNOT_DECLARE_ t_base_resource_pool;

template <typename ResourceType> class _CODNOT_DECLARE_ t_shared_resource_pool;

template <typename ResourceType> class _CODNOT_DECLARE_ t_unique_resource_pool;

template <typename ResourceType> class t_ref_counter;

/**
 * @brief 引用计数器模板
 *
 */
template <typename ResourceType> class t_ref_counter {
  /// @brief 引用计数
  t_m_size ref_count_ = 0;
  ResourceType *resource_ptr_ = nullptr;

public:
  t_ref_counter(ResourceType *resource_ptr)
      : ref_count_(0), resource_ptr_(resource_ptr) {}
  /**
   * @brief 增加计数
   *
   */
  void add_ref() { ref_count_++; }
  /**
   * @brief 减少计数
   *
   */
  void cut_ref() {
    if (ref_count_ > 0) {
      ref_count_--;
    }
  }
};

/**
 * @brief 基本资源池模板
 * @details 用于管理各种资源，包括添加、获取、释放等操作；能够被多个线程持有资源
 * @tparam ResourceType
 */
template <typename ResourceType> class t_base_resource_pool {
protected:
  /// @brief 互斥锁
  std::mutex mutex_;
  /// @brief 资源
  std::vector<ResourceType> pool_;
  t_base_resource_pool() = default;

public:
  /// @brief 禁止拷贝
  t_base_resource_pool(const t_base_resource_pool &) = delete;
  /// @brief 禁止赋值
  t_base_resource_pool &operator=(const t_base_resource_pool &) = delete;
  /// @brief 支持移动语义
  t_base_resource_pool(t_base_resource_pool &&other) = default;
  /**
   * @brief 添加资源
   *
   * @param resource
   */
  virtual void add_resource(ResourceType &&resource) = 0;
  /**
   * @brief 移除资源
   *
   * @param resource
   */
  virtual void remove_resource(ResourceType &resource) = 0;
  /**
   * @brief 释放资源
   *
   * @param resource
   */
  virtual void release_resource(ResourceType &resource) = 0;
  /**
   * @brief 获取资源
   *
   * @return ResourceType&
   */
  virtual ResourceType &acquire_resource() = 0;
  /**
   * @brief 获取资源数量
   *
   * @return t_xl_size
   */
  virtual t_xl_size size() = 0;
};
/**
 * @brief 共享资源池模板
 * @details 使用引用计数，资源能够被多个使用者持有。适用于常量资源。
 * @tparam ResourceType
 * @todo 共享资源池逻辑待完善
 */
template <typename ResourceType>
class t_shared_resource_pool : public t_base_resource_pool<ResourceType> {
  /// @brief 资源引用计数器
  using ref_counter = t_ref_counter<ResourceType>;
  /// @brief 资源引用计数器容器
  std::vector<ref_counter> ref_counters_;

public:
  void release_resource(ResourceType &resource) override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    for (auto &counter : ref_counters_) {
      if (counter.resource_ptr_ == &resource) {
        counter.cut_ref();
        if (counter.ref_count_ == 0) {
          // 如果引用计数为0，将资源放回资源池
          this->pool_.push_back(std::move(*counter.resource_ptr_));
          // 删除引用计数器
          counter.resource_ptr_ = nullptr;
        }
        return;
      }
    }
    throw std::runtime_error("Resource not found in the pool");
  }

  ResourceType &acquire_resource() override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    if (this->pool_.empty()) {
      throw std::runtime_error("Resource pool is empty");
    }
    ResourceType resource = std::move(this->pool_.back());
    this->pool_.pop_back();
    ref_counters_.emplace_back(new ResourceType(resource));
    ref_counters_.back().add_ref();
    return *ref_counters_.back().resource_ptr_;
  }

  void add_resource(ResourceType &&resource) override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->pool_.push_back(std::move(resource));
  }

  void remove_resource(ResourceType &resource) override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    for (auto it = ref_counters_.begin(); it != ref_counters_.end(); ++it) {
      if (it->resource_ptr_ == &resource) {
        if (it->ref_count_ > 0) {
          throw std::runtime_error(
              "Cannot remove resource with active references");
        }
        ref_counters_.erase(it);
        return;
      }
    }
    throw std::runtime_error("Resource not found in the pool");
  }

  t_xl_size size() override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->pool_.size();
  }
};

/**
 * @brief 独占资源池模板
 * @details 每个资源在任何时候只能有一个使用者持有，适用于可变资源
 * @tparam ResourceType
 */
template <typename ResourceType>
class t_unique_resource_pool : public t_base_resource_pool<ResourceType> {
  /// @brief 空闲资源指针
  std::queue<ResourceType *> free_ptrs_;

public:
  ResourceType &acquire_resource() override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    if (free_ptrs_.empty()) {
      throw std::runtime_error("Resource pool is empty");
    }
    ResourceType *resource_ptr = free_ptrs_.front();
    free_ptrs_.pop();
    return *resource_ptr;
  }

  void release_resource(ResourceType &resource) override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    free_ptrs_.push(&resource);
  }

  void add_resource(ResourceType &&resource) override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->pool_.push_back(std::move(resource));
    free_ptrs_.push(&this->pool_.back());
  }

  void remove_resource(ResourceType &resource) override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    for (auto it = this->pool_.begin(); it != this->pool_.end(); ++it) {
      if (&(*it) == &resource) {
        if (!free_ptrs_.empty() && free_ptrs_.front() == &(*it)) {
          free_ptrs_.pop();
        }
        this->pool_.erase(it);
        return;
      }
    }
    throw std::runtime_error("Resource not found in the pool");
  }

  t_xl_size size() override {
    std::lock_guard<std::mutex> lock(this->mutex_);
    return this->pool_.size();
  }
};

} // namespace data
} // namespace codnot