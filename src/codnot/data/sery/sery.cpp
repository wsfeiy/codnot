#include <codnot/data/sery/sery.hpp>

namespace codnot {
namespace data {
namespace sery {
void t_serializer::write_int(int value) {
  static_assert(sizeof(int) == 4, "int size is not 4 bytes");
  t_l_size val = *reinterpret_cast<t_l_size *>(&value);
  buffer_.write_l_size(val);
}

void t_serializer::write_bool(bool value) {
  static_assert(sizeof(bool) == 1, "bool size is not 1 byte");
  t_s_size val = *reinterpret_cast<t_s_size *>(&value);
  buffer_.write_s_size(val);
}

void t_serializer::write_double(double value) {
  static_assert(sizeof(double) == 8, "double size is not 8 bytes");
  t_xl_size val = *reinterpret_cast<t_xl_size *>(&value);
  buffer_.write_xl_size(val);
}

void t_serializer::write_float(float value) {
  static_assert(sizeof(float) == 4, "float size is not 4 bytes");
  t_l_size val = *reinterpret_cast<t_l_size *>(&value);
  buffer_.write_l_size(val);
}

void t_serializer::write_string(std::string val) {
  t_l_size size = val.size();
  write_int(size);
  std::vector<t_byte> bytes(val.begin(), val.end());
  buffer_.write_size(bytes);
}
std::string t_serializer::to_string() {
  const std::vector<t_byte> &buf = buffer_.get_buf();
  return std::string(reinterpret_cast<const char *>(buf.data()), buf.size());
}
double t_deserializer::read_double() {
  t_xl_size val = buffer_.read_xl_size();
  return *reinterpret_cast<double *>(&val);
}

float t_deserializer::read_float() {
  t_l_size val = buffer_.read_l_size();
  return *reinterpret_cast<float *>(&val);
}

int t_deserializer::read_int() {
  t_l_size val = buffer_.read_l_size();
  return *reinterpret_cast<int *>((&val));
}

bool t_deserializer::read_bool() {
  t_s_size val = buffer_.read_s_size();
  return *reinterpret_cast<bool *>(&val);
}

std::string t_deserializer::read_string() {
  t_xl_size size = read_int();
  std::vector<t_byte> bytes = buffer_.read_size(size);
  return std::string(reinterpret_cast<const char *>(bytes.data()), size);
}

t_m_size
t_serializable_class_registry::get_id_by_name(const std::string &name) {
  auto it = map_.find(name);
  if (it != map_.end()) {
    return it->second.id;
  }
  throw std::runtime_error("unknown class name");
}

const st_union_class_id &
t_serializable_class_registry::get_union_class_id(t_m_size id) {
  for (const auto &[name, st] : map_) {
    if (st.id == id) {
      return st;
    }
  }
  throw std::runtime_error("unknown class id");
}

t_m_size t_serializable_class_registry::allocate() {
  t_m_size id = id_bit_map_.allocate();
  if (id == t_m_size_max) {
    throw std::runtime_error("id_bit_map_ is full");
  }
  return id;
}

void t_serializable_class_registry::reg_class(std::string name,
                                              factory_func func) {
  map_[name] = st_union_class_id{
      t_serializable_class_registry::get_instance().allocate(), name, func};
}

std::unique_ptr<it_serializable>
t_serializable_class_registry::create_instance(t_m_size id) {
  const st_union_class_id &st = get_union_class_id(id);
  return st.func_();
}

t_serializable_class_registry &t_serializable_class_registry::get_instance() {
  static t_serializable_class_registry instance;
  return instance;
}

} // namespace sery

} // namespace data

} // namespace codnot
