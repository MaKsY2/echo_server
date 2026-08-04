#pragma once

#include "file_descriptor.hpp"
#include <cstdint>
#include <span>
#include <sys/epoll.h>
#include <unistd.h>
namespace echo {
class Epoll {
  FileDescriptor fd_;

public:
  Epoll();

  void add(int, std::uint32_t);
  void remove(int);
  std::span<const epoll_event> wait(std::span<epoll_event>, int);
};
}; // namespace echo