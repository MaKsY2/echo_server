#pragma once

#include <unistd.h>

namespace epoll {
class FileDescriptor {
  int fd_{-1};

public:
  FileDescriptor() = default;
  explicit FileDescriptor(int fd) noexcept;
  ~FileDescriptor();

  FileDescriptor(const FileDescriptor &) = delete;
  FileDescriptor &operator=(const FileDescriptor &) = delete;

  FileDescriptor(FileDescriptor &&other) noexcept;
  FileDescriptor &operator=(FileDescriptor &&other) noexcept;
  [[nodiscard]] int get() const noexcept;
  [[nodiscard]] bool valid() const noexcept;
};
} // namespace epoll
