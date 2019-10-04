#pragma once

#include <GeneralMesh.h>
#include <GeneralUtility.h>

#include <iostream>
#include <fstream>


class ParsingMesh :public GeneralMesh
{
public:
    ParsingMesh(const char* input_filename_c, std::vector<std::string> face_color, std::map<std::string, std::array<float, 3>> color_table, const char* input_texturename_c = nullptr, const char* key_vertices_filename = nullptr);
    ~ParsingMesh() {};

    struct GLMVertexWithColor {
        glm::vec3 position;
        glm::vec3 color;
    };
    
    void setUVTable(std::string load_full_path);
    void saveOBJ(std::string uv_table_full_path, std::string save_full_path);

    const std::vector<GLMVertexWithColor>& getGLNormalizedVerticesWithColor() const { return gl_vertices_normalized_with_color_; };

private:
    void glFriendlyMesh_();
    glm::vec3 getParsingColor_(int face_idx);
    
    std::string input_filename_c_;

    std::vector<GLMVertexWithColor> gl_vertices_normalized_with_color_;

    std::vector<std::string> face_color_;
    std::map<std::string, std::array<float, 3>> color_table_;
    std::map<std::string, std::array<float, 3>> uv_table_;
};