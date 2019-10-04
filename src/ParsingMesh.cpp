#include "../header/ParsingMesh.h"

ParsingMesh::ParsingMesh(const char* input_filename_c, std::vector<std::string> face_color, std::map<std::string, std::array<float, 3>> color_table, const char* input_texturename_c, const char* key_vertices_filename) :
    face_color_(face_color), color_table_(color_table), input_filename_c_(input_filename_c), GeneralMesh(input_filename_c, key_vertices_filename, true) {
  
    glFriendlyMesh_();
};

void ParsingMesh::setUVTable(std::string load_full_path)
{
    std::vector<std::string> token_list;
    std::string in_line;
    std::ifstream in;
    in.open(load_full_path);

    uv_table_.clear();

    while (getline(in, in_line)) {
        token_list = mg::ssplit(in_line, ',');
        uv_table_.insert(std::pair<std::string, std::array<float, 3>>{token_list[0], std::array<float, 3>{
            std::stof(token_list[1]) / 1023.0f, std::stof(token_list[2]) / 1023.0f, 0.0f}});
    }
    in.close();

}

void ParsingMesh::saveOBJ(std::string uv_table_full_path, std::string save_full_path)
{
    std::string in_line;
    std::ifstream infile;
    std::ofstream outfile(save_full_path);
    std::vector<std::array<float, 2>> cache;
    setUVTable(uv_table_full_path);

    infile.open(input_filename_c_);

    cache.resize(faces_.rows());//cahce.resize(face.size())

    for (auto &iter : cache) {
        iter[0] = 0.0f;
        iter[1] = 0.0f;
    }


    float idx = 0.1f; // consider that this isn't int.

    int vt_idx = 0;
    int face_idx = 0;
    while (getline(infile, in_line)) {
        std::vector<std::string> token_list = mg::ssplit(in_line, ' ');
        if ((in_line.size() > 0) && (token_list[0].compare("vt") == 0)) {
            if (vt_idx++ == 0) {
                for (auto& iter : uv_table_) {

                    outfile << "vt" << " " << std::to_string(iter.second[0]) << " " << std::to_string(iter.second[1]) << std::endl;
                    outfile << "vt" << " " << std::to_string(iter.second[0] + 0.01f) << " " << std::to_string(iter.second[1]) << std::endl;
                    outfile << "vt" << " " << std::to_string(iter.second[0] + 0.01f) << " " << std::to_string(iter.second[1] + 0.01f) << std::endl;
                    
                    iter.second[2] = idx;
                    idx += 1.0f;
                }
                
            }
            continue;
        }
        else if((in_line.size() > 0) && (token_list[0].compare("f") == 0)) {
                //load face Á¤º¸
                //array<int,2> = texturecolor_uv[color_table_[face_idx]] 
                int _texturecolor_uv_idx_seed = (int)uv_table_[face_color_[face_idx++]][2];


                outfile << "f";
                for (int i = 1; i <= 3; ++i) {
                    outfile << " " << mg::ssplit(token_list[i], '/')[0] << "/" << std::to_string(_texturecolor_uv_idx_seed * 3 + i) << "/" << mg::ssplit(token_list[i], '/')[2];
                }

                outfile << std::endl;
            }
        else {
            outfile << in_line << std::endl;
        }
    }
    infile.close();

    infile.open(input_filename_c_);

    infile.close();
    outfile.close();
}

void ParsingMesh::glFriendlyMesh_()
{
    ////GLMVertexWithColor
    for (int face_idx = 0; face_idx < faces_.rows(); ++face_idx) {
        for (int k = 0; k < 3; ++k) {
            int j = faces_(face_idx, k);
            GLMVertexWithColor gl_vert;

            gl_vert.position = glm::vec3(verts_normalized_(j, 0), verts_normalized_(j, 1), verts_normalized_(j, 2));
            gl_vert.color = getParsingColor_(face_idx);
            gl_vertices_normalized_with_color_.push_back(gl_vert);
        }
    }

}

glm::vec3 ParsingMesh::getParsingColor_(int face_idx) {

    glm::vec3 ret(0.0f, 0.0f, 0.0f);

    for (auto& color_chip : color_table_) {
        if (color_chip.first == face_color_[face_idx]) {
            ret = glm::vec3((color_chip.second)[0], (color_chip.second)[1], (color_chip.second)[2]);
            return ret;
        }
    }
    std::cout << "glm::vec3 ParsingMesh::getParsingColor_(int face_idx) ERROR: Incorrect color name is found." << "\n" << face_color_[face_idx] << std::endl;
    return ret;
}