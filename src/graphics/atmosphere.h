#pragma once

#include "../core/GpuContext.h"
#include "OpticalDepthLut.h"
#include "ScatteringLut.h"

namespace imp {
  struct AtmosphereCreateInfo {
    GpuContext *context;
    float planetRadius = 6360e3f;
    float atmosphereRadius = 6420e3f;
    Vector4f scatteringCoefficients =
        makeVector(3.8e-6f, 13.5e-6f, 33.1e-6f, 21e-6f);
    Vector2u opticalDepthLutSize = makeVector(512u, 512u);
    Vector3u scatteringLutSize = makeVector(64u, 256u, 256u);
  };

  class Atmosphere {
  public:
    explicit Atmosphere(AtmosphereCreateInfo const &createInfo);

    Vector4f const &getScatteringCoefficients() const noexcept;
    float getPlanetRadius() const noexcept;
    float getAtmosphereRadius() const noexcept;

    ScatteringLut const &getScatteringLut() const noexcept;
    OpticalDepthLut const &getOpticalDepthLut() const noexcept;

  private:
    GpuContext *context_;
    float planetRadius_;
    float atmosphereRadius_;
    Vector4f scatteringCoefficients_;
    vk::UniqueCommandPool commandPool_;
    vk::UniqueDescriptorSetLayout lutDescriptorSetLayout_;
    vk::UniquePipelineLayout opticalDepthPipelineLayout_;
    vk::UniquePipelineLayout scatteringPipelineLayout_;
    vk::UniquePipeline opticalDepthPipeline_;
    vk::UniquePipeline scatteringPipeline_;
    vk::UniqueDescriptorPool descriptorPool_;
    std::vector<vk::DescriptorSet> descriptorSets_;
    OpticalDepthLut opticalDepthLut_;
    ScatteringLut scatteringLut_;

    vk::UniqueCommandPool createCommandPool();
    vk::UniqueDescriptorSetLayout createLutDescriptorSetLayout();
    vk::UniquePipelineLayout createOpticalDepthPipelineLayout();
    vk::UniquePipelineLayout createScatteringPipelineLayout();
    vk::UniquePipeline createOpticalDepthPipeline();
    vk::UniquePipeline createScatteringPipeline();
    vk::UniqueDescriptorPool createDescriptorPool();
    std::vector<vk::DescriptorSet> allocateDescriptorSets();
    OpticalDepthLut createOpticalDepthLut(Vector2u const &size);
    ScatteringLut createScatteringLut(Vector3u const &size);

    void updateDescriptorSets();
    void updateOpticalDepthLut();
    void updateScatteringLut();
  };
} // namespace imp