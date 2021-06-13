#include <chrono>
#include <exception>
#include <iostream>

#include "core/Display.h"
#include "core/GpuContext.h"
#include "core/ResourceCache.h"
#include "graphics/Camera.h"
#include "graphics/Renderer.h"
#include "graphics/Scene.h"

struct DummyResource {
  int wenis;
};

struct DummyResourceInfo {
  int wonis;
};

struct DummyResourceLoader:
    imp::ResourceLoader<DummyResource, DummyResourceInfo> {
  std::optional<DummyResource> load(DummyResourceInfo const &info) override {
    using namespace std::chrono_literals;
    std::cout << "loading " << info.wonis << "\n";
    std::this_thread::sleep_for(2000ms);
    std::cout << "loaded " << info.wonis << "\n";
    return DummyResource{info.wonis};
  }
};

using DummyResourceCache = imp::ResourceCache<DummyResource, DummyResourceInfo>;

int main() {
  using namespace std::chrono_literals;

  imp::Display::init();
  auto worker = imp::WorkerThread{};
  auto loader = DummyResourceLoader{};

  try {
    auto cache = DummyResourceCache{worker, loader};
    cache.insert("bangus", {69});
    cache.insert("bengus", {70});
    cache.insert("bingus", {71});
    cache.insert("bongus", {72});
    cache.insert("bungus", {73});
    auto bangus = cache.at("bangus");
    auto bengus = cache.at("bengus");
    auto bingus = cache.at("bingus");
    auto bongus = cache.at("bongus");
    auto bungus = cache.at("bungus");
    // GpuContext
    auto gpuContextCreateInfo = imp::GpuContextCreateInfo{};
    gpuContextCreateInfo.validation = true;
    gpuContextCreateInfo.presentation = true;
    auto gpuContext = imp::GpuContext{gpuContextCreateInfo};
    // Window
    auto window = imp::Display{&gpuContext, 1920, 1080, "imp", true};
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
    auto renderer = imp::Renderer{&window};
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!window.shouldClose()) {
      imp::Display::poll();
      auto angle = float(glfwGetTime()) * 0.0043633f * 2.0f - 0.15f;
      sun->setDirection(
          Eigen::Vector3f{0.0f, std::sin(angle), -std::cos(angle)});
      if (window.getFramebufferWidth() != 0 &&
          window.getFramebufferHeight() != 0) {
        auto aspect = float(window.getFramebufferWidth()) /
                      float(window.getFramebufferHeight());
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
  worker.join();
  return 0;
}