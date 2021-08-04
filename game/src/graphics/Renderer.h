#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Scene.h"
#include "SceneView.h"

namespace imp {
  class Display;

  class Renderer {
  public:
    static constexpr auto VERTEX_SIZE = vk::DeviceSize{16};
    static constexpr auto VERTEX_BUFFER_SIZE = 65536 * VERTEX_SIZE;
    static constexpr auto INDEX_SIZE = vk::DeviceSize{2};
    static constexpr auto INDEX_BUFFER_SIZE = 98304 * INDEX_SIZE;
    static constexpr auto TEXTURE_ARRAY_SIZE = std::uint32_t{1024};

    struct Frame {
      vk::DescriptorSet descriptorSet;
      vk::CommandPool commandPool;
      vk::CommandBuffer commandBuffer;
      vk::Semaphore swapchainSemaphore;
      vk::Semaphore frameSemaphore;
      vk::Fence frameFence;
    };

    struct Vertex {
      Eigen::Vector2f position;
      std::uint32_t vertexIndex;
      std::uint32_t textureIndex;
    };

    static_assert(sizeof(Vertex) == 16);
    static_assert(offsetof(Vertex, position) == 0);
    static_assert(offsetof(Vertex, vertexIndex) == 8);
    static_assert(offsetof(Vertex, textureIndex) == 12);

    explicit Renderer(gsl::not_null<Display *> window, std::size_t frameCount);

  private:
    // vk::RenderPass createRenderPass() const;
    vk::DescriptorSetLayout createDescriptorSetLayout() const;
    vk::PipelineLayout createPipelineLayout() const;
    vk::Pipeline createPipeline() const;
    vk::Sampler createSampler() const;
    GpuImage createDefaultRenderImage() const;
    vk::ImageView createDefaultRenderImageView() const;
    vk::DescriptorPool createDescriptorPool() const;
    GpuBuffer createVertexBuffer() const;
    GpuBuffer createIndexBuffer() const;
    void initDescriptorSets();
    void initCommandPools();
    void initCommandBuffers();
    void initSynchronization();

    /*
    std::vector<vk::CommandPool> createCommandPools() const;
    std::vector<vk::CommandBuffer> allocateCommandBuffers();
    std::vector<vk::Semaphore> createSemaphores() const;
    std::vector<vk::Fence> createFences() const;
    vk::DescriptorPool createDescriptorPool() const;
    std::vector<vk::DescriptorSet> allocateDescriptorSets();
    */

  public:
    ~Renderer();

    void begin();
    void end();

    /*void pushTranslation(Eigen::Vector2i const &v);
    void popTranslation();

    void drawBox(
        unsigned width, unsigned height, Spectrum const &color, float alpha);*/

    void draw(
        gsl::not_null<std::shared_ptr<SceneView>> sceneView,
        int x,
        int y,
        int w,
        int h);

    gsl::not_null<Scene::Flyweight const *> getSceneFlyweight() const noexcept;
    gsl::not_null<SceneView::Flyweight const *>
    getSceneViewFlyweight() const noexcept;

  private:

    gsl::not_null<Display *> window_;
    Scene::Flyweight sceneFlyweight_;
    SceneView::Flyweight sceneViewFlyweight_;
    vk::DescriptorSetLayout descriptorSetLayout_;
    vk::PipelineLayout pipelineLayout_;
    vk::Pipeline pipeline_;
    vk::Sampler sampler_;
    GpuImage defaultRenderImage_;
    vk::ImageView defaultRenderImageView_;
    std::vector<Frame> frames_;
    vk::DescriptorPool descriptorPool_;
    GpuBuffer vertexBuffer_;
    GpuBuffer indexBuffer_;
    Vertex *vertexBufferData_;
    std::uint16_t *indexBufferData_;
    std::uint16_t vertexBufferIndex_;
    std::uint32_t indexBufferIndex_;
    std::uint32_t textureIndex_;
    std::uint32_t ditherSeed_;
    std::size_t frameIndex_;

    std::unordered_set<gsl::not_null<std::shared_ptr<Scene>>> scenes_;
    std::unordered_map<gsl::not_null<std::shared_ptr<SceneView>>, std::uint32_t>
        sceneViews_;
  };
} // namespace imp