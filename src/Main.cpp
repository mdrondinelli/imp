#include <chrono>
#include <exception>
#include <iostream>

#include "core/ResourceCache.h"
#include "graphics/Renderer.h"

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

  imp::initWindows();
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
    // Atmosphere
    auto atmosphereCreateInfo = imp::AtmosphereCreateInfo{};
    atmosphereCreateInfo.context = &gpuContext;
    auto atmosphere = std::make_shared<imp::Atmosphere>(atmosphereCreateInfo);
    // Camera
    auto cameraCreateInfo = imp::CameraCreateInfo{};
    cameraCreateInfo.transform.translate({0.0f, 100.0f, 0.0f});
    auto camera = std::make_shared<imp::Camera>(cameraCreateInfo);
    // Scene
    auto sceneCreateInfo = imp::SceneCreateInfo{};
    sceneCreateInfo.context = &gpuContext;
    sceneCreateInfo.atmosphere = atmosphere;
    sceneCreateInfo.camera = camera;
    auto scene = imp::Scene{sceneCreateInfo};
    // Window
    auto windowCreateInfo = imp::WindowCreateInfo{};
    windowCreateInfo.context = &gpuContext;
    windowCreateInfo.size = {800u, 600u};
    windowCreateInfo.title = "I Love Laiba Sunrise Simulator Sunrise Oooweeeh";
    auto window = imp::Window{windowCreateInfo};
    // Renderer
    auto rendererCreateInfo = imp::RendererCreateInfo{};
    rendererCreateInfo.context = &gpuContext;
    rendererCreateInfo.window = &window;
    auto renderer = imp::Renderer{rendererCreateInfo};
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!window.shouldClose()) {
      imp::pollWindows();
      if (window.getFramebufferSize() != imp::Vector2u{}) {
        auto aspect = float(window.getSwapchainSize()[0]) /
                      float(window.getSwapchainSize()[1]);
        camera->setTanHalfFovX(aspect * camera->getTanHalfFovY());
        renderer.render(scene);
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