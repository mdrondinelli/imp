#pragma once

#include <memory>

#include <Eigen/Dense>
#include <absl/container/flat_hash_map.h>

#include "../system/GpuBuffer.h"
#include "../system/GpuImage.h"
#include "../util/Align.h"

namespace imp {
  class GpuContext;

  class Scene;

  class SceneView {
  public:
    static constexpr auto UNIFORM_BUFFER_SIZE = std::size_t{92};
    static constexpr auto UNIFORM_BUFFER_STRIDE =
        align<256>(UNIFORM_BUFFER_SIZE);
    static constexpr auto SKY_VIEW_IMAGE_EXTENT = Extent3u{128, 128, 1};

    class Flyweight {
    public:
      explicit Flyweight(
          gsl::not_null<GpuContext *> context, std::size_t frameCount);

    private:
      vk::DescriptorSetLayout createSkyViewDescriptorSetLayout() const;
      vk::DescriptorSetLayout createRenderDescriptorSetLayout() const;
      vk::DescriptorSetLayout createDownsampleDescriptorSetLayout() const;
      vk::DescriptorSetLayout createBlurDescriptorSetLayout() const;
      vk::DescriptorSetLayout createBlurBlendDescriptorSetLayout() const;
      vk::DescriptorSetLayout createBloomDescriptorSetLayout() const;
      vk::PipelineLayout createSkyViewPipelineLayout() const;
      vk::PipelineLayout createRenderPipelineLayout() const;
      vk::PipelineLayout createDownsamplePipelineLayout() const;
      vk::PipelineLayout createBlurPipelineLayout() const;
      vk::PipelineLayout createBlurBlendPipelineLayout() const;
      vk::PipelineLayout createBloomPipelineLayout() const;
      vk::Pipeline createSkyViewPipeline() const;
      vk::Pipeline createRenderPipeline() const;
      vk::Pipeline createDownsamplePipeline() const;
      absl::flat_hash_map<int, vk::Pipeline> createBlurPipelines() const;
      absl::flat_hash_map<int, vk::Pipeline> createBlurBlendPipelines() const;
      vk::Pipeline createBloomPipeline() const;
      vk::Sampler createSkyViewSampler() const;
      vk::Sampler createGeneralSampler() const;

    public:
      ~Flyweight();

      gsl::not_null<GpuContext *> getContext() const noexcept;
      std::size_t getFrameCount() const noexcept;
      vk::DescriptorSetLayout getSkyViewDescriptorSetLayout() const noexcept;
      vk::DescriptorSetLayout getRenderDescriptorSetLayout() const noexcept;
      vk::DescriptorSetLayout getDownsampleDescriptorSetLayout() const noexcept;
      vk::DescriptorSetLayout getBlurDescriptorSetLayout() const noexcept;
      vk::DescriptorSetLayout getBlurBlendDescriptorSetLayout() const noexcept;
      vk::DescriptorSetLayout getBloomDescriptorSetLayout() const noexcept;
      vk::PipelineLayout getSkyViewPipelineLayout() const noexcept;
      vk::PipelineLayout getRenderPipelineLayout() const noexcept;
      vk::PipelineLayout getDownsamplePipelineLayout() const noexcept;
      vk::PipelineLayout getBlurPipelineLayout() const noexcept;
      vk::PipelineLayout getBlurBlendPipelineLayout() const noexcept;
      vk::PipelineLayout getBloomPipelineLayout() const noexcept;
      vk::Pipeline getSkyViewPipeline() const noexcept;
      vk::Pipeline getRenderPipeline() const noexcept;
      vk::Pipeline getDownsamplePipeline() const noexcept;
      vk::Pipeline getBlurPipeline(int kernelSize) const noexcept;
      vk::Pipeline getBlurBlendPipeline(int kernelSize) const noexcept;
      vk::Pipeline getBloomPipeline() const noexcept;
      vk::Sampler getSkyViewSampler() const noexcept;
      vk::Sampler getGeneralSampler() const noexcept;

    private:
      gsl::not_null<GpuContext *> context_;
      std::size_t frameCount_;
      vk::DescriptorSetLayout skyViewDescriptorSetLayout_;
      vk::DescriptorSetLayout renderDescriptorSetLayout_;
      vk::DescriptorSetLayout downsampleDescriptorSetLayout_;
      vk::DescriptorSetLayout blurDescriptorSetLayout_;
      vk::DescriptorSetLayout blurBlendDescriptorSetLayout_;
      vk::DescriptorSetLayout bloomDescriptorSetLayout_;
      vk::PipelineLayout skyViewPipelineLayout_;
      vk::PipelineLayout renderPipelineLayout_;
      vk::PipelineLayout downsamplePipelineLayout_;
      vk::PipelineLayout blurPipelineLayout_;
      vk::PipelineLayout blurBlendPipelineLayout_;
      vk::PipelineLayout bloomPipelineLayout_;
      vk::Pipeline skyViewPipeline_;
      vk::Pipeline renderPipeline_;
      vk::Pipeline downsamplePipeline_;
      absl::flat_hash_map<int, vk::Pipeline> blurPipelines_;
      absl::flat_hash_map<int, vk::Pipeline> blurBlendPipelines_;
      vk::Pipeline bloomPipeline_;
      vk::Sampler skyViewSampler_;
      vk::Sampler generalSampler_;
    };

    struct Frame {
      GpuImage renderImage;
      GpuImage smallBloomImageArray;
      GpuImage mediumBloomImageArray;
      GpuImage largeBloomImageArray;
      vk::ImageView skyViewImageView;
      vk::ImageView renderImageView;
      vk::ImageView halfRenderImageView;
      vk::ImageView fourthRenderImageView;
      vk::ImageView eighthRenderImageView;
      vk::ImageView smallBloomImageViews[2];
      vk::ImageView mediumBloomImageViews[2];
      vk::ImageView largeBloomImageViews[2];
      vk::DescriptorSet skyViewDescriptorSet;
      vk::DescriptorSet renderDescriptorSet;
      vk::DescriptorSet downsampleDescriptorSets[3];
      vk::DescriptorSet smallBlurDescriptorSets[2];
      vk::DescriptorSet mediumBlurDescriptorSets[2];
      vk::DescriptorSet largeBlurDescriptorSets[2];
      vk::DescriptorSet bloomDescriptorSet;
      vk::CommandPool commandPool;
      vk::CommandBuffer commandBuffer;
      vk::Semaphore semaphore;
      std::shared_ptr<Scene> scene;

      explicit Frame(
          GpuImage &&renderImage,
          GpuImage &&smallBloomImageArray,
          GpuImage &&mediumBloomImageArray,
          GpuImage &&largeBloomImageArray);
    };

    explicit SceneView(
        gsl::not_null<Flyweight const *> flyweight,
        gsl::not_null<std::shared_ptr<Scene>> scene,
        Extent2u const &extent) noexcept;

  private:
    GpuBuffer createUniformBuffer() const;
    GpuImage createSkyViewImage() const;
    std::vector<Frame> createFrames() const;
    GpuImage createRenderImage() const;
    GpuImage createSmallBloomImageArray() const;
    GpuImage createMediumBloomImageArray() const;
    GpuImage createLargeBloomImageArray() const;
    vk::DescriptorPool createDescriptorPool() const;
    vk::CommandPool createTransitionCommandPool() const;
    vk::CommandBuffer createTransitionCommandBuffer();
    void initSkyViewImageView(std::size_t i);
    void initRenderImageView(std::size_t i);
    void initHalfRenderImageView(std::size_t i);
    void initFourthRenderImageView(std::size_t i);
    void initEighthRenderImageView(std::size_t i);
    void initSmallBloomImageViews(std::size_t i);
    void initMediumBloomImageViews(std::size_t i);
    void initLargeBloomImageViews(std::size_t i);
    void allocateDescriptorSets(std::size_t i);
    void initSkyViewDescriptorSet(std::size_t i);
    void initRenderDescriptorSet(std::size_t i);
    void initDownsampleDescriptorSets(std::size_t i);
    void initSmallBlurDescriptorSets(std::size_t i);
    void initMediumBlurDescriptorSets(std::size_t i);
    void initLargeBlurDescriptorSets(std::size_t i);
    void initBloomDescriptorSet(std::size_t i);
    void initCommandPool(std::size_t i);
    void initCommandBuffers(std::size_t i);
    void initSemaphores(std::size_t i);

  public:
    ~SceneView();

    void render(std::size_t i);

  private:
    void updateUniformBuffer(std::size_t i);
    void updateRenderImages(std::size_t i);
    void updateSkyViewDescriptorSet(std::size_t i);
    void updateRenderDescriptorSet(std::size_t i);
    void submitCommands(std::size_t i);
    void computeSkyViewImage(std::size_t i);
    void computeRenderImage(std::size_t i);
    void computeRenderImageMips(std::size_t i);
    void computeBloomImageArray(std::size_t i);
    void applyBloom(std::size_t i);

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
    vk::ImageView getFullRenderImageView(std::size_t i) const noexcept;
    vk::Semaphore getSemaphore(std::size_t i) const noexcept;
    Eigen::Matrix4f const &getViewMatrix() const noexcept;
    void setViewMatrix(Eigen::Matrix4f const &m) noexcept;
    Eigen::Matrix4f const &getProjectionMatrix() const noexcept;
    void setProjectionMatrix(Eigen::Matrix4f const &m) noexcept;
    float getExposure() const noexcept;
    void setExposure(float exposure) noexcept;

  private:
    gsl::not_null<Flyweight const *> flyweight_;
    gsl::not_null<std::shared_ptr<Scene>> scene_;
    Extent2u extent_;
    GpuBuffer uniformBuffer_;
    GpuImage skyViewImage_;
    vk::DescriptorPool descriptorPool_;
    vk::CommandPool transitionCommandPool_;
    vk::CommandBuffer transitionCommandBuffer_;
    vk::Semaphore transitionSemaphore_;
    vk::Fence transitionFence_;
    std::vector<Frame> frames_;
    Eigen::Matrix4f viewMatrix_;
    Eigen::Matrix4f invViewMatrix_;
    Eigen::Matrix4f projectionMatrix_;
    Eigen::Matrix4f invProjectionMatrix_;
    float exposure_;
    std::uint32_t smallBloomKernel_;
    std::uint32_t mediumBloomKernel_;
    std::uint32_t largeBloomKernel_;
    float smallBloomWeight_;
    float mediumBloomWeight_;
    float largeBloomWeight_;
    bool firstFrame_;
  };
} // namespace imp