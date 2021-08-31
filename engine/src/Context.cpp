// clang-format off
module;
#include <cassert>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>
module mobula.gpu;
import <algorithm>;
import <optional>;
import <unordered_map>;
import <unordered_set>;
// clang-format on

namespace mobula {
  namespace gpu {
    Context::Context(bool validationEnabled):
        validationEnabled_{validationEnabled},
        instance_{createInstance()},
        physicalDevice_{findPhysicalDevice()},
        graphicsFamily_{findGraphicsFamily()},
        computeFamily_{findComputeFamily()},
        transferFamily_{findTransferFamily()},
        device_{createDevice()},
        graphicsQueue_{findGraphicsQueue()},
        computeQueue_{findComputeQueue()},
        hostToDeviceQueue_{findHostToDeviceQueue()},
        deviceToHostQueue_{findDeviceToHostQueue()},
        renderPasses_{*device_},
        descriptorSetLayouts_{*device_},
        pipelineLayouts_{*device_},
        pipelines_{*device_},
        samplers_{*device_},
        allocator_{physicalDevice_, *device_, *instance_} {}

    vk::UniqueInstance Context::createInstance() {
      auto applicationInfo = vk::ApplicationInfo{};
      applicationInfo.pApplicationName = "mobula";
      applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      applicationInfo.apiVersion = VK_API_VERSION_1_2;
      auto layers = std::vector<char const *>{};
      if (validationEnabled_) {
        layers.emplace_back("VK_LAYER_KHRONOS_validation");
      }
      auto extensions = std::vector<char const *>{};
      auto glfwExtensionCount = std::uint32_t{};
      auto glfwExtensions =
          glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
      extensions.insert(
          extensions.end(),
          glfwExtensions,
          glfwExtensions + glfwExtensionCount);
      auto createInfo = vk::InstanceCreateInfo{};
      createInfo.pApplicationInfo = &applicationInfo;
      createInfo.enabledLayerCount = static_cast<std::uint32_t>(layers.size());
      createInfo.ppEnabledLayerNames = layers.data();
      createInfo.enabledExtensionCount =
          static_cast<std::uint32_t>(extensions.size());
      createInfo.ppEnabledExtensionNames = extensions.data();
      return vk::createInstanceUnique(createInfo);
    }

    vk::PhysicalDevice Context::findPhysicalDevice() {
      auto physicalDevices = instance_->enumeratePhysicalDevices();
      std::erase_if(physicalDevices, [this](vk::PhysicalDevice physicalDevice) {
        auto properties = physicalDevice.getProperties();
        if (properties.apiVersion < VK_API_VERSION_1_2) {
          return true;
        }
        auto features = physicalDevice.getFeatures();
        if (!features.samplerAnisotropy || !features.textureCompressionBC ||
            !features.shaderSampledImageArrayDynamicIndexing) {
          return true;
        }
        auto extensions = std::unordered_set<std::string>{};
        for (auto &extension :
             physicalDevice.enumerateDeviceExtensionProperties()) {
          extensions.emplace(extension.extensionName.data());
        }
        if (!extensions.contains("VK_KHR_swapchain") ||
            !extensions.contains("VK_KHR_acceleration_structure") ||
            !extensions.contains("VK_KHR_ray_tracing_pipeline")) {
          return true;
        }
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        auto hasGraphics = false;
        auto hasCompute = false;
        for (auto i = std::uint32_t{}; i < queueFamilies.size(); ++i) {
          if ((queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
              glfwGetPhysicalDevicePresentationSupport(
                  *instance_, physicalDevice, i)) {
            hasGraphics = true;
          }
          if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
            hasCompute = true;
          }
        }
        return !hasGraphics || !hasCompute;
      });
      auto value = [](vk::PhysicalDevice physicalDevice) {
        switch (physicalDevice.getProperties().deviceType) {
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
      if (physicalDevices.empty()) {
        throw std::runtime_error{"Failed to find a usable physical device."};
      }
      auto physicalDevice = physicalDevices[0];
      for (auto i = std::size_t{1}; i < physicalDevices.size(); ++i) {
        if (value(physicalDevice) < value(physicalDevices[i])) {
          physicalDevice = physicalDevices[i];
        }
      }
      return physicalDevice;
    }

    std::uint32_t Context::findGraphicsFamily() {
      auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
      for (auto i = std::uint32_t{}; i < queueFamilies.size(); ++i) {
        if ((queueFamilies[i].queueFlags &
             (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) ==
                (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute) &&
            glfwGetPhysicalDevicePresentationSupport(
                *instance_, physicalDevice_, i) &&
            queueFamilies[i].queueCount > 1) {
          return i;
        }
      }
      for (auto i = std::uint32_t{}; i < queueFamilies.size(); ++i) {
        if ((queueFamilies[i].queueFlags &
             (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) ==
                (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute) &&
            glfwGetPhysicalDevicePresentationSupport(
                *instance_, physicalDevice_, i)) {
          return i;
        }
      }
      for (auto i = std::uint32_t{}; i < queueFamilies.size(); ++i) {
        if ((queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
            glfwGetPhysicalDevicePresentationSupport(
                *instance_, physicalDevice_, i)) {
          return i;
        }
      }
      throw std::runtime_error{"Failed to find graphics family."};
    }

    std::uint32_t Context::findComputeFamily() {
      auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
      if ((queueFamilies[graphicsFamily_].queueFlags &
           vk::QueueFlagBits::eCompute)) {
        if (queueFamilies[graphicsFamily_].queueCount > 1) {
          return graphicsFamily_;
        }
        for (auto i = std::uint32_t{}; i < queueFamilies.size(); ++i) {
          if (i != graphicsFamily_ &&
              (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute)) {
            return i;
          }
        }
        return graphicsFamily_;
      } else {
        for (auto i = std::uint32_t{}; i < queueFamilies.size(); ++i) {
          if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
            return i;
          }
        }
        throw std::runtime_error{"Failed to find compute family."};
      }
    }

    std::optional<std::uint32_t> Context::findTransferFamily() {
      auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
      auto queueFamilyIndex = std::uint32_t{};
      auto queueCount = std::uint32_t{};
      for (auto i = std::uint32_t{}; i < queueFamilies.size(); ++i) {
        if ((queueFamilies[i].queueFlags &
             (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute |
              vk::QueueFlagBits::eTransfer)) == vk::QueueFlagBits::eTransfer &&
            queueFamilies[i].queueCount > queueCount) {
          queueFamilyIndex = i;
          queueCount = queueFamilies[i].queueCount;
        }
      }
      if (queueCount) {
        return queueFamilyIndex;
      }
      return std::nullopt;
    }

    vk::UniqueDevice Context::createDevice() {
      auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
      auto queueCounts = std::unordered_map<std::uint32_t, std::uint32_t>{};
      ++queueCounts[graphicsFamily_];
      if (queueCounts[computeFamily_] <
          queueFamilies[computeFamily_].queueCount) {
        ++queueCounts[computeFamily_];
      }
      if (transferFamily_) {
        queueCounts[*transferFamily_] = std::min(
            queueFamilies[*transferFamily_].queueCount, std::uint32_t{2});
      }
      auto maxQueueCount = std::uint32_t{};
      for (auto [_, queueCount] : queueCounts) {
        maxQueueCount = std::max(maxQueueCount, queueCount);
      }
      auto queuePriorities = std::vector<float>(maxQueueCount, 1.0f);
      auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>{};
      for (auto [queueFamilyIndex, queueCount] : queueCounts) {
        auto &createInfo = queueCreateInfos.emplace_back();
        createInfo.queueFamilyIndex = queueFamilyIndex;
        createInfo.queueCount = queueCount;
        createInfo.pQueuePriorities = queuePriorities.data();
      }
      auto extensions = std::vector{
          "VK_KHR_swapchain",
          "VK_KHR_acceleration_structure",
          "VK_KHR_ray_tracing_pipeline"};
      auto features = vk::PhysicalDeviceFeatures{};
      features.samplerAnisotropy = true;
      features.textureCompressionBC = true;
      features.shaderSampledImageArrayDynamicIndexing = true;
      auto createInfo = vk::DeviceCreateInfo{};
      createInfo.queueCreateInfoCount =
          static_cast<std::uint32_t>(queueCreateInfos.size());
      createInfo.pQueueCreateInfos = queueCreateInfos.data();
      createInfo.enabledExtensionCount =
          static_cast<std::uint32_t>(extensions.size());
      createInfo.ppEnabledExtensionNames = extensions.data();
      createInfo.pEnabledFeatures = &features;
      return physicalDevice_.createDeviceUnique(createInfo);
    }

    vk::Queue Context::findGraphicsQueue() {
      return device_->getQueue(graphicsFamily_, 0);
    }

    vk::Queue Context::findComputeQueue() {
      auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
      return device_->getQueue(
          computeFamily_,
          computeFamily_ == graphicsFamily_ &&
              queueFamilies[computeFamily_].queueCount > 1);
    }

    vk::Queue Context::findHostToDeviceQueue() {
      return transferFamily_ ? device_->getQueue(*transferFamily_, 0)
                             : vk::Queue{};
    }

    vk::Queue Context::findDeviceToHostQueue() {
      auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
      return transferFamily_
                 ? device_->getQueue(
                       *transferFamily_,
                       queueFamilies[*transferFamily_].queueCount > 1)
                 : vk::Queue{};
    }
  } // namespace gpu
} // namespace mobula