#include "../header/GeneralMeshTexture.h"

GeneralMeshTexture::GeneralMeshTexture(const char* input_filename_c, const char* input_texturename_c):
    GeneralMesh(input_filename_c, nullptr, true) {    

    readTex_(input_texturename_c);
    readTexUV_(input_filename_c);
    readFaceTexUV_(input_filename_c);
    glFriendlyMesh_();
};

void GeneralMeshTexture::readTex(TextureInfo & _tex_info, const std::string & imagename)
{
    _tex_info.data = stbi_load(imagename.c_str(), &(_tex_info.width), &(_tex_info.height), &(_tex_info.n_channels), 0);
}

//vertices are ordered by self, it can make face only order without other faces info.
Eigen::MatrixXd GeneralMeshTexture::getEGNormalizedVerticesWithUV() const {
    Eigen::MatrixXd ret;
    ret.resize(gl_vertices_normalized_with_uv_.size(), 3);

    for (auto i = 0; i < gl_vertices_normalized_with_uv_.size(); ++i) {
        ret(i, 0) = gl_vertices_normalized_with_uv_[i].position[0];
        ret(i, 1) = gl_vertices_normalized_with_uv_[i].position[1];
        ret(i, 2) = gl_vertices_normalized_with_uv_[i].position[2];
    }

    return ret;
}

const GeneralMeshTexture::TextureInfo& GeneralMeshTexture::getTexInfo() const
{
    if (!tex_info_.data) {
        std::cout << "Failed to load texture" << std::endl;
        exit(1);
    }

    return tex_info_;
}


void GeneralMeshTexture::readTex_(const std::string & imagename)
{
    tex_info_.data = stbi_load(imagename.c_str(), &(tex_info_.width), &(tex_info_.height), &(tex_info_.n_channels), 0);
}

void GeneralMeshTexture::readTexUV_(const std::string & filename)
{
    //get input
    std::string in_line;
    std::ifstream in(filename);
    std::vector<std::array<double, 2>> uv_list_list_;

    while (getline(in, in_line)) {
        //if in_line is start on vt, (!(in_line.substr(0,1) == "vt")) == 1
        if ((in_line.size() > 1) && (in_line.substr(0, 2) == "vt")) {
            std::stringstream stringcin;
            std::vector<std::string> tokenize_in_line_list;

            tokenize_in_line_list = tokenize_getline(in_line);
            uv_list_list_.push_back(std::array<double, 2>());

            std::array<double, 2>& uv_list = uv_list_list_[uv_list_list_.size() - 1];
            for (int k = 0; k < uv_list.size(); ++k) {
                int local_vt_idx;
                double local_vt;

                double& local_v_uv = uv_list[k];
                //first 1 is for ignoring 'f'
                local_vt_idx = 1 + k;
                local_vt = std::stod(tokenize_in_line_list[local_vt_idx]);

                local_v_uv = local_vt;
            }
        }
    }
    uv_.resize(uv_list_list_.size(), 2);
    for (auto i = 0; i < uv_list_list_.size(); ++i) {
        uv_(i, 0) = uv_list_list_[i][0];
        uv_(i, 1) = uv_list_list_[i][1];
    }
    in.close();
}

void GeneralMeshTexture::readFaceTexUV_(const std::string & filename)
{
    //get input
    std::string in_line;
    std::ifstream in(filename);
    std::vector<std::array<int, 3>> v_uv_in_face_list_list_;

    while (getline(in, in_line)) {
        if ((in_line.size() > 0) && (in_line[0] == 'f')) {
            std::stringstream stringcin;
            std::vector<std::string> tokenize_in_line_list;
            tokenize_in_line_list = tokenize_getline(in_line, '/');
            v_uv_in_face_list_list_.push_back(std::array<int, 3>());

            std::array<int, 3>& v_uv_in_face_list = v_uv_in_face_list_list_[v_uv_in_face_list_list_.size() - 1];
            for (int k = 0; k < v_uv_in_face_list.size(); ++k) {
                int local_vt_idx, local_vt;

                int& local_v_uv = v_uv_in_face_list[k];
                //first 1 is for ignoring 'f'
                local_vt_idx = 1 + k * 3 + 1;
                local_vt = std::stoi(tokenize_in_line_list[local_vt_idx]);

                //local_vt is start from 1
                local_v_uv = local_vt - 1;
            }
        }
    }
    //

    faces_tex_uv_.resize(v_uv_in_face_list_list_.size(), 3);
    for (auto i = 0; i < v_uv_in_face_list_list_.size(); ++i) {
        faces_tex_uv_(i, 0) = v_uv_in_face_list_list_[i][0];
        faces_tex_uv_(i, 1) = v_uv_in_face_list_list_[i][1];
        faces_tex_uv_(i, 2) = v_uv_in_face_list_list_[i][2];
    }
    in.close();
}

void GeneralMeshTexture::glFriendlyMesh_()
{
    ////GLMVertexWithUV
    for (int face_idx = 0; face_idx < faces_.rows(); ++face_idx) {
        for (int k = 0; k < 3; ++k) {
            int j = faces_(face_idx, k);
            int uv_idx = faces_tex_uv_(face_idx, k);
            GLMVertexWithUV gl_vert;

            gl_vert.position = glm::vec3(verts_normalized_(j, 0), verts_normalized_(j, 1), verts_normalized_(j, 2));
            gl_vert.normal = glm::vec3(verts_normals_(j, 0), verts_normals_(j, 1), verts_normals_(j, 2));
            gl_vert.uv = glm::vec2(uv_(uv_idx, 0), uv_(uv_idx, 1));
            gl_vertices_normalized_with_uv_.push_back(gl_vert);
        }
    }
}