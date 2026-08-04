#include "epoll.hpp"

#include "error.hpp"
#include "file_descriptor.hpp"

#include <cerrno>
#include <limits>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <system_error>
#include <unistd.h>

namespace echo {
Epoll::Epoll() {
  auto opt_fd = epoll_create1(0);
  if (opt_fd < 0) {
    common::throw_errno("failed to create epoll fd");
  }

  fd_ = FileDescriptor(opt_fd);
}

void Epoll::remove(int target_fd) {
  const auto res = epoll_ctl(fd_.get(), EPOLL_CTL_DEL, target_fd, nullptr);
  if (res < 0) {
    const auto err = errno;
    throw std::system_error(err, std::generic_category(),
                            "epoll_ctl(DEL), target_fd=" +
                                std::to_string(target_fd));
  }
}

std::span<const epoll_event> Epoll::wait(std::span<epoll_event> events,
                                         int timeout_ms) {
  if (!events.size()) {
    throw std::invalid_argument("events size must be at least one");
  }
  if (events.size() > std::numeric_limits<int>::max()) {
    throw std::length_error("events size must be at most INT_MAX");
  }
  while (true) {

    const auto cnt = epoll_wait(fd_.get(), events.data(),
                                static_cast<int>(events.size()), timeout_ms);

    if (cnt < 0) {
      if (errno == EINTR) {
        continue;
      }
      const auto err = errno;
      throw std::system_error(err, std::generic_category(), "epoll_wait");
    }
    return events.first(cnt);
  }
}

void Epoll::add(int target_fd, std::uint32_t mask) {
  epoll_event event{
      .events = mask,
      .data =
          epoll_data{
              .fd = target_fd,
          },
  };

  const auto res = epoll_ctl(fd_.get(), EPOLL_CTL_ADD, target_fd, &event);
  if (res < 0) {
    const auto err = errno;
    throw std::system_error(err, std::generic_category(),
                            "epoll_ctl(ADD), target_fd=" +
                                std::to_string(target_fd));
  }
}
} // namespace echo
