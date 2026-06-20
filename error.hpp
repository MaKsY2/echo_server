#pragma once

#include <cerrno>
#include <system_error>

[[noreturn]] inline void throw_errno(const char *what) {
  throw std::system_error(errno, std::generic_category(), what);
}