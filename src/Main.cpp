#include <chrono>
#include <exception>
#include <iostream>

#include "core/ResourceCache.h"
#include "graphics/Renderer.h"
#include "math/Constants.h"

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
    gpuContextCreateInfo.validation = false;
    gpuContextCreateInfo.presentation = true;
    auto gpuContext = imp::GpuContext{gpuContextCreateInfo};
    // Atmosphere
    auto atmosphere = std::make_shared<imp::Atmosphere>();
    //atmosphere->setRayleighScattering(0.0f);
    //atmosphere->setMieScattering(atmosphere->getMieScattering() * 5.0f);
    //atmosphere->setMieAbsorption(atmosphere->getMieAbsorption() * 5.0f);
    // Camera
    auto cameraCreateInfo = imp::CameraCreateInfo{};
    cameraCreateInfo.transform.translate(imp::Vector3f{0.0f, 2.0f, 0.0f});
    cameraCreateInfo.transform.rotateGlobal(imp::rotationQuaternion(
        imp::toRadians(0.0f), imp::Vector3f{1.0f, 0.0f, 0.0f}));
    auto camera = std::make_shared<imp::Camera>(cameraCreateInfo);
    // Sun
    // wikipedia: radiance of the sun is 15.4 megawatts per meter squared per
    // steradian. 42% of that is visible. I assume evenly divided into rgb.
    auto sunCreateInfo = imp::DirectionalLightCreateInfo{};
    sunCreateInfo.irradiance = imp::Vector3f{200.0f};
    sunCreateInfo.direction = imp::normalize(imp::Vector3f{0.0f, -1.0f, 1.0f});
    auto sun = std::make_shared<imp::DirectionalLight>(sunCreateInfo);
    // Scene
    auto sceneCreateInfo = imp::SceneCreateInfo{};
    sceneCreateInfo.context = &gpuContext;
    sceneCreateInfo.atmosphere = atmosphere;
    sceneCreateInfo.sunLight = sun;
    sceneCreateInfo.moonLight = nullptr;
    sceneCreateInfo.camera = camera;
    auto scene = imp::Scene{sceneCreateInfo};
    // Window
    auto windowCreateInfo = imp::WindowCreateInfo{};
    windowCreateInfo.context = &gpuContext;
    windowCreateInfo.size = imp::Vector2u{1920u, 1080u};
    windowCreateInfo.title = "I Love Laiba Sunrise Simulator Sunrise Oooweeeh";
    windowCreateInfo.fullscreen = true;
    auto window = imp::Window{windowCreateInfo};
    // Renderer
    auto rendererCreateInfo = imp::RendererCreateInfo{};
    rendererCreateInfo.window = &window;
    auto renderer = imp::Renderer{rendererCreateInfo};
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!window.shouldClose()) {
      imp::pollWindows();
      if (window.getFramebufferSize() != imp::Vector2u{}) {
        auto angle = float(glfwGetTime()) * 0.0043633f * 2.0f - 0.15f;
        //auto angle = imp::toRadians(45.0f);
        sun->setDirection(
            imp::Vector3f{0.0f, imp::sin(angle), -imp::cos(angle)});
        auto aspect = float(window.getFramebufferSize()[0]) /
                      float(window.getFramebufferSize()[1]);
        if (aspect >= 1.0f) {
          camera->setTanHalfFovX(imp::tan(imp::toRadians(70.0f / 2.0f)));
          camera->setTanHalfFovY(camera->getTanHalfFovX() / aspect);
        } else {
          camera->setTanHalfFovY(imp::tan(imp::toRadians(70.0f / 2.0f)));
          camera->setTanHalfFovX(camera->getTanHalfFovY() * aspect);
        }
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