#ifndef POST_PROCESSING_KERNELS_HPP
#define POST_PROCESSING_KERNELS_HPP

#include <vector>

namespace Engine::Renderer {

    struct PostProcessingKernel {
        int dimension;
        std::vector<float> weights;
    };

    namespace Kernels {

        inline const PostProcessingKernel gaussianBlur3x3 = {
            3,
            {
                1.0f / 16, 2.0f / 16, 1.0f / 16,
                2.0f / 16, 4.0f / 16, 2.0f / 16,
                1.0f / 16, 2.0f / 16, 1.0f / 16
            }
        };

        inline const PostProcessingKernel sharpen3x3 = {
            3,
            {
                 0, -1,  0,
                -1,  5, -1,
                 0, -1,  0
            }
        };

        inline const PostProcessingKernel edgeDetection3x3 = {
            3,
            {
                -1, -1, -1,
                -1,  8, -1,
                -1, -1, -1
            }
        };
    }
}

#endif // POST_PROCESSING_KERNELS_HPP
