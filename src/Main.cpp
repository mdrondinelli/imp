#include <chrono>
#include <exception>
#include <iostream>

#include "graphics/Camera.h"
#include "graphics/Renderer.h"
#include "graphics/Scene.h"
#include "system/Display.h"
#include "system/GpuContext.h"

int main() {
  using namespace std::chrono_literals;

  try {
    imp::Display::init();
    // GpuContext
    auto gpuContextCreateInfo = imp::GpuContextCreateInfo{};
    gpuContextCreateInfo.validation = true;
    gpuContextCreateInfo.presentation = true;
    auto gpuContext = imp::GpuContext{gpuContextCreateInfo};
    // Display
    auto display = imp::Display{&gpuContext, 1920, 1080, "imp", true};
    // Scene
    auto atmosphere = std::make_shared<imp::Atmosphere>();
    auto sun = std::make_shared<imp::DirectionalLight>(
        imp::Spectrum{200.0f}, Eigen::Vector3f{0.0f, -1.0f, 1.0f}.normalized());
    auto scene = imp::Scene{};
    scene.setAtmosphere(atmosphere);
    scene.setSunLight(sun);
    // Camera
    auto camera =
        std::make_shared<imp::Camera>(Eigen::Vector3f{0.0f, 2.0f, 0.0f});
    // Renderer
    auto renderer = imp::Renderer{&display};
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!display.shouldClose()) {
      imp::Display::poll();
      auto angle = float(glfwGetTime()) * 0.0043633f * 2.0f - 0.15f;
      sun->setDirection(
          Eigen::Vector3f{0.0f, std::sin(angle), -std::cos(angle)});
      if (display.getFramebufferWidth() != 0 &&
          display.getFramebufferHeight() != 0) {
        auto aspect = float(display.getFramebufferWidth()) /
                      float(display.getFramebufferHeight());
        if (aspect >= 1.0f) {
          camera->setTanHalfFovX(1.0f);
          camera->setTanHalfFovY(camera->getTanHalfFovX() / aspect);
        } else {
          camera->setTanHalfFovY(1.0f);
          camera->setTanHalfFovX(camera->getTanHalfFovY() * aspect);
        }
        renderer.render(scene, *camera);
        ++frame_count;
      }
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