#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <assimp/matrix4x4.h>
#include <foundation/PxTransform.h>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>

namespace utilities
{
    inline glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from)
    {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    inline physx::PxTransform glmMat4ToPxTransform(const glm::mat4& mat) {
        glm::vec3 pos = glm::vec3(mat[3]);
        glm::quat rot = glm::quat_cast(mat);
        return physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z),
                                  physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
    }

    inline glm::mat4 pxTransformToGlmMat4(const physx::PxTransform& transform) {
        glm::quat q(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
        glm::mat4 rotation = glm::toMat4(q);
        rotation[3] = glm::vec4(transform.p.x, transform.p.y, transform.p.z, 1.0f);
        return rotation;
    }
} //namespace utilities

#endif //UTILITIES_HPP