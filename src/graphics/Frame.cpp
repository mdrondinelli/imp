#include "Frame.h"

namespace imp {
  Frame::Frame(
      std::shared_ptr<FrameFlyweight const> flyweight,
      FrameCreateInfo const &createInfo):
      flyweight_{std::move(flyweight)},
      imageAcquisitionSemaphore_{flyweight_->getWindow()
                                     ->getContext()
                                     ->getDevice()
                                     .createSemaphoreUnique({})},
      queueSubmissionSemaphore_{flyweight_->getWindow()
                                    ->getContext()
                                    ->getDevice()
                                    .createSemaphoreUnique({})},
      queueSubmissionFence_{
          flyweight_->getWindow()->getContext()->getDevice().createFenceUnique(
              {vk::FenceCreateFlagBits::eSignaled})},
      commandPool_{createCommandPool()},
      commandBuffer_{allocateCommandBuffer()},
      transmittanceLut_{
          flyweight_->getTransmittanceLutFlyweight(),
          createInfo.transmittanceLutSize},
      skyViewLut_{
          flyweight_->getSkyViewLutFlyweight(), createInfo.skyViewLutSize} {
  }

  void Frame::render(Scene const &scene, std::uint32_t seed) {
    auto &window = *flyweight_->getWindow();
    auto &context = *window.getContext();
    auto framebuffer =
        window.acquireFramebuffer(*imageAcquisitionSemaphore_, {});
    auto beginInfo = vk::CommandBufferBeginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    auto waitStage = vk::PipelineStageFlags{
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    auto submitInfo = vk::SubmitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &*imageAcquisitionSemaphore_;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &*commandBuffer_;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &*queueSubmissionSemaphore_;
    context.getDevice().waitForFences(
        *queueSubmissionFence_,
        true,
        std::numeric_limits<std::uint64_t>::max());
    context.getDevice().resetFences(*queueSubmissionFence_);
    context.getDevice().resetCommandPool(*commandPool_);
    commandBuffer_->begin(beginInfo);
    computeTransmittanceLut(scene);
    computeSkyViewLut(scene);
    renderAtmosphere(scene, seed, framebuffer);
    commandBuffer_->end();
    context.getGraphicsQueue().submit(submitInfo, *queueSubmissionFence_);
    window.presentFramebuffer(1, &*queueSubmissionSemaphore_, framebuffer);
  }

  namespace {
    vk::ImageMemoryBarrier createLayoutTransition(
        vk::AccessFlags srcAccessMask,
        vk::AccessFlags dstAccessMask,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::Image image) {
      auto barrier = vk::ImageMemoryBarrier{};
      barrier.srcAccessMask = srcAccessMask;
      barrier.dstAccessMask = dstAccessMask;
      barrier.oldLayout = oldLayout;
      barrier.newLayout = newLayout;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = image;
      barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      return barrier;
    }
  } // namespace

  void Frame::computeTransmittanceLut(Scene const &scene) {
    if (transmittanceLut_.compute(*commandBuffer_, scene)) {
      commandBuffer_->pipelineBarrier(
          vk::PipelineStageFlagBits::eComputeShader,
          vk::PipelineStageFlagBits::eComputeShader |
              vk::PipelineStageFlagBits::eFragmentShader,
          {},
          {},
          {},
          {createLayoutTransition(
              vk::AccessFlagBits::eShaderWrite,
              vk::AccessFlagBits::eShaderRead,
              vk::ImageLayout::eGeneral,
              vk::ImageLayout::eShaderReadOnlyOptimal,
              transmittanceLut_.getImage())});
    }
  }

  void Frame::computeSkyViewLut(Scene const &scene) {
    skyViewLut_.compute(*commandBuffer_, scene, transmittanceLut_);
    commandBuffer_->pipelineBarrier(
        vk::PipelineStageFlagBits::eComputeShader,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        {},
        {},
        {createLayoutTransition(
            vk::AccessFlagBits::eShaderWrite,
            vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eGeneral,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            skyViewLut_.getImage())});
  }

  void Frame::renderAtmosphere(
      Scene const &scene, std::uint32_t seed, vk::Framebuffer framebuffer) {
    auto &window = *flyweight_->getWindow();
    auto clearValue = vk::ClearValue{};
    clearValue.color.float32 = std::array{0.0f, 0.0f, 0.0f, 0.0f};
    auto renderPassBegin = vk::RenderPassBeginInfo{};
    renderPassBegin.renderPass = flyweight_->getRenderPass();
    renderPassBegin.framebuffer = framebuffer;
    renderPassBegin.renderArea.offset.x = 0;
    renderPassBegin.renderArea.offset.y = 0;
    renderPassBegin.renderArea.extent.width = window.getSwapchainSize()[0];
    renderPassBegin.renderArea.extent.height = window.getSwapchainSize()[1];
    renderPassBegin.clearValueCount = 1;
    renderPassBegin.pClearValues = &clearValue;
    auto viewport = vk::Viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = window.getSwapchainSize()[0];
    viewport.height = window.getSwapchainSize()[1];
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    auto scissor = vk::Rect2D{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = window.getSwapchainSize()[0];
    scissor.extent.height = window.getSwapchainSize()[1];
    auto camera = scene.getCamera();
    auto invProjection = inverse(camera->getProjectionMatrix());
    auto invView = camera->getTransform().getMatrix();
    auto frustumCorners = std::array{
        Vector4f{-1.0f, -1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, -1.0f, 1.0f, 1.0f},
        Vector4f{-1.0f, 1.0f, 1.0f, 1.0f},
        Vector4f{1.0f, 1.0f, 1.0f, 1.0f}};
    for (auto &frustumCorner : frustumCorners) {
      frustumCorner = invProjection * frustumCorner;
      frustumCorner /= frustumCorner[3];
      frustumCorner = invView * frustumCorner;
    }
    auto cameraPosition = camera->getTransform().getTranslation();
    auto pushConstants = std::array<char, 80>{};
    std::memcpy(pushConstants.data() + 0, &frustumCorners, 64);
    std::memcpy(pushConstants.data() + 64, &cameraPosition, 12);
    std::memcpy(pushConstants.data() + 76, &seed, 4);
    commandBuffer_->beginRenderPass(
        renderPassBegin, vk::SubpassContents::eInline);
    commandBuffer_->bindPipeline(
        vk::PipelineBindPoint::eGraphics, flyweight_->getPipeline());
    commandBuffer_->setViewport(0, viewport);
    commandBuffer_->setScissor(0, scissor);
    commandBuffer_->bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        flyweight_->getPipelineLayout(),
        0,
        {transmittanceLut_.getTextureDescriptorSet(),
         skyViewLut_.getTextureDescriptorSet()},
        {});
    commandBuffer_->pushConstants(
        flyweight_->getPipelineLayout(),
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0,
        84,
        pushConstants.data());
    commandBuffer_->draw(3, 1, 0, 0);
    commandBuffer_->endRenderPass();
  }

  vk::UniqueCommandPool Frame::createCommandPool() {
    auto &context = *flyweight_->getWindow()->getContext();
    auto createInfo = vk::CommandPoolCreateInfo{};
    createInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    createInfo.queueFamilyIndex = context.getGraphicsFamily();
    return context.getDevice().createCommandPoolUnique(createInfo);
  }

  vk::UniqueCommandBuffer Frame::allocateCommandBuffer() {
    auto &context = *flyweight_->getWindow()->getContext();
    auto allocateInfo = vk::CommandBufferAllocateInfo{};
    allocateInfo.commandPool = *commandPool_;
    allocateInfo.level = vk::CommandBufferLevel::ePrimary;
    allocateInfo.commandBufferCount = 1;
    return std::move(
        context.getDevice().allocateCommandBuffersUnique(allocateInfo)[0]);
  }
} // namespace imp