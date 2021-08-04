#include "GpuContext.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <GLFW/glfw3.h>

namespace imp {
  GpuContext::GpuContext(GpuContextCreateInfo const &createInfo):
      validationEnabled_{createInfo.validation},
      presentationEnabled_{createInfo.presentation},
      instance_{createInstance()},
      physicalDevice_{selectPhysicalDevice()},
      graphicsFamily_{selectGraphicsFamily()},
      computeFamily_{selectComputeFamily()},
      transferFamily_{selectTransferFamily()},
      presentFamily_{selectPresentFamily()},
      device_{createDevice()},
      graphicsQueue_{selectGraphicsQueue()},
      computeQueue_{selectComputeQueue()},
      transferQueue_{selectTransferQueue()},
      presentQueue_{selectPresentQueue()},
      allocator_{createAllocator()},
      renderPasses_{*device_},
      descriptorSetLayouts_{*device_},
      pipelineLayouts_{*device_},
      samplers_{*device_} {}

  GpuContext::~GpuContext() {
    device_->waitIdle();
    vmaDestroyAllocator(allocator_);
  }

  bool GpuContext::isValidationEnabled() const noexcept {
    return validationEnabled_;
  }

  bool GpuContext::isPresentationEnabled() const noexcept {
    return presentationEnabled_;
  }

  vk::Instance GpuContext::getInstance() const noexcept {
    return *instance_;
  }

  vk::PhysicalDevice GpuContext::getPhysicalDevice() const noexcept {
    return physicalDevice_;
  }

  std::uint32_t GpuContext::getGraphicsFamily() const noexcept {
    return graphicsFamily_;
  }

  std::uint32_t GpuContext::getComputeFamily() const noexcept {
    return computeFamily_;
  }

  std::optional<std::uint32_t> GpuContext::getTransferFamily() const noexcept {
    return transferFamily_;
  }

  std::optional<std::uint32_t> GpuContext::getPresentFamily() const noexcept {
    return presentFamily_;
  }

  vk::Device GpuContext::getDevice() const noexcept {
    return *device_;
  }

  vk::Queue GpuContext::getGraphicsQueue() const noexcept {
    return graphicsQueue_;
  }

  vk::Queue GpuContext::getComputeQueue() const noexcept {
    return computeQueue_;
  }

  vk::Queue GpuContext::getTransferQueue() const noexcept {
    return transferQueue_;
  }

  vk::Queue GpuContext::getPresentQueue() const noexcept {
    return presentQueue_;
  }

  gsl::not_null<VmaAllocator> GpuContext::getAllocator() const noexcept {
    return allocator_;
  }

  vk::RenderPass
  GpuContext::createRenderPass(GpuRenderPassCreateInfo const &createInfo) {
    return renderPasses_.create(createInfo);
  }

  vk::DescriptorSetLayout GpuContext::createDescriptorSetLayout(
      GpuDescriptorSetLayoutCreateInfo const &createInfo) {
    return descriptorSetLayouts_.create(createInfo);
  }

  vk::PipelineLayout GpuContext::createPipelineLayout(
      GpuPipelineLayoutCreateInfo const &createInfo) {
    return pipelineLayouts_.create(createInfo);
  }

  vk::Sampler
  GpuContext::createSampler(GpuSamplerCreateInfo const &createInfo) {
    return samplers_.create(createInfo);
  }

  vk::UniqueInstance GpuContext::createInstance() {
    auto app_info = vk::ApplicationInfo{};
    app_info.pApplicationName = "dream";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "dream";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;
    auto layers = std::vector<char const *>{};
    if (validationEnabled_) {
      layers.emplace_back("VK_LAYER_KHRONOS_validation");
    }
    auto extensions = std::vector<char const *>{};
    if (presentationEnabled_) {
      auto glfw_extension_count = uint32_t{};
      auto glfw_extensions =
          glfwGetRequiredInstanceExtensions(&glfw_extension_count);
      extensions.insert(
          extensions.end(),
          glfw_extensions,
          glfw_extensions + glfw_extension_count);
    }
    auto create_info = vk::InstanceCreateInfo{};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    create_info.ppEnabledLayerNames = layers.data();
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    return vk::createInstanceUnique(create_info);
  }

  vk::PhysicalDevice GpuContext::selectPhysicalDevice() {
    auto required_extensions = std::unordered_set<std::string>{};
    if (presentationEnabled_) {
      required_extensions.emplace(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    auto physical_devices = std::vector<vk::PhysicalDevice>{};
    for (auto pd : instance_->enumeratePhysicalDevices()) {
      auto properties = pd.getProperties();
      if (properties.apiVersion < VK_API_VERSION_1_1) {
        continue;
      }
      auto features = pd.getFeatures();
      if (!features.shaderSampledImageArrayDynamicIndexing) {
        continue;
      }
      auto extensions = pd.enumerateDeviceExtensionProperties();
      auto missing_extensions = required_extensions;
      for (auto &extension : extensions) {
        missing_extensions.erase(extension.extensionName);
      }
      if (!missing_extensions.empty()) {
        continue;
      }
      auto queue_families = pd.getQueueFamilyProperties();
      auto has_graphics = false;
      for (auto &qf : queue_families) {
        if (qf.queueFlags & vk::QueueFlagBits::eGraphics) {
          has_graphics = true;
          break;
        }
      }
      if (!has_graphics) {
        continue;
      }
      if (presentationEnabled_) {
        auto has_presentation = false;
        for (auto i = uint32_t{}; i < queue_families.size(); ++i) {
          if (glfwGetPhysicalDevicePresentationSupport(*instance_, pd, i)) {
            has_presentation = true;
            break;
          }
        }
        if (!has_presentation) {
          continue;
        }
      }
      physical_devices.emplace_back(pd);
    }
    if (physical_devices.empty()) {
      throw std::runtime_error{"failed to select vulkan device."};
    }
    auto best_pd = physical_devices[0];
    auto best_type = physical_devices[0].getProperties().deviceType;
    for (auto i = size_t{1}; i < physical_devices.size(); ++i) {
      auto curr_pd = physical_devices[i];
      auto curr_type = physical_devices[i].getProperties().deviceType;
      auto priority = [](vk::PhysicalDeviceType type) {
        switch (type) {
        default:
        case vk::PhysicalDeviceType::eOther:
          return 0;
        case vk::PhysicalDeviceType::eCpu:
          return 1;
        case vk::PhysicalDeviceType::eVirtualGpu:
          return 2;
        case vk::PhysicalDeviceType::eIntegratedGpu:
          return 3;
        case vk::PhysicalDeviceType::eDiscreteGpu:
          return 4;
        }
      };
      if (priority(curr_type) > priority(best_type)) {
        best_pd = curr_pd;
        best_type = curr_type;
      }
    }
    return best_pd;
  }

  std::uint32_t GpuContext::selectGraphicsFamily() {
    auto families = physicalDevice_.getQueueFamilyProperties();
    for (auto i = std::uint32_t{0}; i < families.size(); ++i) {
      if ((families[i].queueFlags &
           (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) ==
          (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) {
        return i;
      }
    }
    throw std::runtime_error{"failed to select graphics family."};
  }

  std::uint32_t GpuContext::selectComputeFamily() {
    auto families = physicalDevice_.getQueueFamilyProperties();
    if (families[graphicsFamily_].queueCount == 1) {
      for (auto i = std::uint32_t{0}; i < families.size(); ++i) {
        if (i != graphicsFamily_ &&
            families[i].queueFlags & vk::QueueFlagBits::eCompute) {
          return i;
        }
      }
    }
    return graphicsFamily_;
  }

  std::optional<std::uint32_t> GpuContext::selectTransferFamily() {
    auto families = physicalDevice_.getQueueFamilyProperties();
    for (auto i = std::uint32_t{0}; i < families.size(); ++i) {
      if ((families[i].queueFlags &
           (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute |
            vk::QueueFlagBits::eTransfer)) == vk::QueueFlagBits::eTransfer) {
        return i;
      }
    }
    return std::nullopt;
  }

  std::optional<std::uint32_t> GpuContext::selectPresentFamily() {
    if (presentationEnabled_) {
      if (glfwGetPhysicalDevicePresentationSupport(
              *instance_, physicalDevice_, graphicsFamily_)) {
        return graphicsFamily_;
      }
      auto families = physicalDevice_.getQueueFamilyProperties();
      for (auto i = std::uint32_t{0}; i < families.size(); ++i) {
        if (glfwGetPhysicalDevicePresentationSupport(
                *instance_, physicalDevice_, i)) {
          return i;
        }
      }
    }
    return std::nullopt;
  }

  vk::UniqueDevice GpuContext::createDevice() {
    auto queueCounts = std::unordered_map<std::uint32_t, std::uint32_t>{};
    auto maxQueueCounts = 0u;
    maxQueueCounts = std::max(maxQueueCounts, ++queueCounts[graphicsFamily_]);
    maxQueueCounts = std::max(maxQueueCounts, ++queueCounts[computeFamily_]);
    if (transferFamily_) {
      maxQueueCounts =
          std::max(maxQueueCounts, ++queueCounts[*transferFamily_]);
    }
    if (presentFamily_ && *presentFamily_ != graphicsFamily_) {
      maxQueueCounts = std::max(maxQueueCounts, ++queueCounts[*presentFamily_]);
    }
    auto queuePriorities = std::vector<float>(maxQueueCounts, 1.0f);
    auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>{};
    for (auto [queueFamilyIndex, queueCount] : queueCounts) {
      auto queueCreateInfo = vk::DeviceQueueCreateInfo{};
      queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
      queueCreateInfo.queueCount = queueCount;
      queueCreateInfo.pQueuePriorities = queuePriorities.data();
      queueCreateInfos.emplace_back(queueCreateInfo);
    }
    auto extensions = std::vector<char const *>{};
    if (presentationEnabled_) {
      extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    auto features = vk::PhysicalDeviceFeatures{};
    features.shaderSampledImageArrayDynamicIndexing = true;
    auto create_info = vk::DeviceCreateInfo{};
    create_info.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    create_info.pQueueCreateInfos = queueCreateInfos.data();
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.pEnabledFeatures = &features;
    return physicalDevice_.createDeviceUnique(create_info);
  }

  vk::Queue GpuContext::selectGraphicsQueue() {
    return device_->getQueue(graphicsFamily_, 0);
  }

  vk::Queue GpuContext::selectComputeQueue() {
    return device_->getQueue(computeFamily_, graphicsFamily_ == computeFamily_);
  }

  vk::Queue GpuContext::selectTransferQueue() {
    return transferFamily_ ? device_->getQueue(*transferFamily_, 0)
                           : vk::Queue{};
  }

  vk::Queue GpuContext::selectPresentQueue() {
    return presentFamily_ ? device_->getQueue(*presentFamily_, 0) : vk::Queue{};
  }

  gsl::not_null<VmaAllocator> GpuContext::createAllocator() {
    auto info = VmaAllocatorCreateInfo{};
    info.physicalDevice = physicalDevice_;
    info.device = *device_;
    info.instance = *instance_;
    info.vulkanApiVersion = VK_API_VERSION_1_1;
    auto allocator = VmaAllocator{};
    if (vmaCreateAllocator(&info, &allocator)) {
      throw std::runtime_error{"failed to create vulkan allocator."};
    }
    return gsl::not_null{allocator};
  }
} // namespace imp