#include <chrono>
#include <exception>
#include <iostream>

#include "core/gpu_manager.h"
#include "core/window.h"
#include "core/world.h"
#include "graphics/renderer.h"

int main() {
  using namespace std::chrono_literals;
  try {
    imp::init_windows();
    auto gpu_manager_create_info = imp::gpu_manager_create_info{};
    gpu_manager_create_info.validation_enabled = true;
    gpu_manager_create_info.presentation_enabled = true;
    auto gpu_manager = imp::gpu_manager{gpu_manager_create_info};
    auto window_manager_create_info = imp::window_manager_create_info{};
    window_manager_create_info.instance = gpu_manager.instance();
    window_manager_create_info.size = imp::make_vector(1600, 900);
    window_manager_create_info.title = "I love Laiba!";
    auto window_manager = imp::window_manager{window_manager_create_info};
    auto renderer_create_info = imp::renderer_create_info{};
    renderer_create_info.gpu_manager = &gpu_manager;
    renderer_create_info.window_manager = &window_manager;
    auto renderer = imp::renderer{renderer_create_info};
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!window_manager.should_close()) {
      imp::poll_windows();
      renderer.render();
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