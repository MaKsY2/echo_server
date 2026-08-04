#include "epoll.hpp"
#include "file_descriptor.hpp"
#include <array>
#include <cstring>
#include <gtest/gtest.h>
#include <sys/epoll.h>
#include <unistd.h>

TEST(EpollTest, ReadinessPersistsUntilPipeIsDrained) {
  int pipe_fd[2];
  int pipe_res = ::pipe(pipe_fd);
  ASSERT_EQ(pipe_res, 0);
  echo::FileDescriptor read_fd(pipe_fd[0]);
  echo::FileDescriptor write_fd(pipe_fd[1]);
  echo::Epoll epoll;
  std::array<epoll_event, 4> buffer{};
  epoll.add(read_fd.get(), EPOLLIN);

  auto res = epoll.wait(buffer, 0);
  EXPECT_EQ(res.size(), 0);
  const char *sent = "AB";
  const auto written = ::write(write_fd.get(), sent, strlen(sent));
  ASSERT_EQ(written, strlen(sent));

  res = epoll.wait(buffer, 0);
  ASSERT_EQ(res.size(), 1);
  EXPECT_EQ(res[0].data.fd, read_fd.get());
  EXPECT_NE(res[0].events & EPOLLIN, 0U);

  res = epoll.wait(buffer, 0);
  EXPECT_EQ(res.size(), 1);

  std::array<char, 3> received{};
  auto read_bytes = ::read(read_fd.get(), received.data(), strlen(sent));
  ASSERT_EQ(read_bytes, strlen(sent));
  EXPECT_STREQ(received.data(), sent);

  res = epoll.wait(buffer, 0);
  EXPECT_EQ(res.size(), 0);
}