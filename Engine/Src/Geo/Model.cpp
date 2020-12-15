#include "Model.h"

Model::Model():
    storeIndices(true),
    modelPath(""),
    meshes({}),
    texTypes({}),
    allVertices({}),
    allIndices({}),
    VAO(0),
    VBO(0),
    EBO(0),
    modelMatsForAll({}),
    colorsForAll({}),
    diffuseTexIndicesForAll({})
{
}

Model::Model(cstr const& fPath, const std::initializer_list<aiTextureType>& iL):
    storeIndices(true),
    modelPath(fPath),
    meshes({}),
    texTypes(iL),
    allVertices({}),
    allIndices({}),
    VAO(0),
    VBO(0),
    EBO(0)
{
}

Model::~Model(){
    if(VAO){
        glDeleteVertexArrays(1, &VAO);
    }
    if(VBO){
        glDeleteBuffers(1, &VBO);
    }
    if(EBO){
        glDeleteBuffers(1, &EBO);
    }
}

void Model::LoadModel() const{ //Load model into a DS of Assimp called a scene obj (root obj of Assimp's data interface)
    Assimp::Importer importer;
    const aiScene* const scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    if(!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE){ //If !scene || !(root node of scene) || returned data is incomplete (given by 1 of its flags)
        return (void)printf("Assimp error: %s\n", importer.GetErrorString());
    }
    ProcessNode(scene, scene->mRootNode);
}

void Model::ProcessNode(const aiScene* const& scene, const aiNode* const& node) const{ //Process all of the scene obj's nodes recursively to translate the loaded data to an arr of Mesh objs //For parent-child relation between meshes
    for(uint i = 0; i < node->mNumMeshes; ++i){ //Process all the nodes' meshes (if any)
        const aiMesh* const mesh = scene->mMeshes[node->mMeshes[i]]; //Check a node's mesh indices and retrieve the corresponding mesh by indexing the scene's mMeshes array
        meshes.emplace_back(ProcessMesh(scene, mesh)); //Store mesh obj
    }
    for(uint i = 0; i < node->mNumChildren; ++i){ //node->mNumChildren is base case/exit condition/terminating condition
        ProcessNode(scene, node->mChildren[i]); //Each node contains a set of mesh indices where each points to a specific mesh located in the scene obj
    }
}

Mesh Model::ProcessMesh(const aiScene* const& scene, const aiMesh* const& meshObj) const{
    Mesh mesh;
    mesh.vertices = new std::vector<Vertex>();
    mesh.indices = new std::vector<uint>();

    for(uint i = 0; i < meshObj->mNumVertices; ++i){ //For each vertex of the mesh...
        const aiVector3D& vertices = meshObj->mVertices[i];
        const aiColor4D* const& colours = meshObj->mColors[0];
        const aiVector3D* const& texCoords = meshObj->mTextureCoords[0];
        const aiVector3D* const& normals = meshObj->mNormals;
        const aiVector3D* const& tangents = meshObj->mTangents;
        mesh.vertices->push_back({
            glm::vec3(vertices.x, vertices.y, vertices.z),
            colours ? glm::vec4(colours[i].r, colours[i].g, colours[i].b, colours[i].a) : glm::vec4(.7f, .4f, .1f, 1.f),
            texCoords ? glm::vec2(texCoords[i].x, texCoords[i].y) : glm::vec2(0.f),
            normals ? glm::vec3(normals[i].x, normals[i].y, normals[i].z) : glm::vec3(0.f),
            tangents ? glm::vec3(tangents[i].x, tangents[i].y, tangents[i].z) : glm::vec3(0.f),
            0,
        });
    }
    for(uint i = 0; i < meshObj->mNumFaces; ++i){ //For each face of the mesh... //Each mesh has an arr of primitive faces (triangles due to the aiProcess_Triangulate post-processing option)
        for(uint j = 0; j < meshObj->mFaces[i].mNumIndices; ++j){
            mesh.indices->emplace_back(meshObj->mFaces[i].mIndices[j]);
        }
    }
    if(meshObj->mMaterialIndex >= 0){ //LoadMtlTexs //Query the mesh's mtl index to check if the mesh contains a mtl
        for(size_t i = 0; i < texTypes.size(); ++i){
            for(uint j = 0; j < scene->mMaterials[meshObj->mMaterialIndex]->GetTextureCount(texTypes[i]); ++j){
                aiString aiStr;
                scene->mMaterials[meshObj->mMaterialIndex]->GetTexture(texTypes[i], j, &aiStr);
                switch(texTypes[i]){
                    case aiTextureType_DIFFUSE:
                        mesh.AddTexMap({"Imgs/" + str{aiStr.C_Str()}, Mesh::TexType::Diffuse, 0});
                        break;
                    case aiTextureType_SPECULAR:
                        mesh.AddTexMap({"Imgs/" + str{aiStr.C_Str()}, Mesh::TexType::Spec, 0});
                        break;
                    case aiTextureType_EMISSIVE:
                        mesh.AddTexMap({"Imgs/" + str{aiStr.C_Str()}, Mesh::TexType::Emission, 0});
                        break;
                    case aiTextureType_AMBIENT:
                        mesh.AddTexMap({"Imgs/" + str{aiStr.C_Str()}, Mesh::TexType::Reflection, 0});
                        break;
                    case aiTextureType_HEIGHT:
                        mesh.AddTexMap({"Imgs/" + str{aiStr.C_Str()}, Mesh::TexType::Bump, 0});
                        break;
                }
            }
        }
    }
    return mesh;
}

void Model::AddModelMatForAll(const glm::mat4& modelMat){
    modelMatsForAll.emplace_back(modelMat);
}

void Model::AddColorForAll(const glm::vec3& color){
    colorsForAll.emplace_back(color);
}

void Model::AddDiffuseTexIndexForAll(const int diffuseTexIndex){
    diffuseTexIndicesForAll.emplace_back(diffuseTexIndex);
}

void Model::ReserveModelMatsForAll(const size_t& size){
    modelMatsForAll.reserve(size);
}

void Model::ResizeModelMatsForAll(const size_t& size){
    modelMatsForAll.resize(size);
}

void Model::ReserveColorsForAll(const size_t& size){
    colorsForAll.reserve(size);
}

void Model::ResizeColorsForAll(const size_t& size){
    colorsForAll.resize(size);
}

void Model::ReserveDiffuseTexIndicesForAll(const size_t& size){
    diffuseTexIndicesForAll.reserve(size);
}

void Model::ResizeDiffuseTexIndicesForAll(const size_t& size){
    diffuseTexIndicesForAll.resize(size);
}

void Model::InstancedRender(ShaderProg& SP, const int& primitive){
    if(primitive < 0){
        return (void)puts("Invalid primitive!\n");
    }
    if(!meshes.size()){
        LoadModel();
    }
    const size_t& size = meshes.size();
    for(size_t i = 0; i < size; ++i){
        meshes[i].primitive = primitive;
        meshes[i].model = modelForAll;

        meshes[i].ReserveModelMats(modelMatsForAll.size());
        meshes[i].ReserveColors(colorsForAll.size());
        meshes[i].ReserveDiffuseTexIndices(diffuseTexIndicesForAll.size());

        meshes[i].modelMats = modelMatsForAll;
        meshes[i].colors = colorsForAll;
        meshes[i].diffuseTexIndices = diffuseTexIndicesForAll;

        meshes[i].InstancedRender(SP);
    }
}

void Model::Render(ShaderProg& SP, const int& primitive){
    if(primitive < 0){
        return (void)puts("Invalid primitive!\n");
    }
    if(!meshes.size()){
        LoadModel();
    }
    const size_t& size = meshes.size();
    for(size_t i = 0; i < size; ++i){
        meshes[i].primitive = primitive;
        meshes[i].model = modelForAll;
        meshes[i].Render(SP);
    }
}

void Model::SetModelForAll(const glm::mat4& model){
    this->modelForAll = model;
}