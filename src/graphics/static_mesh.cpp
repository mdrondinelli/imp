#include "static_mesh.h"

namespace imp {
  static_mesh::static_mesh(
      gpu_context &context,
      size_t vertex_count,
      static_vertex const *vertices,
      size_t index_count,
      uint16_t const *indices):
      vertex_buffer_{create_vertex_buffer(context, vertex_count)},
      index_buffer_{create_index_buffer(context, index_count)} {
    auto vertex_buffer_size = vertex_count * sizeof(static_vertex);
    auto index_buffer_size = index_count * sizeof(uint16_t);
    auto create_info = vk::BufferCreateInfo{};
    create_info.size = vertex_buffer_size + index_buffer_size;
    create_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
    create_info.sharingMode = vk::SharingMode::eExclusive;
    auto alloc_info = VmaAllocationCreateInfo{};
    alloc_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY;
    auto staging_buffer = context.create_buffer(create_info, alloc_info);
    staging_buffer.map();
    auto vertex_dst = staging_buffer.data();
    auto index_dst = staging_buffer.data() + vertex_buffer_size;
    std::memcpy(vertex_dst, vertices, vertex_buffer_size);
    std::memcpy(index_dst, indices, index_buffer_size);
    staging_buffer.unmap();

  }

  gpu_buffer
  static_mesh::create_vertex_buffer(gpu_context &context, size_t vertex_count) {
    auto create_info = vk::BufferCreateInfo{};
    create_info.size = vertex_count * sizeof(static_vertex);
    create_info.usage = vk::BufferUsageFlagBits::eTransferDst |
                        vk::BufferUsageFlagBits::eVertexBuffer;
    create_info.sharingMode = vk::SharingMode::eExclusive;
    auto alloc_info = VmaAllocationCreateInfo{};
    alloc_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
    return context.create_buffer(create_info, alloc_info);
  }

  gpu_buffer
  static_mesh::create_index_buffer(gpu_context &context, size_t index_count) {
    auto create_info = vk::BufferCreateInfo{};
    create_info.size = index_count * sizeof(uint16_t);
    create_info.usage = vk::BufferUsageFlagBits::eTransferDst |
                        vk::BufferUsageFlagBits::eIndexBuffer;
    create_info.sharingMode = vk::SharingMode::eExclusive;
    auto alloc_info = VmaAllocationCreateInfo{};
    alloc_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
    return context.create_buffer(create_info, alloc_info);
  }
} // namespace imp