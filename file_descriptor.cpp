#include "file_descriptor.hpp"

#include <utility>

namespace epoll {
FileDescriptor::FileDescriptor(int fd) noexcept : fd_(fd) {}

FileDescriptor::~FileDescriptor() {
  if (fd_ >= 0) {
    ::close(fd_);
  }
}
FileDescriptor::FileDescriptor(FileDescriptor &&other) noexcept
    : fd_(std::exchange(other.fd_, -1)) {}

FileDescriptor &FileDescriptor::operator=(FileDescriptor &&other) noexcept {
  if (this != &other) {
    if (fd_ >= 0)
      ::close(fd_);
    fd_ = std::exchange(other.fd_, -1);
  }
  return *this;
}

[[nodiscard]] int FileDescriptor::get() const noexcept { return fd_; }
[[nodiscard]] bool FileDescriptor::valid() const noexcept { return fd_ >= 0; }

} // namespace epoll