#include "StaticMesh.h"

namespace imp {
  //StaticMesh::StaticMesh(
  //    GpuContext &context,
  //    size_t vertex_count,
  //    StaticVertex const *vertices,
  //    size_t index_count,
  //    uint16_t const *indices):
  //    vertex_buffer_{create_vertex_buffer(context, vertex_count)},
  //    index_buffer_{create_index_buffer(context, index_count)} {
  //  auto vertex_buffer_size = vertex_count * sizeof(StaticVertex);
  //  auto index_buffer_size = index_count * sizeof(uint16_t);
  //  auto create_info = vk::BufferCreateInfo{};
  //  create_info.size = vertex_buffer_size + index_buffer_size;
  //  create_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
  //  create_info.sharingMode = vk::SharingMode::eExclusive;
  //  auto alloc_info = VmaAllocationCreateInfo{};
  //  alloc_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY;
  //  auto staging_buffer = context.createBuffer(create_info, alloc_info);
  //  staging_buffer.map();
  //  auto vertex_dst = staging_buffer.data();
  //  auto index_dst = staging_buffer.data() + vertex_buffer_size;
  //  std::memcpy(vertex_dst, vertices, vertex_buffer_size);
  //  std::memcpy(index_dst, indices, index_buffer_size);
  //  staging_buffer.unmap();

  //}

  //GpuBuffer
  //StaticMesh::create_vertex_buffer(GpuContext &context, size_t vertex_count) {
  //  auto create_info = vk::BufferCreateInfo{};
  //  create_info.size = vertex_count * sizeof(StaticVertex);
  //  create_info.usage = vk::BufferUsageFlagBits::eTransferDst |
  //                      vk::BufferUsageFlagBits::eVertexBuffer;
  //  create_info.sharingMode = vk::SharingMode::eExclusive;
  //  auto alloc_info = VmaAllocationCreateInfo{};
  //  alloc_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
  //  return context.createBuffer(create_info, alloc_info);
  //}

  //GpuBuffer
  //StaticMesh::create_index_buffer(GpuContext &context, size_t index_count) {
  //  auto create_info = vk::BufferCreateInfo{};
  //  create_info.size = index_count * sizeof(uint16_t);
  //  create_info.usage = vk::BufferUsageFlagBits::eTransferDst |
  //                      vk::BufferUsageFlagBits::eIndexBuffer;
  //  create_info.sharingMode = vk::SharingMode::eExclusive;
  //  auto alloc_info = VmaAllocationCreateInfo{};
  //  alloc_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
  //  return context.createBuffer(create_info, alloc_info);
  //}
} // namespace imp