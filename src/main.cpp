#include <chrono>
#include <exception>
#include <iostream>

#include "core/resource_cache.h"
#include "core/world.h"
#include "graphics/renderer.h"
#include "graphics/static_vertex.h"
#include "math/constants.h"

struct dummy_resource {
  int wenis;
};

struct dummy_resource_info {
  int wonis;
};

using dummy_resource_cache =
    imp::resource_cache<dummy_resource, dummy_resource_info>;

struct dummy_resource_loader:
    dummy_resource_cache::loader {
  std::optional<dummy_resource> load(dummy_resource_info const &info) override {
    using namespace std::chrono_literals;
    std::cout << "loading " << info.wonis << "\n";
    std::this_thread::sleep_for(2000ms);
    std::cout << "loaded " << info.wonis << "\n";
    return dummy_resource{info.wonis};
  }
};


int main() {
  using namespace std::chrono_literals;

  auto worker = imp::worker_thread{};
  auto loader = dummy_resource_loader{};

  try {
    auto cache = dummy_resource_cache{&worker, &loader};
    cache.try_emplace("bangus", dummy_resource_info{69});
    cache.try_emplace("bengus", dummy_resource_info{70});
    cache.try_emplace("bingus", dummy_resource_info{71});
    cache.try_emplace("bongus", dummy_resource_info{72});
    cache.try_emplace("bungus", dummy_resource_info{73});
    auto bangus = cache.at("bangus");
    auto bengus = cache.at("bengus");
    auto bingus = cache.at("bingus");
    auto bongus = cache.at("bongus");
    auto bungus = cache.at("bungus");

    imp::init_windows();
    auto gpu_context = imp::gpu_context{true, true};
    auto window = imp::window{
        gpu_context,
        imp::make_vector(640, 480),
        "I Love Laiba Sunrise Simulator Sunrise Oooweeeh"};
    auto renderer = imp::renderer{gpu_context, window};
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!window.should_close()) {
      imp::poll_windows();
      if (window.framebuffer_size() != 0) {
        renderer.render();
      }
      ++frame_count;
      if (std::chrono::high_resolution_clock::now() - frame_time > 1s) {
        std::cout << frame_count << " fps\n";
        frame_time = std::chrono::high_resolution_clock::now();
        frame_count = 0;
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << "\n";
  }
  worker.join();
  return 0;
}