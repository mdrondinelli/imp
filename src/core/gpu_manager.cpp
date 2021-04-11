#include "gpu_manager.h"

#include <optional>
#include <string>
#include <unordered_set>

#include <GLFW/glfw3.h>

namespace imp {
  gpu_manager::gpu_manager(gpu_manager_create_info const &create_info):
      validation_enabled_{create_info.validation_enabled},
      presentation_enabled_{create_info.presentation_enabled},
      instance_{create_instance()},
      physical_device_{select_physical_device()},
      logical_device_{create_logical_device()},
      allocator_{create_allocator()} {}

  vk::UniqueInstance gpu_manager::create_instance() {
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

  gpu_manager::physical_device_info gpu_manager::select_physical_device() {
    auto physical_devices = instance_->enumeratePhysicalDevices();
    if (physical_devices.empty()) {
      throw std::runtime_error{"Failed to find vulkan physical device."};
    }
    auto best_physical_device = vk::PhysicalDevice{};
    auto best_graphics_family = uint32_t{};
    auto best_compute_family = uint32_t{};
    auto best_present_family = uint32_t{};
    for (auto curr_physical_device : physical_devices) {
      auto properties = curr_physical_device.getProperties();
      if (properties.apiVersion < VK_API_VERSION_1_1) {
        continue;
      }
      auto available_extensions =
          curr_physical_device.enumerateDeviceExtensionProperties();
      auto required_extensions =
          std::unordered_set<std::string>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
      for (auto &available_extension : available_extensions) {
        required_extensions.erase(available_extension.extensionName);
      }
      if (!required_extensions.empty()) {
        continue;
      }
      auto features = curr_physical_device.getFeatures();
      if (!features.shaderSampledImageArrayDynamicIndexing) {
        continue;
      }
      auto queue_families = curr_physical_device.getQueueFamilyProperties();
      auto graphics_family = std::optional<uint32_t>{};
      auto compute_family = std::optional<uint32_t>{};
      auto present_family = std::optional<uint32_t>{};
      for (auto i = uint32_t{0}; i < queue_families.size(); ++i) {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
          graphics_family = i;
        }
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eCompute) {
          compute_family = i;
        }
        if (graphics_family && compute_family) {
          break;
        }
      }
      if (presentation_enabled_) {
        for (auto i = uint32_t{0}; i < queue_families.size(); ++i) {
          if (glfwGetPhysicalDevicePresentationSupport(
                  *instance_, curr_physical_device, i)) {
            present_family = i;
            break;
          }
        }
        if (!present_family) {
          continue;
        }
      }
      if (!graphics_family || !compute_family) {
        continue;
      }
      if (best_physical_device) {
        auto best_type = best_physical_device.getProperties().deviceType;
        auto curr_type = curr_physical_device.getProperties().deviceType;
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
        if (priority(best_type) > priority(curr_type)) {
          continue;
        }
      }
      best_physical_device = curr_physical_device;
      best_graphics_family = *graphics_family;
      best_compute_family = *compute_family;
      best_present_family = present_family ? *present_family : 0xffffffff;
    }
    if (!best_physical_device) {
      throw std::runtime_error{"Failed to select vulkan physical device."};
    }
    auto physical_device = physical_device_info{};
    physical_device.handle = best_physical_device;
    physical_device.graphics_family = best_graphics_family;
    physical_device.compute_family = best_compute_family;
    physical_device.present_family = best_present_family;
    return physical_device;
  }

  gpu_manager::logical_device_info gpu_manager::create_logical_device() {
    auto queue_families = std::unordered_set{
        physical_device_.graphics_family, physical_device_.compute_family};
    if (presentation_enabled_) {
      queue_families.emplace(physical_device_.present_family);
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
    auto logical_device = logical_device_info{};
    logical_device.handle =
        physical_device_.handle.createDeviceUnique(device_info);
    logical_device.graphics_queue =
        logical_device.handle->getQueue(physical_device_.graphics_family, 0);
    logical_device.compute_queue =
        logical_device.handle->getQueue(physical_device_.compute_family, 0);
    logical_device.present_queue = presentation_enabled_
                                       ? logical_device.handle->getQueue(
                                             physical_device_.present_family, 0)
                                       : vk::Queue{};
    return logical_device;
  }

  gpu_allocator gpu_manager::create_allocator() {
    auto info = gpu_allocator_create_info{};
    info.instance = instance();
    info.physical_device = physical_device();
    info.device = logical_device();
    return gpu_allocator{info};
  }

  bool gpu_manager::validation_enabled() const noexcept {
    return validation_enabled_;
  }

  bool gpu_manager::presentation_enabled() const noexcept {
    return presentation_enabled_;
  }

  vk::Instance gpu_manager::instance() const noexcept {
    return *instance_;
  }

  vk::PhysicalDevice gpu_manager::physical_device() const noexcept {
    return physical_device_.handle;
  }

  uint32_t gpu_manager::graphics_family() const noexcept {
    return physical_device_.graphics_family;
  }

  uint32_t gpu_manager::compute_family() const noexcept {
    return physical_device_.compute_family;
  }

  uint32_t gpu_manager::present_family() const noexcept {
    return physical_device_.present_family;
  }

  vk::Device gpu_manager::logical_device() const noexcept {
    return *logical_device_.handle;
  }

  vk::Queue gpu_manager::graphics_queue() const noexcept {
    return logical_device_.graphics_queue;
  }

  vk::Queue gpu_manager::compute_queue() const noexcept {
    return logical_device_.compute_queue;
  }

  vk::Queue gpu_manager::present_queue() const noexcept {
    return logical_device_.present_queue;
  }

  gpu_allocator const &gpu_manager::allocator() const noexcept {
    return allocator_;
  }

  gpu_allocator &gpu_manager::allocator() noexcept {
    return allocator_;
  }
} // namespace imp