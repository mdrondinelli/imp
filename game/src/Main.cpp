#include <chrono>
#include <exception>
#include <iostream>

#include "graphics/Renderer.h"
#include "graphics/Scene.h"
#include "graphics/SceneView.h"
#include "system/Display.h"
#include "system/GpuContext.h"

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
        imp::Spectrum{0.42f * 1367.0f / 3.0f},
        Eigen::Vector3f{0.0f, -1.0f, 1.0f}.normalized())};
    scene->setPlanet(earth);
    scene->setSunLight(sun);
    auto groundView = imp::gsl::not_null{std::make_shared<imp::SceneView>(
        renderer.getSceneViewFlyweight(), scene, imp::Extent2u{1920, 1080})};
    groundView->setExposure(1.0f / 16.0f);
    auto skyView = imp::gsl::not_null{std::make_shared<imp::SceneView>(
        renderer.getSceneViewFlyweight(), scene, imp::Extent2u{1920, 1080})};
    skyView->setExposure(1.0f / 8.0f);
    {
      auto viewMatrix = Eigen::Matrix4f::Identity().eval();
      viewMatrix(1, 3) = -2.0f;
      groundView->setViewMatrix(viewMatrix);
    }
    {
      auto viewMatrix = Eigen::Matrix4f{};
      viewMatrix.col(0) = Eigen::Vector4f{0.0f, 0.0f, -1.0f, 0.0f};
      viewMatrix.col(1) = Eigen::Vector4f{-1.0f, 0.0f, 0.0f, 0.0f};
      viewMatrix.col(2) = Eigen::Vector4f{0.0f, 1.0f, 0.0f, 0.0f};
      viewMatrix.col(3) = Eigen::Vector4f{
          0.0f, 1.0f * earth->getAtmosphereRadius(), 0.0f, 1.0f};
      viewMatrix = viewMatrix.inverse().eval();
      skyView->setViewMatrix(viewMatrix);
    }
    {
      auto tanHalfFovY = 0.514f;
      auto focalLength = 1.0f / tanHalfFovY;
      auto aspectRatio = float(groundView->getExtent().width) /
                         float(groundView->getExtent().height);
      auto n = 0.01f;
      auto f = 100.0f;
      auto projectionMatrix = Eigen::Matrix4f::Zero().eval();
      projectionMatrix(0, 0) = focalLength / aspectRatio;
      projectionMatrix(1, 1) = -focalLength;
      projectionMatrix(2, 2) = n / (f - n);
      projectionMatrix(2, 3) = n * f / (f - n);
      projectionMatrix(3, 2) = -1;
      groundView->setProjectionMatrix(projectionMatrix);
    }
    {
      auto tanHalfFovY = 0.7f;
      auto focalLength = 1.0f / tanHalfFovY;
      auto aspectRatio = float(skyView->getExtent().width) /
                         float(skyView->getExtent().height);
      auto n = 0.01f;
      auto f = 100.0f;
      auto projectionMatrix = Eigen::Matrix4f::Zero().eval();
      projectionMatrix(0, 0) = focalLength / aspectRatio;
      projectionMatrix(1, 1) = -focalLength;
      projectionMatrix(2, 2) = n / (f - n);
      projectionMatrix(2, 3) = n * f / (f - n);
      projectionMatrix(3, 2) = -1;
      skyView->setProjectionMatrix(projectionMatrix);
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
      auto theta = float(glfwGetTime()) * 0.0043633f * 1.0f - 0.05f;
      auto cosTheta = std::cos(theta);
      auto sinTheta = std::sin(theta);
      Eigen::Vector3f cosAxis = {0.0f, 0.0f, -1.0f};
      Eigen::Vector3f sinAxis = Eigen::Vector3f{1.0f, 2.0f, 0.0f}.normalized();
      sun->setDirection(cosAxis * cosTheta + sinAxis * sinTheta);
      if (window.getFramebufferWidth() != 0 &&
          window.getFramebufferHeight() != 0) {
        renderer.begin();
        renderer.draw(groundView, 0, 0, 1920, 1080);
        // renderer.draw(skyView, 0, 0, 1920, 1080);
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