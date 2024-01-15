#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

class Model
{
    public:
        Model(const std::string& path);
        void draw();
    
private:
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
#endif