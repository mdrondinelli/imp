#include <chrono>
#include <exception>
#include <iostream>

#include "core/world.h"
#include "graphics/renderer.h"
#include "graphics/static_vertex.h"
#include "math/constants.h"

int main() {
  auto p = imp::make_vector(0.0f, 0.0f, 0.0f);
  auto n = imp::normalize(imp::make_vector(1.0f, 1.0f, -1.0f));
  auto v = imp::static_vertex{imp::make_vector(0.0f, 0.0f, 0.0f), n};
  std::cout << "uncompressed = " << n << "\n";
  std::cout << "compressed = " << v.normal() << "\n";
  auto cos_theta = imp::min(imp::dot(n, v.normal()), 1.0f);
  std::cout << "cos theta = " << cos_theta << "\n";
  auto theta = std::acos(cos_theta);
  std::cout << "theta = " << theta * imp::constants<float>::pi / 180.0f << "\n";

  using namespace std::chrono_literals;
  try {
    imp::init_windows();
    auto gpu_context = imp::gpu_context{true, true};
    auto window = imp::window{
        gpu_context,
        imp::make_vector(1600, 900),
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
  return 0;
}