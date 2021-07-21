#pragma once

#include <memory>

#include <Eigen/Dense>

#include "../system/GpuBuffer.h"
#include "../system/GpuImage.h"

namespace imp {
  class GpuContext;

  class Scene;

  class SceneView {
  public:
    static constexpr auto UNIFORM_BUFFER_STRIDE = vk::DeviceSize{256};
    static constexpr auto UNIFORM_BUFFER_SIZE = vk::DeviceSize{108};
    static constexpr auto SKY_VIEW_IMAGE_EXTENT = Extent3u{128, 128, 1};

    class Flyweight {
    public:
      explicit Flyweight(
          gsl::not_null<GpuContext *> context, std::size_t frameCount);

    private:
      vk::DescriptorSetLayout createSkyViewDescriptorSetLayout() const;
      vk::DescriptorSetLayout createRenderDescriptorSetLayout() const;
      vk::PipelineLayout createSkyViewPipelineLayout() const;
      vk::PipelineLayout createRenderPipelineLayout() const;
      vk::Pipeline createSkyViewPipeline() const;
      vk::Pipeline createRenderPipeline() const;
      vk::Sampler createTransmittanceSampler() const;
      vk::Sampler createSkyViewSampler() const;

    public:
      ~Flyweight();

      gsl::not_null<GpuContext *> getContext() const noexcept;
      std::size_t getFrameCount() const noexcept;
      vk::DescriptorSetLayout getSkyViewDescriptorSetLayout() const noexcept;
      vk::DescriptorSetLayout getRenderDescriptorSetLayout() const noexcept;
      vk::PipelineLayout getSkyViewPipelineLayout() const noexcept;
      vk::PipelineLayout getRenderPipelineLayout() const noexcept;
      vk::Pipeline getSkyViewPipeline() const noexcept;
      vk::Pipeline getRenderPipeline() const noexcept;
      vk::Sampler getTransmittanceSampler() const noexcept;
      vk::Sampler getSkyViewSampler() const noexcept;

    private:
      gsl::not_null<GpuContext *> context_;
      std::size_t frameCount_;
      vk::DescriptorSetLayout skyViewDescriptorSetLayout_;
      vk::DescriptorSetLayout renderDescriptorSetLayout_;
      vk::PipelineLayout skyViewPipelineLayout_;
      vk::PipelineLayout renderPipelineLayout_;
      vk::Pipeline skyViewPipeline_;
      vk::Pipeline renderPipeline_;
      vk::Sampler transmittanceSampler_;
      vk::Sampler skyViewSampler_;
    };

    struct Frame {
      GpuImage renderImage;
      vk::ImageView skyViewImageView;
      vk::ImageView renderImageView;
      vk::DescriptorSet skyViewDescriptorSet;
      vk::DescriptorSet renderDescriptorSet;
      vk::CommandPool commandPool;
      vk::CommandBuffer skyViewCommandBuffer;
      vk::CommandBuffer renderCommandBuffer;
      vk::Semaphore skyViewSemaphore;
      vk::Semaphore renderSemaphore;
      std::shared_ptr<Scene> scene;

      explicit Frame(GpuImage &&renderImage);
    };

    explicit SceneView(
        gsl::not_null<Flyweight const *> flyweight,
        gsl::not_null<std::shared_ptr<Scene>> scene,
        Extent2u const &extent) noexcept;

  private:
    GpuBuffer createUniformBuffer() const;
    GpuImage createSkyViewImage() const;
    vk::DescriptorPool createDescriptorPool() const;
    std::vector<Frame> createFrames() const;
    GpuImage createRenderImage() const;
    void initSkyViewImageView(std::size_t i);
    void initRenderImageView(std::size_t i);
    void initSkyViewDescriptorSet(std::size_t i);
    void initRenderDescriptorSet(std::size_t i);
    void initCommandPool(std::size_t i);
    void initCommandBuffers(std::size_t i);
    void initSemaphores(std::size_t i);

  public:
    ~SceneView();

    void render(std::size_t i);

  private:
    void updateUniformBuffer(std::size_t i);
    void updateRenderImage(std::size_t i);
    void updateSkyViewDescriptorSet(std::size_t i);
    void updateRenderDescriptorSet(std::size_t i);
    void submitSkyViewCommandBuffer(std::size_t i);
    void submitRenderCommandBuffer(std::size_t i);

  public:
    gsl::not_null<Flyweight const *> getFlyweight() const noexcept;
    gsl::not_null<std::shared_ptr<Scene>> getScene() const noexcept;
    void setScene(gsl::not_null<std::shared_ptr<Scene>> scene) noexcept;
    Extent2u const &getExtent() const noexcept;
    void setExtent(Extent2u const &extent) noexcept;
    GpuBuffer const &getUniformBuffer() const noexcept;
    GpuImage const &getSkyViewImage() const noexcept;
    GpuImage const &getRenderImage(std::size_t i) const noexcept;
    vk::ImageView getSkyViewImageView(std::size_t i) const noexcept;
    vk::ImageView getRenderImageView(std::size_t i) const noexcept;
    vk::Semaphore getSkyViewSemaphore(std::size_t i) const noexcept;
    vk::Semaphore getRenderSemaphore(std::size_t i) const noexcept;
    Eigen::Matrix4f const &getViewMatrix() const noexcept;
    void setViewMatrix(Eigen::Matrix4f const &m) noexcept;
    Eigen::Matrix4f const &getProjectionMatrix() const noexcept;
    void setProjectionMatrix(Eigen::Matrix4f const &m) noexcept;

  private:
    gsl::not_null<Flyweight const *> flyweight_;
    gsl::not_null<std::shared_ptr<Scene>> scene_;
    Extent2u extent_;
    GpuBuffer uniformBuffer_;
    GpuImage skyViewImage_;
    vk::DescriptorPool descriptorPool_;
    std::vector<Frame> frames_;
    Eigen::Matrix4f viewMatrix_;
    Eigen::Matrix4f invViewMatrix_;
    Eigen::Matrix4f projectionMatrix_;
    Eigen::Matrix4f invProjectionMatrix_;
  };
} // namespace imp