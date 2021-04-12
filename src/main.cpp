#include <chrono>
#include <exception>
#include <iostream>

#include "core/world.h"
#include "graphics/renderer.h"

int main() {
  using namespace std::chrono_literals;
  try {
    imp::init_windows();
    auto gpu_context = imp::gpu_context{false, true};
    auto window =
        imp::window{gpu_context, imp::make_vector(1600, 900), "I love Laiba"};
    auto renderer = imp::renderer{gpu_context, window};
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!window.should_close()) {
      imp::poll_windows();
      if (window.framebuffer_width() + window.framebuffer_height() != 0) {
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
  return 0;
}