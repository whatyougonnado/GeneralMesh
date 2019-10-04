#pragma once

#include <GeneralMesh.h>
#include <iostream>

class GeneralMeshTexture :public GeneralMesh
{
public:
    GeneralMeshTexture(const char* input_filename_c, const char* input_texturename_c);
    ~GeneralMeshTexture() {};

    struct TextureInfo {
        unsigned char *data;
        int width;
        int height;
        int n_channels;

    };

    struct GLMVertexWithUV
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    static void readTex(TextureInfo& _tex_info, const std::string & imagename);

    const std::vector<GLMVertexWithUV>& getGLNormalizedVerticesWithUV() const { return gl_vertices_normalized_with_uv_; };
    Eigen::MatrixXd getEGNormalizedVerticesWithUV() const;
    const TextureInfo& getTexInfo() const;

private:
    void readTex_(const std::string & imagename);
    void readTexUV_(const std::string& filename);
    //get from GeneralMeshIdx
    void readFaceTexUV_(const std::string& filename);
    void glFriendlyMesh_();

    std::vector<GLMVertexWithUV> gl_vertices_normalized_with_uv_;

    Eigen::MatrixXd uv_;
    Eigen::MatrixXi faces_tex_uv_;

    TextureInfo tex_info_;
};