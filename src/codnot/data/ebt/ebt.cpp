#include <codnot/data/ebt/ebt.hpp>

namespace codnot {
namespace data {
namespace ebt {
std::string ebt_serialize(t_ebt &ebt) {
  uint16_t id =
      sery::t_serializable_class_registry::get_instance().get_id_by_name(
          typeid(t_ebt).name());
  sery::t_serializer ts;
  ts.write_int(id);
  ebt.serialize(ts);
  return ts.to_string();
}
t_ebt ebt_deserialize(std::string str) {
  sery::t_deserializer ds(str);
  t_ebt ebt = sery::deserialize<t_ebt>(ds);
  return ebt;
}

t_ebt::t_ebt(file &file) {
  file.open_file(std::ios::in | std::ios::out | std::ios::binary);
  std::string sr = file.read();
  codnot::data::sery::t_deserializer ds(sr);
  st_ebt_header ebt_header = {static_cast<uint32_t>(ds.read_int()),
                              static_cast<uint32_t>(ds.read_int()),
                              static_cast<uint32_t>(ds.read_int())};
  if (ebt_header.magic_number_ != EBT_MAGIC_NUMBER) {
    throw std::runtime_error("Invalid ebt file");
  }
  std::string sri = ds.read_string();
  *this = ebt_deserialize(sri);
}
void t_ebt::remove(const std::string &key) {
  auto it = ebt_map_.find(key);
  if (it != ebt_map_.end()) {
    ebt_map_.erase(it);
  } else {
    throw std::runtime_error("Key not found");
  }
}
bool t_ebt::has_key(const std::string &key) const {
  return ebt_map_.find(key) != ebt_map_.end();
}
void t_ebt::save_to_file(file &file) {
  sery::t_serializer ts;
  std::string sr = ebt_serialize(*this);
  st_ebt_header ebt_header = {EBT_MAGIC_NUMBER, EBT_VERSION,
                              static_cast<uint32_t>(sr.size())};
  ts.write_int(ebt_header.magic_number_);
  ts.write_int(ebt_header.version_);
  ts.write_int(ebt_header.size_);
  ts.write_string(sr);
  file.open_file(std::ios::in | std::ios::out | std::ios::binary)
      .write(ts.to_string())
      .close_file();
}
void t_ebt::serialize(sery::t_serializer &ts) {
  ts.write_int(ebt_map_.size());
  for (auto &[key, val] : ebt_map_) {
    ts.write_string(key);
    std::visit(
        [&ts](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, t_ebt>) {
            ts.buffer_.write_s_size(0);
            arg.serialize(ts);
          } else if constexpr (std::is_same_v<T, bool>) {
            ts.buffer_.write_s_size(1);
            ts.write_bool(arg);
          } else if constexpr (std::is_same_v<T, int>) {
            ts.buffer_.write_s_size(2);
            ts.write_int(arg);
          } else if constexpr (std::is_same_v<T, double>) {
            ts.buffer_.write_s_size(3);
            ts.write_double(arg);
          } else if constexpr (std::is_same_v<T, std::string>) {
            ts.buffer_.write_s_size(4);
            ts.write_string(arg);
          } else if constexpr (std::is_same_v<T, std::wstring>) {
            ts.buffer_.write_s_size(5);
            ts.write_string(arg);
          } else if constexpr (std::is_same_v<T, std::vector<std::any>>) {
            if (arg.empty()) {
              ts.buffer_.write_s_size(6); // 假设 6 表示 vector<std::any>
              ts.write_int(0);            // 向量大小为 0
            } else {
              std::visit(
                  [&ts](auto &&first_element) {
                    using subt = std::decay_t<decltype(first_element)>;
                    if constexpr (std::is_same_v<subt, t_ebt>) {
                      ts.buffer_.write_s_size(6);

                    } else if constexpr (std::is_same_v<subt, bool>) {
                      ts.buffer_.write_s_size(7);

                    } else if constexpr (std::is_same_v<subt, int>) {
                      ts.buffer_.write_s_size(8);

                    } else if constexpr (std::is_same_v<subt, double>) {
                      ts.buffer_.write_s_size(9);

                    } else if constexpr (std::is_same_v<subt, std::string>) {
                      ts.buffer_.write_s_size(10);
                      
                    } else {
                      throw std::runtime_error("Unsupported type in vector");
                    }
                  },
                  arg[0]);
            }
          } else {
            throw std::runtime_error("Unsupported type in ebt");
          }
        },
        val);
  }
}
void t_ebt::deserialize(sery::t_deserializer &ds) {
  ebt_map_.clear();
  size_t size = ds.read_int();
  for (size_t i = 0; i < size; ++i) {
    std::string key = ds.read_string(); // 反序列化键

    int type_id = ds.buffer_.read_s_size(); // 反序列化类型标识符

    // 根据类型标识符进行反序列化
    switch (type_id) {
    case 0: {
      t_ebt ebt;
      ebt.deserialize(ds);
      ebt_map_[key] = ebt;
      break;
    }
    case 1: {
      ebt_map_[key] = ds.read_bool();
      break;
    }
    case 2: {
      ebt_map_[key] = ds.read_int();
      break;
    }
    case 3: {
      ebt_map_[key] = ds.read_double();
      break;
    }
    case 4: {
      ebt_map_[key] = ds.read_string();
      break;
    }
    case 5: {
      ebt_map_[key] = ds.read_string();
      break;
    }
    default:
      throw std::runtime_error("Unknown type identifier");
    }
  }
}

} // namespace ebt
} // namespace data
} // namespace codnot
