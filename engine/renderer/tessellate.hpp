#ifndef TESSELLATE_HPP
#define TESSELLATE_HPP

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "renderer/mesh.hpp"
#include "math/lorentz.hpp"

namespace Engine::Renderer {

    // Poincare geodesics are curved arcs, so straight edges between projected
    // vertices cut the corner. Subdividing each triangle and placing the new
    // vertices on the hyperboloid geodesic makes the rasterized edges follow the
    // arc. The vertex shader still just lifts (aPos -> hyperboloid) and flattens,
    // so storing the geodesic point's spatial part as the position is enough.

    namespace Tessallator {

        // Lift a model-space position onto the hyperboloid: (x, sqrt(1 + |x|^2)).
        inline glm::vec4 liftToHyperboloid(const glm::vec3& p) {
            return glm::vec4(p, std::sqrt(1.0f + glm::dot(p, p)));
        }

        inline glm::vec3 safeNormalize(const glm::vec3& v) {
            float len2 = glm::dot(v, v);
            return len2 > 1e-12f ? v * (1.0f / std::sqrt(len2)) : v;
        }

        // Midpoint of two mesh vertices, split into two parallel tracks so each
        // space keeps its own straight lines:
        //   - position (Euclidean): plain linear midpoint, stays on the flat face
        //   - hyperPosition (hyperbolic): spatial part of the geodesic midpoint
        // Each track interpolates from its own endpoint field, so deeper
        // subdivision levels stay correct.
        inline Vertex geodesicMidVertex(const Vertex& a, const Vertex& b) {
            glm::vec4 hMid = Engine::Lorentz::geodesicMidpoint(
                liftToHyperboloid(a.hyperPosition), liftToHyperboloid(b.hyperPosition));

            Vertex m;
            m.position      = 0.5f * (a.position + b.position);
            m.hyperPosition = glm::vec3(hMid);
            m.normal        = safeNormalize(a.normal + b.normal);
            m.texCoords     = 0.5f * (a.texCoords + b.texCoords);
            m.tangent       = safeNormalize(a.tangent + b.tangent);
            m.bitangent     = safeNormalize(a.bitangent + b.bitangent);
            return m;
        }

        // One 4:1 split of every triangle. Midpoints are shared via a cache keyed
        // on the (sorted) endpoint indices so adjacent triangles stay welded.
        inline void subdivideOnce(std::vector<Vertex>& verts,
                                  std::vector<unsigned int>& indices) {
            std::vector<Vertex> outVerts = verts; // originals keep their indices
            std::vector<unsigned int> outIndices;
            outIndices.reserve(indices.size() * 4);
            std::map<std::pair<unsigned int, unsigned int>, unsigned int> midCache;

            auto midpoint = [&](unsigned int i, unsigned int j) -> unsigned int {
                std::pair<unsigned int, unsigned int> key(std::min(i, j), std::max(i, j));
                auto it = midCache.find(key);
                if (it != midCache.end()) return it->second;
                unsigned int id = static_cast<unsigned int>(outVerts.size());
                outVerts.push_back(geodesicMidVertex(verts[i], verts[j]));
                midCache[key] = id;
                return id;
            };

            for (size_t t = 0; t + 2 < indices.size(); t += 3) {
                unsigned int a = indices[t], b = indices[t + 1], c = indices[t + 2];
                unsigned int ab = midpoint(a, b);
                unsigned int bc = midpoint(b, c);
                unsigned int ca = midpoint(c, a);

                const unsigned int tris[12] = {
                    a, ab, ca,
                    ab, b, bc,
                    ca, bc, c,
                    ab, bc, ca,
                };
                outIndices.insert(outIndices.end(), tris, tris + 12);
            }

            verts = std::move(outVerts);
            indices = std::move(outIndices);
        }

    } // namespace detail

    // Geodesic-subdivide the mesh `level` times (2^level segments per edge).
    // level <= 0 leaves the mesh untouched.
    inline void tessellateHyperbolic(std::vector<Vertex>& vertices,
                                     std::vector<unsigned int>& indices,
                                     int level) {
        for (int i = 0; i < level; ++i) {
            Tessallator::subdivideOnce(vertices, indices);
        }
    }

} // namespace Engine::Renderer

#endif // TESSELLATE_HPP
