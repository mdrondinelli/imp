#include "Client.h"

namespace imp {
  Client::~Client() {
    loadingThread_.join();
  }
} // namespace imp