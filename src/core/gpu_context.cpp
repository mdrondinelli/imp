#include "gpu_context.h"

#include <optional>
#include <string>
#include <unordered_set>

#include <GLFW/glfw3.h>

namespace imp {
  gpu_context::gpu_context(bool validation_enabled, bool presentation_enabled):
      validation_enabled_{validation_enabled},
      presentation_enabled_{presentation_enabled},
      instance_{create_instance()},
      physical_device_{select_physical_device()},
      graphics_family_{select_graphics_family()},
      //compute_family_{select_compute_family()},
      present_family_{select_present_family()},
      device_{create_device()},
      graphics_queue_{select_graphics_queue()},
      //compute_queue_{select_compute_queue()},
      present_queue_{select_present_queue()},
      allocator_{create_allocator()} {}

  gpu_context::~gpu_context() {
    device_->waitIdle();
    vmaDestroyAllocator(allocator_);
  }

  vk::UniqueInstance gpu_context::create_instance() {
    auto app_info = vk::ApplicationInfo{};
    app_info.pApplicationName = "imp";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "imp";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;
    auto layers = std::vector<char const *>{};
    if (validation_enabled_) {
      layers.emplace_back("VK_LAYER_KHRONOS_validation");
    }
    auto extensions = std::vector<char const *>{};
    if (presentation_enabled_) {
      auto glfw_extension_count = uint32_t{};
      auto glfw_extensions =
          glfwGetRequiredInstanceExtensions(&glfw_extension_count);
      extensions.insert(
          extensions.end(),
          glfw_extensions,
          glfw_extensions + glfw_extension_count);
    }
    auto instance_info = vk::InstanceCreateInfo{};
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instance_info.ppEnabledLayerNames = layers.data();
    instance_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();
    return vk::createInstanceUnique(instance_info);
  }

  vk::PhysicalDevice gpu_context::select_physical_device() {
    auto physical_devices = std::vector<vk::PhysicalDevice>{};
    for (auto physical_device : instance_->enumeratePhysicalDevices()) {
      auto properties = physical_device.getProperties();
      if (properties.apiVersion < VK_API_VERSION_1_1) {
        continue;
      }
      auto extensions = physical_device.enumerateDeviceExtensionProperties();
      auto missing_extensions =
          std::unordered_set<std::string>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
      for (auto &extension : extensions) {
        missing_extensions.erase(extension.extensionName);
      }
      if (!missing_extensions.empty()) {
        continue;
      }
      auto features = physical_device.getFeatures();
      if (!features.shaderSampledImageArrayDynamicIndexing) {
        continue;
      }
      auto queue_families = physical_device.getQueueFamilyProperties();
      auto graphics_family = false;
      auto compute_family = false;
      for (auto i = uint32_t{0}; i < queue_families.size(); ++i) {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
          graphics_family = true;
        }
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eCompute) {
          compute_family = true;
        }
      }
      if (!graphics_family || !compute_family) {
        continue;
      }
      if (presentation_enabled_) {
        auto present_family = false;
        for (auto i = uint32_t{0}; i < queue_families.size(); ++i) {
          if (glfwGetPhysicalDevicePresentationSupport(
                  *instance_, physical_device, i)) {
            present_family = true;
          }
        }
        if (!present_family) {
          continue;
        }
      }
      physical_devices.emplace_back(physical_device);
    }
    if (physical_devices.empty()) {
      throw std::runtime_error{"Failed to find vulkan physical device."};
    }
    auto physical_device = physical_devices[0];
    for (auto i = size_t{1}; i < physical_devices.size(); ++i) {
      auto best_type = physical_device.getProperties().deviceType;
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
      if (priority(best_type) < priority(curr_type)) {
        physical_device = physical_devices[i];
      }
    }
    return physical_device;
  }

  uint32_t gpu_context::select_graphics_family() {
    constexpr auto const flags =
        vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute;
    auto queue_families = physical_device_.getQueueFamilyProperties();
    for (auto i = uint32_t{0}; i < queue_families.size(); ++i) {
      if ((queue_families[i].queueFlags & flags) == flags) {
        return i;
      }
    }
    throw std::runtime_error{"failed to select graphics family."};
  }

  // uint32_t gpu_context::select_compute_family() {
  //  auto queue_families = physical_device_.getQueueFamilyProperties();
  //  for (auto i = uint32_t{0}; i < queue_families.size(); ++i) {
  //    if (queue_families[i].queueFlags & vk::QueueFlagBits::eCompute) {
  //      return i;
  //    }
  //  }
  //  throw std::runtime_error{"failed to select compute family."};
  //}

  uint32_t gpu_context::select_present_family() {
    if (!presentation_enabled_) {
      return 0xffffffff;
    }
    auto queue_families = physical_device_.getQueueFamilyProperties();
    for (auto i = uint32_t{0}; i < queue_families.size(); ++i) {
      if (glfwGetPhysicalDevicePresentationSupport(
              *instance_, physical_device_, i)) {
        return i;
      }
    }
    throw std::runtime_error{"failed to select present family."};
  }

  vk::UniqueDevice gpu_context::create_device() {
    auto queue_families = std::unordered_set{graphics_family_/*, compute_family_*/};
    if (presentation_enabled_) {
      queue_families.emplace(present_family_);
    }
    auto queue_priority = 1.0f;
    auto queue_infos = std::vector<vk::DeviceQueueCreateInfo>{};
    for (auto queue_family : queue_families) {
      auto queue_info = vk::DeviceQueueCreateInfo{};
      queue_info.queueFamilyIndex = queue_family;
      queue_info.queueCount = 1;
      queue_info.pQueuePriorities = &queue_priority;
      queue_infos.emplace_back(queue_info);
    }
    auto extensions = std::vector<char const *>{};
    if (presentation_enabled_) {
      extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    auto device_info = vk::DeviceCreateInfo{};
    device_info.queueCreateInfoCount =
        static_cast<uint32_t>(queue_infos.size());
    device_info.pQueueCreateInfos = queue_infos.data();
    device_info.enabledExtensionCount =
        static_cast<uint32_t>(extensions.size());
    device_info.ppEnabledExtensionNames = extensions.data();
    return physical_device_.createDeviceUnique(device_info);
  }

  vk::Queue gpu_context::select_graphics_queue() {
    return device_->getQueue(graphics_family_, 0);
  }

  //vk::Queue gpu_context::select_compute_queue() {
  //  return device_->getQueue(compute_family_, 0);
  //}

  vk::Queue gpu_context::select_present_queue() {
    return presentation_enabled_ ? device_->getQueue(present_family_, 0)
                                 : vk::Queue{};
  }

  VmaAllocator gpu_context::create_allocator() {
    auto info = VmaAllocatorCreateInfo{};
    info.physicalDevice = physical_device_;
    info.device = *device_;
    info.instance = *instance_;
    info.vulkanApiVersion = VK_API_VERSION_1_1;
    auto allocator = VmaAllocator{};
    if (vmaCreateAllocator(&info, &allocator) != VK_SUCCESS) {
      throw std::runtime_error{"failed to create vulkan allocator."};
    }
    return allocator;
  }

  bool gpu_context::validation_enabled() const noexcept {
    return validation_enabled_;
  }

  bool gpu_context::presentation_enabled() const noexcept {
    return presentation_enabled_;
  }

  vk::Instance gpu_context::instance() const noexcept {
    return *instance_;
  }

  vk::PhysicalDevice gpu_context::physical_device() const noexcept {
    return physical_device_;
  }

  uint32_t gpu_context::graphics_family() const noexcept {
    return graphics_family_;
  }

  //uint32_t gpu_context::compute_family() const noexcept {
  //  return compute_family_;
  //}

  uint32_t gpu_context::present_family() const noexcept {
    return present_family_;
  }

  vk::Device gpu_context::device() const noexcept {
    return *device_;
  }

  vk::Queue gpu_context::graphics_queue() const noexcept {
    return graphics_queue_;
  }

  //vk::Queue gpu_context::compute_queue() const noexcept {
  //  return compute_queue_;
  //}

  vk::Queue gpu_context::present_queue() const noexcept {
    return present_queue_;
  }

  vk::Sampler
  gpu_context::create_sampler(vk::SamplerCreateInfo const &create_info) {
    if (auto it = samplers_.find(create_info); it != samplers_.end()) {
      return *it->second;
    }
    return *samplers_
                .emplace(create_info, device_->createSamplerUnique(create_info))
                .first->second;
  }

  gpu_buffer gpu_context::create_buffer(
      vk::BufferCreateInfo const &buffer_info,
      VmaAllocationCreateInfo const &allocation_info) {
    return gpu_buffer{buffer_info, allocation_info, allocator_};
  }

  gpu_image gpu_context::create_image(
      vk::ImageCreateInfo const &image_info,
      VmaAllocationCreateInfo const &allocation_info) {
    return gpu_image{image_info, allocation_info, allocator_};
  }
} // namespace imp