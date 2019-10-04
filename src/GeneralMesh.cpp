#include "../header/GeneralMesh.h"


GeneralMesh::GeneralMesh(const char* input_filename_c, const char* key_vertices_filename)
{ 
    Init_(input_filename_c, key_vertices_filename);
    glFriendlyMesh_();
}

GeneralMesh::GeneralMesh(const char * input_filename_c, const char * key_vertices_filename, bool flag)
{
    Init_(input_filename_c, key_vertices_filename);
}
GeneralMesh::~GeneralMesh()
{
}

void GeneralMesh::Init_(const char* input_filename_c, const char* key_vertices_filename)
{
    // check for existance
    if (!checkFileExist_(input_filename_c))
    {
        throw std::exception("General Mesh: input file doesn't exist");
    }
    if (key_vertices_filename != nullptr && !checkFileExist_(key_vertices_filename))
    {
        throw std::exception("General Mesh: key vertices file doesn't exist");
    }

    std::string input_filename(input_filename_c);
    cutName_(input_filename);


    std::string saved_filename = mg::getDir(input_filename) + "normalized_" + name_;

    if (checkFileExist_(saved_filename.c_str())) {
        readFile_(saved_filename, true);

    }
    else {
        readFile_(input_filename);
        normalizeVertices_();
        saveNormalizedMesh(mg::getDir(input_filename));
    }

    if (key_vertices_filename != nullptr)
    {
        readKeyVertices_(key_vertices_filename);
    }


}

void GeneralMesh::saveNormalizedMesh(std::string path) const
{
    igl::writeOBJ(path + "normalized_" + name_, verts_normalized_, faces_);
}

void GeneralMesh::readFile_(const std::string & filename, bool save)
{
    std::size_t point_pos = filename.find_last_of('.');
    std::string extention = filename.substr(point_pos + 1);
    bool sucess;
    if (extention == "obj")
    {
        sucess = igl::readOBJ(filename, verts_, faces_);
    }
    else if (extention == "ply")
    {
        sucess = igl::readPLY(filename, verts_, faces_);
    }
    else
    {
        throw std::exception("GeneralMesh::Unsupported type of input mesh. Supported types: .obj, .ply");
    }

    if (!sucess || verts_.rows() == 0 || faces_.rows() == 0)
    {
        throw std::exception("GeneralMesh::reading mesh file failed: vertices/faces are not filled");
    }

    igl::per_vertex_normals(verts_, faces_, verts_normals_);

    verts_normalized_ = verts_;
}

void GeneralMesh::normalizeVertices_()
{
    //mean_point_ = verts_.colwise().mean();
    //using boundingbox
    mean_point_.resize(3, 1);
    mean_point_(0) = (verts_.col(0).maxCoeff() + verts_.col(0).minCoeff()) / 2.0;
    mean_point_(1) = (verts_.col(1).maxCoeff() + verts_.col(1).minCoeff()) / 2.0;
    mean_point_(2) = (verts_.col(2).maxCoeff() + verts_.col(2).minCoeff()) / 2.0;
    verts_normalized_ = verts_.rowwise() - mean_point_.transpose();

    // convert to meters heuristically
    // check for sm througth height (Y axis)
    double interval = verts_normalized_.col(1).maxCoeff() - verts_normalized_.col(1).minCoeff();
    if (interval > 1)
    {
        double reciprocal_interval = 1 / interval;
        //big adjustment
        verts_normalized_ *= reciprocal_interval;
        std::cout << "Warning: Mesh is found to use sm/mm units. Scaled down by " << reciprocal_interval << std::endl;
    }
}

void GeneralMesh::glFriendlyMesh_()
{
    ////GLMVertex
    // vertices
    for (int i = 0; i < verts_normalized_.rows(); ++i)
    {
        GLMVertex gl_vert;
        gl_vert.position = glm::vec3(verts_normalized_(i, 0), verts_normalized_(i, 1), verts_normalized_(i, 2));
        gl_vert.normal = glm::vec3(verts_normals_(i, 0), verts_normals_(i, 1), verts_normals_(i, 2));

        gl_vertices_normalized_.push_back(gl_vert);
    }

    // faces
    for (int i = 0; i < faces_.rows(); ++i)
    {
        for (int j = 0; j < faces_.cols(); ++j)
        {
            // shift indices to start from zero
            gl_faces_.push_back(faces_(i, j));
        }
    }
    ////
}

void GeneralMesh::cutName_(const std::string & filename)
{
    std::string object_name = mg::ssplit(std::string{ filename }, '/').back();

    this->name_ = object_name;
}

void GeneralMesh::readKeyVertices_(const char * filename)
{
    std::fstream inFile;
    inFile.open(filename, std::ios_base::in);
    int keys_n;
    inFile >> keys_n;
    // Sanity check
    if (keys_n <= 0)
    {
        throw std::exception("Number of key vertices should be a positive number!");
    }

    std::string key_name;
    int vertexId;
    for (int i = 0; i < keys_n; i++)
    {
        inFile >> key_name;
        inFile >> vertexId;

        key_points_.insert(CoordsDictEntry(key_name, verts_.row(vertexId)));
    }

    inFile.close();
}

bool GeneralMesh::checkFileExist_(const char * filename)
{
    std::ifstream infile(filename);
    return infile.good();
}

std::vector<std::string> GeneralMesh::tokenize_getline(const std::string& data, const char& delimiter) {
    std::vector<std::string> result;
    std::string delimiter_str, token;
    std::stringstream ss(data);
    bool isDefaultDelimiter;

    delimiter_str = delimiter;
    isDefaultDelimiter = (!delimiter_str.compare(" "));

    if (isDefaultDelimiter) {
        while (getline(ss, token, delimiter)) {
            result.push_back(token);
        }
    }
    else {
        while (getline(ss, token, delimiter)) {
            std::istringstream iss(token);
            for (std::string splited_str; iss >> splited_str; ) {
                result.push_back(splited_str);
            }
        }
    }

    return result;
}
