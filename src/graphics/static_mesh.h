#pragma once

#include "../core/gpu_buffer.h"
#include "../core/gpu_context.h"
#include "static_vertex.h"

namespace imp {
  class static_mesh {
  public:
    static_mesh(
        gpu_context &context,
        size_t vertex_count,
        static_vertex const *vertices,
        size_t index_count,
        uint16_t const *indices);

  private:
    gpu_buffer vertex_buffer_;
    gpu_buffer index_buffer_;

    gpu_buffer create_vertex_buffer(gpu_context &context, size_t vertex_count);
    gpu_buffer create_index_buffer(gpu_context &context, size_t index_count);
  };
} // namespace imp