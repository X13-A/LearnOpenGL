#include "Model.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model()
{
    _scale = glm::vec3(1, 1, 1);
    _position = glm::vec3(0, 0, 0);
    _rotation = glm::vec3(0, 0, 0);
}

Model::Model(const std::string& path)
{
    loadModel(path);
    _scale = glm::vec3(1, 1, 1);
    _position = glm::vec3(0, 0, 0);
    _rotation = glm::vec3(0, 0, 0);
}

const glm::vec3& Model::getPosition() const
{
    return _position;
}

const glm::vec3& Model::getRotation() const
{
    return _rotation;
}

const glm::vec3& Model::getScale() const
{
    return _scale;
}

void Model::setPosition(const glm::vec3& value)
{
    _position = value;
    for (size_t i = 0; i < _meshes.size(); i++)
    {
        _meshes[i]->transform.setPosition(value);
    }
}

void Model::setRotation(const glm::vec3& value)
{
    _rotation = value;
    for (size_t i = 0; i < _meshes.size(); i++)
    {
        _meshes[i]->transform.setRotation(value);
    }
}

void Model::setScale(const glm::vec3& value)
{
    _scale = value;
    for (size_t i = 0; i < _meshes.size(); i++)
    {
        _meshes[i]->transform.setScale(value);
    }
}

void Model::translate(const glm::vec3& offset)
{
    _position += offset;
    for (size_t i = 0; i < _meshes.size(); i++)
    {
        _meshes[i]->transform.translate(offset);
    }
}

void Model::rotate(const glm::vec3& axis, const float& degree)
{
    _rotation = axis * degree;
    for (size_t i = 0; i < _meshes.size(); i++)
    {
        _meshes[i]->transform.rotate(axis, degree);
    }
}

void Model::scale(const glm::vec3& value)
{
    _scale *= value;
    for (size_t i = 0; i < _meshes.size(); i++)
    {
        _meshes[i]->transform.scale(value);
    }
}

void Model::draw(Shader& shader)
{
    for (unsigned int i = 0; i < _meshes.size(); i++)
    {
        shader.setMat4("modelMatrix", _meshes[i]->transform.getTransformMatrix());
        _meshes[i]->draw();
    }
}

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    _directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(processMesh(mesh, scene));
    }
    // do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        glm::vec2 texCoords;

        // Check if mesh contain texture coordinates
        if (mesh->mTextureCoords[0])
        {
            texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            texCoords = glm::vec2(0.0f, 0.0f);
        }

        Vertex vertex = Vertex(pos, glm::vec4(1, 1, 1, 1), texCoords, normal);
        vertices.push_back(vertex);
    }
    
    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // TODO: Process materials
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return std::make_shared<Mesh>(glm::vec3(), glm::vec3(), glm::vec3(1, 1, 1), vertices, indices);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    // TODO
    return std::vector<Texture>();
}