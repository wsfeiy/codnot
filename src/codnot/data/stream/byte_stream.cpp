#include <codnot/data/stream/byte_stream.hpp>
namespace codnot {
namespace data {
namespace stream {
bool is_little_endian() {
  uint16_t test = 0x0001;
  return *(reinterpret_cast<uint8_t *>(&test)) == 0x01;
}
void swap_endian(std::vector<t_byte> &bytes) {
  std::size_t n = bytes.size();
  for (size_t i = 0; i < n / 2; i++) {
    std::swap(bytes[i], bytes[n - i - 1]);
  }
}

t_byte_stream::t_byte_stream(std::string str) {
  std::vector<t_byte> byts(str.begin(), str.end());
  write_size(byts);
}

void t_byte_stream::write_s_size(t_s_size val) { write(val); }

void t_byte_stream::write_m_size(t_m_size val) {
  std::vector<t_byte> bytes(2);
  std::memcpy(bytes.data(), &val, 2);
  write_bytes(bytes);
}

void t_byte_stream::write_l_size(t_l_size val) {
  std::vector<t_byte> bytes(4);
  std::memcpy(bytes.data(), &val, 4);
  write_bytes(bytes);
}

void t_byte_stream::write_xl_size(t_xl_size val) {
  std::vector<t_byte> bytes(8);
  std::memcpy(bytes.data(), &val, 8);
  write_bytes(bytes);
}

t_s_size t_byte_stream::read_s_size() { return static_cast<t_s_size>(read()); }

t_m_size t_byte_stream::read_m_size() {
  std::vector<t_byte> bytes = read_bytes(2);
  t_m_size val;
  std::memcpy(&val, bytes.data(), 2);
  return val;
}

t_l_size t_byte_stream::read_l_size() {
  std::vector<t_byte> bytes = read_bytes(4);
  t_l_size val;
  std::memcpy(&val, bytes.data(), 4);
  return val;
}

t_xl_size t_byte_stream::read_xl_size() {
  std::vector<t_byte> bytes = read_bytes(8);
  t_xl_size val;
  std::memcpy(&val, bytes.data(), 8);
  return val;
}

void t_byte_stream::write_bytes(std::vector<t_byte> &bytes) {
  if (is_little_endian_ && bytes.size() > 1) {
    swap_endian(bytes);
  }
  write_size(bytes);
}

std::vector<t_byte> t_byte_stream::read_bytes(t_xl_size size) {
  std::vector<t_byte> bytes = read_size(size);
  if (is_little_endian_ && size > 1) {
    swap_endian(bytes);
  }
  return bytes;
}
} // namespace stream

} // namespace data

} // namespace codnot
