// GeneralMesh is a simple class to work with meshes (static, so far)
//
// * loads the files in .obj or .ply
// * performs heuristic normalization 
//      * puts the mesh in the world center 
//      * scales it to the meters units (simple heuristic, may not work for large objects or on arbitrary scales)
//      * but allows to access both normalized and original unnormalized vertices 
// * provides access to geometry in different formats  
//      * Eigen, for compatibility with libigl
//      * glm, for compatibility with OpenGL
// * is able to load the list of the designated vertices with names (optional),
//   see example of the file in the GeneralMeash example project (key_vertices_example.txt)
// 

#pragma once

// Fix compilation errors
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/readPLY.h>
#include <igl/per_vertex_normals.h>

#include <map>
#include <vector>

// for reading texture
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>
#include <array>

#include <sstream>

// for opengl-friendly conversion
#include <glm/glm.hpp>
using CoordsDictionary = std::map<std::string, Eigen::RowVectorXd>;
using CoordsDictEntry = std::pair<std::string, Eigen::RowVectorXd>;

#include <GeneralUtility.h>

#include "Shader.h"

class GeneralMesh
{
public:

    struct GLMVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    GeneralMesh() {};
    GeneralMesh(const char* input_filename_c, const char* key_vertices_filename = nullptr);
    virtual ~GeneralMesh();

    const std::string& getName() const           { return name_; }

    const Eigen::MatrixXi& getFaces() const      { return faces_; }
    // with vertex ids staring from zero:
    const std::vector<unsigned int>& getGLMFaces() const      { return gl_faces_; }

    const Eigen::MatrixXd& getVertices() const   { return verts_; }
    const Eigen::MatrixXd& getNormalizedVertices() const   { return verts_normalized_; }
    const std::vector<GLMVertex>& getGLNormalizedVertices() const { return gl_vertices_normalized_; }
    
    const Eigen::VectorXd& getMeanPoint() const  { return mean_point_; }
    // the user need to check the dictionary for emptiness
    const CoordsDictionary& getKeyPoints() const  { return key_points_; }

    void saveNormalizedMesh(std::string path) const;
    //divide by delimiter and white space(' ')
    static std::vector<std::string> tokenize_getline(const std::string& data, const char& delimiter = ' ');


protected:
    //prevent read twice(== ban GeneralMesh::glFriendlyMesh_();)
    GeneralMesh(const char* input_filename_c, const char* key_vertices_filename, bool flag);
    void Init_(const char* input_filename_c, const char* key_vertices_filename = nullptr);
    void readFile_(const std::string& filename, bool save = false);
    void normalizeVertices_();
    virtual void glFriendlyMesh_();
    void cutName_(const std::string& filename);
    void readKeyVertices_(const char * filename);
    bool checkFileExist_(const char * filename);
    
    std::string name_;

    Eigen::MatrixXd verts_;
    Eigen::MatrixXd verts_normalized_;
    Eigen::MatrixXd verts_normals_;
    std::vector<GLMVertex> gl_vertices_normalized_;

    Eigen::MatrixXi faces_;
    std::vector<unsigned int> gl_faces_;

    Eigen::VectorXd mean_point_;

    CoordsDictionary key_points_;
};
