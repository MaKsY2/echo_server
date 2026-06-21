#include "test_utils.hpp"
#include <gtest/gtest.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

using namespace std::chrono_literals;
using namespace epoll;
using namespace tests;

TEST_F(ServerFixture, EchoesShortMessage) {
  TestClient c(port_);
  c.send("hello");
  EXPECT_EQ(c.recv_exact(5), "hello");
}

TEST_F(ServerFixture, EchoesMultipleMessages) {
  TestClient c(port_);
  c.send("first");
  EXPECT_EQ(c.recv_exact(5), "first");
  c.send("second");
  EXPECT_EQ(c.recv_exact(6), "second");
}

TEST_F(ServerFixture, EchoesLargePayload) {
  TestClient c(port_);
  std::string big(64 * 1024, 'A');
  c.send(big);
  EXPECT_EQ(c.recv_exact(big.size()), big);
}

TEST_F(ServerFixture, HandlesClientDisconnect) {
  {
    TestClient c(port_);
    c.send("bye");
    EXPECT_EQ(c.recv_exact(3), "bye");
    c.close();
  }
  std::this_thread::sleep_for(50ms);
  TestClient c2(port_);
  c2.send("alive");
  EXPECT_EQ(c2.recv_exact(5), "alive");
}

TEST_F(ServerFixture, HandlesTwoClientsConcurrently) {
  TestClient c1(port_);
  TestClient c2(port_);

  c1.send("aaa");
  c2.send("bbb");

  EXPECT_EQ(c1.recv_exact(3), "aaa");
  EXPECT_EQ(c2.recv_exact(3), "bbb");
}
