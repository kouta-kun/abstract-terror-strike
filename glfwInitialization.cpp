//
// Created by kouta on 29/10/20.
//

#include "glfwInitialization.h"
#include "glfw_platform.h"

#include <iostream>

gltactics::mesh gltactics::mesh::playerModel = mesh("player.obj");
gltactics::mesh gltactics::mesh::wallModel = mesh("wallModel.obj");
gltactics::mesh gltactics::mesh::doorModel = mesh("door.obj");
gltactics::mesh gltactics::mesh::stairModel = mesh("stair.obj");

gltactics::mesh::mesh(std::string input_file) {
    const aiScene *g_scene = nullptr;
    Assimp::Importer importer;
    g_scene = importer.ReadFile(input_file, aiProcessPreset_TargetRealtime_Quality);
    if (!g_scene) {
        std::cout << importer.GetErrorString() << std::endl;
        exit(1);
    }
    const struct aiNode *parent = g_scene->mRootNode;

    insertNode(parent, g_scene);

}

void gltactics::mesh::insertNode(const aiNode *node, const aiScene *g_scene) {
    for (uint n = 0; n < node->mNumMeshes; n++) {
        const struct aiMesh *mesh = g_scene->mMeshes[node->mMeshes[n]];

        for (uint t = 0; t < mesh->mNumFaces; t++) {
            const struct aiFace *face = &mesh->mFaces[t];
            for (uint idx = 0; idx < face->mNumIndices; idx++) {
                this->indexes.push_back(this->vertex.size() / 3 + face->mIndices[idx]);
            }
        }
        for (uint idx = 0; idx < mesh->mNumVertices; idx++) {
            glm::vec3 vert = glm::vec3(mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z);
            this->vertex.push_back(vert.x);
            this->vertex.push_back(vert.y);
            this->vertex.push_back(vert.z);
            this->normals.push_back(mesh->mNormals[idx].x);
            this->normals.push_back(mesh->mNormals[idx].y);
            this->normals.push_back(mesh->mNormals[idx].z);
        }
    }
    for (uint n = 0; n < node->mNumChildren; n++) {
        insertNode(node->mChildren[n], g_scene);
    }
}

using UniformFn = std::function<void(GLuint, const gltactics::glMesh &)>;

gltactics::glfw_platform::glfw_platform(game_manager &manager)
        : manager{manager} {
    this->window = nullptr;
}

static inline void initializeGLFW() {
    glewExperimental = GL_TRUE;
    if (!glfwInit()) {
        std::cerr << "GLFW couldn't init" << std::endl;
        exit(1);
    }
}

static inline GLFWwindow *getWindow() {
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return glfwCreateWindow(gltactics::width, gltactics::height, "Abstract Terror Strike", NULL, NULL);
}

gltactics::shader::shader(std::string vertex_file, std::string fragment_file, std::vector<std::string> uniformNames) {
    this->programId = LoadShaders(vertex_file.c_str(), fragment_file.c_str());
    for (std::string name : uniformNames) {
        this->uniforms[name] = glGetUniformLocation(this->programId, name.c_str());
    }


}

void gltactics::glfw_platform::loadShaders() {

    this->CCS = shader("noShadingVertex.glsl", "noShadingFragment.glsl",
                       {"MVP", "ModelColor"});
    this->ACS = shader("attributeColorVertex.glsl", "attributeColorFragment.glsl",
                       {"Model", "View", "Projection", "LightPosition_worldspace", "fragmentColor"});

}

template<typename T>
static size_t getByteCount(const std::vector<T> &vector) {
    return vector.size() * sizeof(T);
}

template<typename T>
static void vecToBuffer(GLuint destination, const std::vector<T> &sourceVec) {
    const void *sourcePtr = sourceVec.data();
    unsigned long byteCount = getByteCount(sourceVec);
    glBindBuffer(GL_ARRAY_BUFFER, destination);
    glBufferData(GL_ARRAY_BUFFER, byteCount,
                 sourcePtr, GL_STATIC_DRAW);
}

static void loadModel(const gltactics::mesh &model,
                      GLuint vertexDest, GLuint elementDest,
                      size_t &indexCountDest) {
    vecToBuffer(vertexDest, model.vertex);
    indexCountDest = model.indexes.size();
    vecToBuffer(elementDest, model.indexes);
}

static void loadModel(const gltactics::mesh &model,
                      GLuint vertexDest, GLuint elementDest, GLuint normalDest,
                      size_t &indexCountDest) {
    vecToBuffer(vertexDest, model.vertex);
    indexCountDest = model.indexes.size();
    vecToBuffer(elementDest, model.indexes);
    vecToBuffer(normalDest, model.normals);
}

void gltactics::glfw_platform::initialize() {
    initializeGLFW();

    this->window = getWindow();
    if (window == nullptr) {
        std::cerr << "Couldn't open window" << std::endl;
        exit(1);
    }

    glfwMakeContextCurrent(this->window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW could not init" << std::endl;
        exit(1);
    }

    loadShaders();

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    this->floorVertexCount = sizeof(floorBlockModel) / sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorBlockModel), floorBlockModel, GL_STATIC_DRAW);

//    loadModel(mesh::wallModel, wallVertexBuffer, wallElementBuffer, this->wallIndexCount);
//    loadModel(mesh::doorModel, doorVertexBuffer, doorElementBuffer, this->doorIndexCount);
//    loadModel(mesh::stairModel, stairVertexBuffer, stairElementBuffer, this->stairIndexCount);

    this->wall = std::make_unique<gltactics::glMesh>(mesh::wallModel, CCS,
                                                     std::map<std::string, UniformFn>{
                                                             {"ModelColor", [](
                                                                     GLuint modelColorUniform,
                                                                     const glMesh &mesh) {
                                                                 float col[] = {1.0f, 1.0f, 1.0f};
                                                                 glUniform3fv(modelColorUniform, 1, col);
                                                             }}});
    this->door = std::make_unique<gltactics::glMesh>(mesh::doorModel, CCS,
                                                     std::map<std::string, UniformFn>{
                                                             {"ModelColor", [](
                                                                     GLuint modelColorUniform,
                                                                     const glMesh &mesh) {
                                                                 float col[] = {1.0f, 1.0f, 0.0f};
                                                                 glUniform3fv(modelColorUniform, 1, col);
                                                             }}});
    this->player = std::make_unique<gltactics::glMesh>(mesh::playerModel, ACS,
                                                       std::map<std::string, UniformFn>{
                                                               {"fragmentColor",            [](
                                                                       GLuint modelColorUniform,
                                                                       const glMesh &mesh) {
                                                                   float col[] = {0.0f, 1.0f, 0.0f};
                                                                   glUniform3fv(modelColorUniform, 1, col);
                                                               }},
                                                               {"LightPosition_worldspace", [this](
                                                                       GLuint lightPosUniform,
                                                                       const glMesh &mesh) {
                                                                   std::array<size_t, 2> array = this->manager.getPlayerCharacter().position();
                                                                   float pos[] = {(float) array[0], 3.0f,
                                                                                  (float) array[1]};
                                                                   glUniform3fv(lightPosUniform, 1, pos);
                                                               }},
                                                               {"Projection",               [this](
                                                                       GLuint modelUniform,
                                                                       const glMesh &mesh) {
                                                                   glUniformMatrix4fv(modelUniform, 1, false,
                                                                                      &(this->projection[0][0]));
                                                               }},
                                                               {"View",                     [this](
                                                                       GLuint modelUniform,
                                                                       const glMesh &mesh) {
                                                                   glUniformMatrix4fv(modelUniform, 1, false,
                                                                                      &(this->getViewMat()[0][0]));
                                                               }}});
    this->
            stair = std::make_unique<gltactics::glMesh>(mesh::stairModel, ACS,
                                                        std::map<std::string, UniformFn>{
                                                                {"fragmentColor",            [](
                                                                        GLuint modelColorUniform,
                                                                        const glMesh &mesh) {
                                                                    float col[] = {0.0f,
                                                                                   1.0f,
                                                                                   1.0f};
                                                                    glUniform3fv(
                                                                            modelColorUniform,
                                                                            1, col);
                                                                }},
                                                                {"LightPosition_worldspace", [this](
                                                                        GLuint lightPosUniform,
                                                                        const glMesh &mesh) {
                                                                    std::array<size_t, 2> array = this->manager.
                                                                            getPlayerCharacter().position();
                                                                    float pos[] = {(float) array[0], 3.0f,
                                                                                   (float) array[1]};
                                                                    glUniform3fv(lightPosUniform, 1, pos);
                                                                }},
                                                                {"Projection",               [this](
                                                                        GLuint modelUniform,
                                                                        const glMesh &mesh) {
                                                                    glUniformMatrix4fv(modelUniform, 1, false,
                                                                                       &(this->projection[0][0]));
                                                                }},
                                                                {"View",                     [this](
                                                                        GLuint modelUniform,
                                                                        const glMesh &mesh) {
                                                                    glUniformMatrix4fv(modelUniform, 1, false,
                                                                                       &(this->getViewMat()[0][0]));
                                                                }}});
    glfwSetKeyCallback(this->window, key_callback);
}


gltactics::glMesh::glMesh(const gltactics::mesh &mesh, gltactics::shader &shader,
                          std::map<std::string, std::function<void(GLuint, const glMesh &)>> uniformGetters)
        : _shader(shader) {
    GLuint glVal[1] = {0};
    this->vaoId = 0;
    glGenVertexArrays(1, glVal);
    vaoId = glVal[0];
    glBindVertexArray(this->vaoId);

    glGenBuffers(1, &vertexBufferId);
    glGenBuffers(1, &elementBufferId);
    glGenBuffers(1, &normalBufferId);

    loadModel(mesh, vertexBufferId, elementBufferId, normalBufferId, elementCount);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
    glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);

    for (auto i : uniformGetters) {
        const std::string &uniformName = i.first;
        std::function<void(GLuint, const glMesh &)> uniformFn = i.second;
        addUniformFunction(uniformName, uniformFn);
    }
}

void gltactics::glMesh::addUniformFunction(const std::string &uniformName,
                                           const std::function<void(GLuint, const glMesh &)> &uniformFn) {
    GLuint uniformLocation = this->_shader.uniforms[uniformName];
    this->uniformGetters[uniformName] = ([uniformLocation, uniformFn, this]() {
        uniformFn(uniformLocation, *this);
    });
}

