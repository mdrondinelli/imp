#include <chrono>
#include <exception>
#include <iostream>

#include "graphics/Renderer.h"
#include "graphics/Scene.h"
#include "graphics/SceneView.h"
#include "system/Display.h"
#include "system/GpuContext.h"

// clang-format off
import mobula.gpu;
// clang-format on

int main() {
  using namespace std::chrono_literals;

  try {
    imp::Display::init();
    auto gpuContextCreateInfo = imp::GpuContextCreateInfo{};
    gpuContextCreateInfo.validation = false;
    gpuContextCreateInfo.presentation = true;
    auto gpuContext = imp::GpuContext{gpuContextCreateInfo};
    auto window =
        imp::Display{imp::gsl::not_null{&gpuContext}, 1920, 1080, "imp", true};
    auto renderer = imp::Renderer{imp::gsl::not_null{&window}, 3};
    auto scene = imp::gsl::not_null{
        std::make_shared<imp::Scene>(renderer.getSceneFlyweight())};
    auto earth = imp::gsl::not_null{std::make_shared<imp::Planet>()};
    earth->setPosition({0.0f, -earth->getGroundRadius(), 0.0f});
    auto sun = imp::gsl::not_null{std::make_shared<imp::DirectionalLight>(
        imp::Spectrum{191.4f},
        Eigen::Vector3f{0.0f, -1.0f, 1.0f}.normalized())};
    scene->setPlanet(earth);
    scene->setSunLight(sun);
    auto views =
        std::vector<imp::gsl::not_null<std::shared_ptr<imp::SceneView>>>{};
    for (auto i = 0; i < 1; ++i) {
      views.emplace_back(std::make_shared<imp::SceneView>(
          renderer.getSceneViewFlyweight(), scene, imp::Extent2u{1920, 1080}));
    }
    views[0]->setExposure(1.0f / 10.0f);
    // views[1]->setExposure(1.0f / 12.0f);
    // views[2]->setExposure(1.0f / 12.0f);
    // views[3]->setExposure(1.0f / 12.0f);
    // views[0]->setBloomEnabled(true);
    // views[1]->setBloomEnabled(true);
    // views[2]->setBloomEnabled(true);
    // views[3]->setBloomEnabled(true);
    {
      auto viewMatrix = Eigen::Matrix4f::Identity().eval();
      viewMatrix(1, 3) = -64.0f;
      views[0]->setViewMatrix(viewMatrix);
      /*viewMatrix(1, 3) = -256.0f;
      views[1]->setViewMatrix(viewMatrix);
      viewMatrix(1, 3) = -1024.0f;
      views[2]->setViewMatrix(viewMatrix);
      viewMatrix(1, 3) = -4096.0f;
      views[3]->setViewMatrix(viewMatrix);*/
    }
    {
      auto tanHalfFovY = 1.0f;
      auto focalLength = 1.0f / tanHalfFovY;
      auto aspectRatio = float(views[0]->getExtent().width) /
                         float(views[0]->getExtent().height);
      auto n = 0.01f;
      auto f = 100.0f;
      auto projectionMatrix = Eigen::Matrix4f::Zero().eval();
      projectionMatrix(0, 0) = focalLength / aspectRatio;
      projectionMatrix(1, 1) = -focalLength;
      projectionMatrix(2, 2) = n / (f - n);
      projectionMatrix(2, 3) = n * f / (f - n);
      projectionMatrix(3, 2) = -1;
      views[0]->setProjectionMatrix(projectionMatrix);
      // views[1]->setProjectionMatrix(projectionMatrix);
      // views[2]->setProjectionMatrix(projectionMatrix);
      // views[3]->setProjectionMatrix(projectionMatrix);
    }
    // Eigen::Matrix4f getProjectionMatrix(Camera const &c) noexcept {
    //  //  auto w = 2 * c.getTanHalfFovX();
    //  //  auto h = 2 * c.getTanHalfFovY();
    //  //  auto n = c.getMinDepth();
    //  //  auto f = c.getMaxDepth();
    //  //  auto m = Eigen::Matrix4f::Zero().eval();
    //  //  m(0, 0) = 2 * n / w;
    //  //  m(1, 1) = 2 * n / -h;
    //  //  m(2, 2) = n / (f - n);
    //  //  m(2, 3) = f * n / (f - n);
    //  //  m(3, 2) = -1;
    //  //  return m;
    //  //}
    auto frame_time = std::chrono::high_resolution_clock::now();
    auto frame_count = 0;
    while (!window.shouldClose()) {
      imp::Display::poll();
      auto theta = float(glfwGetTime()) * 0.0034906585f * 4.5f - 0.1f;
      auto cosTheta = std::cos(theta);
      auto sinTheta = std::sin(theta);
      Eigen::Vector3f cosAxis = {0.0f, 0.0f, -1.0f};
      Eigen::Vector3f sinAxis = Eigen::Vector3f{0.0f, 1.0f, 0.0f}.normalized();
      sun->setDirection(cosAxis * cosTheta + sinAxis * sinTheta);
      if (window.getFramebufferWidth() != 0 &&
          window.getFramebufferHeight() != 0) {
        renderer.begin();
        for (auto i = 0; i < 1; ++i) {
          renderer.draw(views[i], 0, 0, 1920, 1080);
        }
        // renderer.draw(groundView, 0, 0, 1920, 1080);
        renderer.end();
        ++frame_count;
      }
      if (std::chrono::high_resolution_clock::now() - frame_time > 1s) {
        std::cout << frame_count << " fps\n";
        frame_time = std::chrono::high_resolution_clock::now();
        frame_count = 0;
      }
    }
    gpuContext.getDevice().waitIdle();
  } catch (std::exception &e) {
    std::cerr << e.what() << "\n";
  }
  return 0;
}