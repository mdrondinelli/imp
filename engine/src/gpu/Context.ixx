// clang-format off
module;
#include <vulkan/vulkan.hpp>
export module mobula.gpu:Context;
import <optional>;
import :Allocator;
import :DescriptorSetLayoutCache;
import :PipelineCache;
import :PipelineLayoutCache;
import :RenderPassCache;
import :SamplerCache;
// clang-format on

namespace mobula {
  namespace gpu {
    export class Context {
    public:
      explicit Context(bool validationEnabled);

      /**
       * \return A reference to this context's render pass cache.
       */
      RenderPassCache &getRenderPasses() noexcept {
        return renderPasses_;
      }

      /**
       * \return A reference to this context's descriptor set layout cache.
       */
      DescriptorSetLayoutCache &getDescriptorSetLayouts() noexcept {
        return descriptorSetLayouts_;
      }

      /**
       * \return A reference to this context's pipeline layout cache.
       */
      PipelineLayoutCache &getPipelineLayouts() noexcept {
        return pipelineLayouts_;
      }

      /**
       * \return A reference to this context's pipeline cache.
       */
      PipelineCache &getPipelines() noexcept {
        return pipelines_;
      }

      /**
       * \return A reference to this context's sampler cache.
       */
      SamplerCache &getSamplers() noexcept {
        return samplers_;
      }

      /**
       * \return A reference to this context's allocator.
       */
      Allocator &getAllocator() noexcept {
        return allocator_;
      }

    private:
      vk::UniqueInstance createInstance();
      vk::PhysicalDevice findPhysicalDevice();
      std::uint32_t findGraphicsFamily();
      std::uint32_t findComputeFamily();
      std::optional<std::uint32_t> findTransferFamily();
      vk::UniqueDevice createDevice();
      vk::Queue findGraphicsQueue();
      vk::Queue findComputeQueue();
      vk::Queue findHostToDeviceQueue();
      vk::Queue findDeviceToHostQueue();

      bool validationEnabled_;
      vk::UniqueInstance instance_;
      vk::PhysicalDevice physicalDevice_;
      std::uint32_t graphicsFamily_;
      std::uint32_t computeFamily_;
      std::optional<std::uint32_t> transferFamily_;
      vk::UniqueDevice device_;
      vk::Queue graphicsQueue_;
      vk::Queue computeQueue_;
      vk::Queue hostToDeviceQueue_;
      vk::Queue deviceToHostQueue_;
      RenderPassCache renderPasses_;
      DescriptorSetLayoutCache descriptorSetLayouts_;
      PipelineLayoutCache pipelineLayouts_;
      PipelineCache pipelines_;
      SamplerCache samplers_;
      Allocator allocator_;
    };
  } // namespace gpu
} // namespace mobula