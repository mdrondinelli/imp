//#pragma once
//
//#include <memory>
//
//#include "../system/GpuImage.h"
//
//namespace imp {
//  class Scene;
//  class TransmittanceLut;
//
//  class AerialPerspectiveLut {
//  public:
//    class Flyweight {
//    public:
//      explicit Flyweight(GpuContext &context);
//
//      GpuContext *getContext() const noexcept;
//      vk::DescriptorSetLayout getImageDescriptorSetLayout() const noexcept;
//      vk::DescriptorSetLayout getTextureDescriptorSetLayout() const noexcept;
//      vk::PipelineLayout getPipelineLayout() const noexcept;
//      vk::Pipeline getPipeline() const noexcept;
//      vk::Sampler getSampler() const noexcept;
//
//    private:
//      GpuContext *context_;
//      vk::DescriptorSetLayout imageDescriptorSetLayout_;
//      vk::DescriptorSetLayout textureDescriptorSetLayout_;
//      vk::UniquePipelineLayout pipelineLayout_;
//      vk::UniquePipeline pipeline_;
//      vk::Sampler sampler_;
//
//      vk::DescriptorSetLayout createImageDescriptorSetLayout();
//      vk::DescriptorSetLayout createTextureDescriptorSetLayout();
//      vk::UniquePipelineLayout createPipelineLayout();
//      vk::UniquePipeline createPipeline();
//      vk::Sampler createSampler();
//    };
//
//    explicit AerialPerspectiveLut(
//        std::shared_ptr<Flyweight const> flyweight,
//        unsigned width,
//        unsigned height,
//        unsigned depth);
//
//    unsigned getWidth() const noexcept;
//    unsigned getHeight() const noexcept;
//    unsigned getDepth() const noexcept;
//    vk::DescriptorSet getImageDescriptorSet() const noexcept;
//    vk::DescriptorSet getTextureDescriptorSet() const noexcept;
//
//    void compute(
//        vk::CommandBuffer cmd,
//        Scene const &scene,
//        TransmittanceLut const &transmittanceLut);
//
//  private:
//    std::shared_ptr<Flyweight const> flyweight_;
//    unsigned width_;
//    unsigned height_;
//    unsigned depth_;
//    GpuImage image_;
//    vk::UniqueImageView imageView_;
//    vk::UniqueDescriptorPool descriptorPool_;
//    std::vector<vk::DescriptorSet> descriptorSets_;
//
//    GpuImage createImage();
//    vk::UniqueImageView createImageView();
//    vk::UniqueDescriptorPool createDescriptorPool();
//    std::vector<vk::DescriptorSet> allocateDescriptorSets();
//    void updateDescriptorSets();
//  };
//} // namespace imp