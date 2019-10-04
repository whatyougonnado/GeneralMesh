#include "../header/GeneralMeshIdx.h"

GeneralMeshIdx::GeneralMeshIdx(const char* input_filename_c) :
    GeneralMesh(input_filename_c, nullptr, true) {
    glFriendlyMesh_();
};

glm::vec3 GeneralMeshIdx::encodeNormalizedNumeralSystem(int val, int to) {
    glm::vec3 ret;
    int integer[3], rest;
    double ele[3];

    rest = val + 1; // make id zero(same color with background) to id one

    integer[0] = val / to / to;
    rest -= integer[0] * to * to;

    integer[1] = rest / to;
    rest -= integer[1] * to;

    integer[2] = rest;

    for (auto k = 0; k < 3; ++k) {
        ret[k] = (double)integer[k] / (double)to;
    }

    return glm::vec3(ret[0], ret[1], ret[2]);

}

void GeneralMeshIdx::glFriendlyMesh_()
{
    ////GLMVertexWithId
    for (int face_idx = 0; face_idx < faces_.rows(); ++face_idx) {
        for (int k = 0; k < 3; ++k) {
            int j = faces_(face_idx, k);
            GLMVertexWithId gl_vert;

            gl_vert.position = glm::vec3(verts_normalized_(j, 0), verts_normalized_(j, 1), verts_normalized_(j, 2));
            gl_vert.faceid = encodeNormalizedNumeralSystem(face_idx);
            gl_vertices_normalized_with_faceid_.push_back(gl_vert);
        }
    }

}