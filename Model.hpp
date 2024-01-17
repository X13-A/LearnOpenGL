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
        void draw(Shader& shader);
        const glm::vec3& getPosition() const;
        const glm::vec3& getRotation() const;
        const glm::vec3& getScale() const;

        void setPosition(const glm::vec3& value);
        void setRotation(const glm::vec3& value);
        void setScale(const glm::vec3& value);
        void translate(const glm::vec3& offset);
        void rotate(const glm::vec3& axis, const float& degree);
        void scale(const glm::vec3& value);


    private:
        std::vector<Mesh*> _meshes;
        std::string _directory;
        glm::vec3 _position;
        glm::vec3 _rotation;
        glm::vec3 _scale;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
#endif