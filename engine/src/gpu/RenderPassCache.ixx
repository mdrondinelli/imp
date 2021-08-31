// clang-format off
export module mobula.gpu:RenderPassCache;
import <mutex>;
import <unordered_set>;
import :RenderPass;
import :RenderPassParams;
// clang-format on

namespace mobula {
  namespace gpu {
    /**
     * \brief Cache for render passes.
     */
    export class RenderPassCache {
    public:
      /**
       * \param device the device to be used by this cache to create render
       * passes.
       */
      explicit RenderPassCache(vk::Device device);

      /**
       * If this function is called with params equal to the params of a
       * previous invocation, it returns the same render pass as the first
       * invocation. Otherwise, this function creates and returns a new render
       * pass.
       *
       * \param params the parameters of a render pass.
       *
       * \return a pointer to the render pass described by params.
       */
      RenderPass const *get(RenderPassParams const &params);

    private:
      struct Hash {
        using is_transparent = void;

        std::size_t operator()(RenderPass const &renderPass) const noexcept {
          return hash_value(renderPass.getParams());
        }

        std::size_t operator()(RenderPassParams const &params) const noexcept {
          return hash_value(params);
        }
      };

      struct Equal {
        using is_transparent = void;

        bool operator()(
            RenderPass const &lhs, RenderPass const &rhs) const noexcept {
          return &lhs == &rhs;
        }

        bool operator()(
            RenderPass const &lhs, RenderPassParams const &rhs) const noexcept {
          return lhs.getParams() == rhs;
        }

        bool operator()(
            RenderPassParams const &lhs, RenderPass const &rhs) const noexcept {
          return lhs == rhs.getParams();
        }
      };

      vk::Device device_;
      std::unordered_set<RenderPass, Hash, Equal> cache_;
      std::mutex mutex_;
    };
  } // namespace gpu
} // namespace mobula