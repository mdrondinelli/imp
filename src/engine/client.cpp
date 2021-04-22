#include "client.h"

namespace imp {
  client::~client() {
    loading_thread_.join();
  }
} // namespace imp