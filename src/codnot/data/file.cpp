#include <codnot/data/file.hpp>
#include <filesystem>
#include <fstream>
namespace codnot {
namespace data {
t_file::t_file(const std::string &path) : path_(path) {}

t_file::t_file(const fs::path &path) : path_(path) {}

t_file::t_file(t_file &&file) : path_(std::move(file.path_)) {}

bool t_file::exists() const { return fs::exists(path_); }

bool t_file::is_file() { return fs::is_regular_file(path_); }

void t_file::create_file_not_exits() {
  std::filesystem::path dir = path_.parent_path();
  if (!dir.empty() && !std::filesystem::exists(dir)) {
    if (!std::filesystem::create_directories(dir)) {
      throw std::runtime_error("Failed to create directory: " + dir.string());
    }
  }
  if (!exists()) {
    std::ofstream ofs(path_);
    if (!ofs) {
      throw std::runtime_error("Failed to create file: " + path_.string());
    }
    ofs.close();
  }
}

t_file &t_file::open_file(std::ios::openmode mode) {
  create_file_not_exits();
  fs_.open(path_, mode);
  if (!fs_) {
    throw std::runtime_error("Failed to open file: " + path_.string());
  }
  mode_ = mode;
  return *this;
}

t_file &t_file::write(const std::string &data) {
  if (!fs_.is_open() || !(mode_ & std::ios::out)) {
    throw std::runtime_error("File is not open for writing: " + path_.string());
  }
  if (is_binary_mode()) {
    fs_.write(data.data(), data.size());
  } else {
    fs_ << data;
  }
  return *this;
}

std::string t_file::read() {
  if (!fs_.is_open() || !(mode_ & std::ios::in)) {
    throw std::runtime_error("File is not open for reading: " + path_.string());
  }
  std::string content;
  if (is_binary_mode()) {
    fs_.seekg(0, std::ios::end);
    size_t size = fs_.tellg();
    fs_.seekg(0, std::ios::beg);
    content.resize(size);
    fs_.read(&content[0], size);
  } else {
    content.assign((std::istreambuf_iterator<char>(fs_)),
                   std::istreambuf_iterator<char>());
  }
  return content;
}

bool t_file::is_binary_mode() const { return mode_ & std::ios::binary; }

void t_file::close_file() {
  if (fs_.is_open()) {
    fs_.close();
  }
}

t_file::~t_file() { close_file(); }
} // namespace data

} // namespace codnot