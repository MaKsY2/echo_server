#include <string>
namespace common {
enum class ServerType { Threaded, Epoll, Uring, Unknown };

inline ServerType ParseServerType(const std::string &type) {
  if (type == "threaded")
    return ServerType::Threaded;
  if (type == "epoll")
    return ServerType::Epoll;
  if (type == "uring")
    return ServerType::Uring;

  return ServerType::Unknown;
}
} // namespace common