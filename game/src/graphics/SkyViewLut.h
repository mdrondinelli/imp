//#pragma once
//
//#include "../system/GpuImage.h"
//#include "../util/Extent.h"
//
//namespace imp {
//  class AtmosphereBuffer;
//  class DirectionalLight;
//  class TransmittanceLut;
//
//  class SkyViewLut {
//  public:
//    class Flyweight {
//    public:
//      explicit Flyweight(GpuContext *context);
//
//      GpuContext *getContext() const noexcept;
//      vk::DescriptorSetLayout getDescriptorSetLayout() const noexcept;
//      vk::PipelineLayout getPipelineLayout() const noexcept;
//      vk::Pipeline getPipeline() const noexcept;
//      vk::Sampler getSampler() const noexcept;
//
//    private:
//      GpuContext *context_;
//      vk::DescriptorSetLayout descriptorSetLayout_;
//      vk::UniquePipelineLayout pipelineLayout_;
//      vk::UniquePipeline pipeline_;
//      vk::Sampler sampler_;
//
//      vk::DescriptorSetLayout createDescriptorSetLayout();
//      vk::UniquePipelineLayout createPipelineLayout();
//      vk::UniquePipeline createPipeline();
//      vk::Sampler createSampler();
//    };
//
//    SkyViewLut(
//        Flyweight const *flyweight,
//        TransmittanceLut const *transmittanceLut,
//        Extent2u const &extent);
//
//    unsigned getFrameCount() const noexcept;
//    unsigned getFrameIndex() const noexcept;
//    Flyweight const *getFlyweight() const noexcept;
//    AtmosphereBuffer const *getAtmosphereBuffer() const noexcept;
//    TransmittanceLut const *getTransmittanceLut() const noexcept;
//    Extent2u const &getExtent() const noexcept;
//    GpuImage const &getImage() const noexcept;
//    vk::ImageView getImageView() const noexcept;
//    vk::ImageView getImageView(unsigned frameIndex) const noexcept;
//
//    void
//    compute(vk::CommandBuffer cmd, DirectionalLight const &sun, float altitude);
//
//  private:
//    Flyweight const *flyweight_;
//    AtmosphereBuffer const *atmosphereBuffer_;
//    TransmittanceLut const *transmittanceLut_;
//    Extent2u extent_;
//    GpuImage image_;
//    std::vector<vk::UniqueImageView> imageViews_;
//    vk::UniqueDescriptorPool descriptorPool_;
//    std::vector<vk::DescriptorSet> descriptorSets_;
//
//    GpuImage createImage();
//    std::vector<vk::UniqueImageView> createImageViews();
//    vk::UniqueDescriptorPool createDescriptorPool();
//    std::vector<vk::DescriptorSet> allocateDescriptorSets();
//    void updateDescriptorSets();
//  };
//} // namespace imp