#include "GpuContext.h"

#include <optional>
#include <string>
#include <unordered_set>

#include <GLFW/glfw3.h>

namespace imp {
  GpuContext::GpuContext(GpuContextCreateInfo const &createInfo):
      validationEnabled_{createInfo.validation},
      presentationEnabled_{createInfo.presentation},
      instance_{createInstance()},
      physicalDevice_{selectPhysicalDevice()},
      graphicsFamily_{selectGraphicsFamily()},
      transferFamily_{selectTransferFamily()},
      presentFamily_{selectPresentFamily()},
      device_{createDevice()},
      graphicsQueue_{selectGraphicsQueue()},
      transferQueue_{selectTransferQueue()},
      presentQueue_{selectPresentQueue()},
      allocator_{createAllocator()} {}

  GpuContext::~GpuContext() {
    device_->waitIdle();
    vmaDestroyAllocator(allocator_);
  }

  vk::UniqueInstance GpuContext::createInstance() {
    auto app_info = vk::ApplicationInfo{};
    app_info.pApplicationName = "imp";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "imp";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;
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
    auto queue_families = physicalDevice_.getQueueFamilyProperties();
    for (auto i = std::uint32_t{0}; i < queue_families.size(); ++i) {
      if ((queue_families[i].queueFlags &
           (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) ==
          (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) {
        return i;
      }
    }
    throw std::runtime_error{"failed to select graphics family."};
  }

  std::uint32_t GpuContext::selectTransferFamily() {
    auto queue_families = physicalDevice_.getQueueFamilyProperties();
    for (auto i = std::uint32_t{0}; i < queue_families.size(); ++i) {
      if ((queue_families[i].queueFlags &
           (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute |
            vk::QueueFlagBits::eTransfer)) == vk::QueueFlagBits::eTransfer) {
        return i;
      }
    }
    return graphicsFamily_;
  }

  std::uint32_t GpuContext::selectPresentFamily() {
    if (!presentationEnabled_) {
      return 0xffffffff;
    }
    if (glfwGetPhysicalDevicePresentationSupport(
            *instance_, physicalDevice_, graphicsFamily_)) {
      return graphicsFamily_;
    }
    auto queue_families = physicalDevice_.getQueueFamilyProperties();
    for (auto i = std::uint32_t{0}; i < queue_families.size(); ++i) {
      if (glfwGetPhysicalDevicePresentationSupport(
              *instance_, physicalDevice_, i)) {
        return i;
      }
    }
    throw std::runtime_error{"failed to select present family."};
  }

  vk::UniqueDevice GpuContext::createDevice() {
    auto queue_families = std::unordered_set<uint32_t>{};
    queue_families.emplace(graphicsFamily_);
    queue_families.emplace(transferFamily_);
    if (presentationEnabled_) {
      queue_families.emplace(presentFamily_);
    }
    auto queue_create_infos = std::vector<vk::DeviceQueueCreateInfo>{};
    auto queue_priority = 1.0f;
    for (auto queue_family : queue_families) {
      auto queue_create_info = vk::DeviceQueueCreateInfo{};
      queue_create_info.queueFamilyIndex = queue_family;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &queue_priority;
      queue_create_infos.emplace_back(queue_create_info);
    }
    auto extensions = std::vector<char const *>{};
    if (presentationEnabled_) {
      extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    auto features = vk::PhysicalDeviceFeatures{};
    features.shaderSampledImageArrayDynamicIndexing = true;
    auto create_info = vk::DeviceCreateInfo{};
    create_info.queueCreateInfoCount =
        static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.pEnabledFeatures = &features;
    return physicalDevice_.createDeviceUnique(create_info);
  }

  vk::Queue GpuContext::selectGraphicsQueue() {
    return device_->getQueue(graphicsFamily_, 0);
  }

  vk::Queue GpuContext::selectTransferQueue() {
    return device_->getQueue(transferFamily_, 0);
  }

  vk::Queue GpuContext::selectPresentQueue() {
    return presentationEnabled_ ? device_->getQueue(presentFamily_, 0)
                                : vk::Queue{};
  }

  VmaAllocator GpuContext::createAllocator() {
    auto info = VmaAllocatorCreateInfo{};
    info.physicalDevice = physicalDevice_;
    info.device = *device_;
    info.instance = *instance_;
    info.vulkanApiVersion = VK_API_VERSION_1_1;
    auto allocator = VmaAllocator{};
    if (vmaCreateAllocator(&info, &allocator)) {
      throw std::runtime_error{"failed to create vulkan allocator."};
    }
    return allocator;
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

  std::uint32_t GpuContext::getTransferFamily() const noexcept {
    return transferFamily_;
  }

  std::uint32_t GpuContext::getPresentFamily() const noexcept {
    return presentFamily_;
  }

  vk::Device GpuContext::getDevice() const noexcept {
    return *device_;
  }

  vk::Queue GpuContext::getGraphicsQueue() const noexcept {
    return graphicsQueue_;
  }

  vk::Queue GpuContext::getTransferQueue() const noexcept {
    return transferQueue_;
  }

  vk::Queue GpuContext::getPresentQueue() const noexcept {
    return presentQueue_;
  }

  GpuBuffer GpuContext::createBuffer(
      vk::BufferCreateInfo const &buffer_info,
      VmaAllocationCreateInfo const &allocation_info) {
    return GpuBuffer{buffer_info, allocation_info, allocator_};
  }

  GpuImage GpuContext::createImage(
      vk::ImageCreateInfo const &image_info,
      VmaAllocationCreateInfo const &allocation_info) {
    return GpuImage{image_info, allocation_info, allocator_};
  }

  // vk::Sampler
  // GpuContext::create_sampler(vk::SamplerCreateInfo const &create_info) {
  //  if (auto it = samplers_.find(create_info); it != samplers_.end()) {
  //    return *it->second;
  //  }
  //  return *samplers_
  //              .emplace(create_info,
  //              device_->createSamplerUnique(create_info)) .first->second;
  //}
} // namespace imp