#pragma once

namespace echo {

class Server {
public:
  virtual ~Server() = default;

  virtual void stop() = 0;
  virtual void run() = 0;
};
} // namespace echo