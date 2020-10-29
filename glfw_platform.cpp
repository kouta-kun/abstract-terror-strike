//
// Created by kouta on 28/10/20.
//
#include "glfw_platform.h"
#include <iostream>

gltactics::model gltactics::model::playerModel = gltactics::model("player.obj");
gltactics::model gltactics::model::wallModel = gltactics::model("wallModel.obj");

static gltactics::input_status status;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        status.
                movementDirection = gltactics::direction::up;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        status.
                movementDirection = gltactics::direction::down;
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        status.
                movementDirection = gltactics::direction::left;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        status.
                movementDirection = gltactics::direction::right;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        status.
                items = true;
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        status.
                environment = true;
    }
}

gltactics::model::model(std::string input_file) {
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

void gltactics::model::insertNode(const aiNode *node, const aiScene *g_scene) {
    for (uint n = 0; n < node->mNumMeshes; n++) {
        const struct aiMesh *mesh = g_scene->mMeshes[node->mMeshes[n]];

        for (uint t = 0; t < mesh->mNumFaces; t++) {
            const struct aiFace *face = &mesh->mFaces[t];
            for (uint idx = 0; idx < face->mNumIndices; idx++) {
                this->indexes.push_back(this->vertex.size() / 3 + face->mIndices[idx]);
            }
        }
        for (uint idx = 0; idx < mesh->mNumVertices; idx++) {
            glm::vec4 vert = glm::vec4(mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z, 1);
            this->vertex.push_back(vert.x);
            this->vertex.push_back(vert.y);
            this->vertex.push_back(vert.z);
        }
    }
    for (uint n = 0; n < node->mNumChildren; n++) {
        insertNode(node->mChildren[n], g_scene);
    }
}

gltactics::glfw_platform::glfw_platform(gltactics::game_manager &manager) : manager{manager} {
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

void gltactics::glfw_platform::loadShaders() {
    this->ShaderProgram = gltactics::LoadShaders("vertexShader.glsl", "fragmentShader.glsl");
    this->MatrixID = glGetUniformLocation(this->ShaderProgram, "MVP");
    this->ModelColor = glGetUniformLocation(this->ShaderProgram, "ModelColor");
}

void gltactics::glfw_platform::makeBuffers() {
    glGenVertexArrays(1, &VertexArrayId);
    glBindVertexArray(this->VertexArrayId);
    glGenBuffers(1, &this->floorVertexBuffer);
    glGenBuffers(1, &this->charVertexBuffer);
    glGenBuffers(1, &this->charElementBuffer);
    glGenBuffers(1, &this->wallVertexBuffer);
    glGenBuffers(1, &this->wallElementBuffer);
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
    makeBuffers();
    loadShaders();
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorBlockModel), floorBlockModel, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, charVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, model::playerModel.vertex.size() * sizeof(float),
                 model::playerModel.vertex.data(), GL_STATIC_DRAW);

    this->charIndexCount = model::playerModel.indexes.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, charElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, charIndexCount * sizeof(unsigned int),
                 model::playerModel.indexes.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, wallVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, model::wallModel.vertex.size() * sizeof(float),
                 model::wallModel.vertex.data(), GL_STATIC_DRAW);

    this->wallIndexCount = model::wallModel.indexes.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, wallIndexCount * sizeof(unsigned int),
                 model::wallModel.indexes.data(), GL_STATIC_DRAW);

    glfwSetKeyCallback(this->window, key_callback);
}

glm::mat4 gltactics::glfw_platform::getMVPmat(float x, float y, float z) const {
    const std::array<size_t, 2> &pos = manager.getPlayerCharacter().position();
    unsigned long posX = pos[1];
    unsigned long posY = pos[0];
    glm::mat4 view = glm::lookAt(glm::vec3(camX + posX, camY, camZ + posY),
                                 glm::vec3(posX, 0, posY),
                                 glm::vec3(0, -1, 0));

    std::cout << "\rCamera pos: (" << camX << ',' << camY << ',' << camZ << ");";
    std::cout.flush();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    glm::mat4 mvp = this->projection * view * model;

    return mvp;
}

void gltactics::glfw_platform::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float color[3] = {1.0, 0.0, 0.0};

    glUseProgram(ShaderProgram);

    glUniform3fv(ModelColor, 1, color);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuffer);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );

    for (int x = 0; x < DEFAULT_MAPSIZE; x++) {
        for (int y = 0; y < DEFAULT_MAPSIZE; y++) {
            glm::mat4 mvp = getMVPmat(x, 0, y);
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(floorBlockModel) / sizeof(float));
        }
    }

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;
    glUniform3fv(ModelColor, 1, color);

    glBindBuffer(GL_ARRAY_BUFFER, wallVertexBuffer);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallElementBuffer);

    for (size_t x = 0; x < DEFAULT_MAPSIZE; x++) {
        for (size_t y = 0; y < DEFAULT_MAPSIZE; y++) {
            if (manager.getMap()[{y, x}].tileType == gltactics::type::WALL) {
                glm::mat4 mvp = getMVPmat(x, 0, y);
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
                glDrawElements(GL_TRIANGLES, wallIndexCount, GL_UNSIGNED_INT, nullptr);
            }
        }
    }

    color[0] = 0.0f;
    color[1] = 1.0f;
    color[2] = 0.0f;
    glUniform3fv(ModelColor, 1, color);

    const std::array<size_t, 2> &pos = manager.getPlayerCharacter().position();
    glm::mat4 mvp = getMVPmat(pos[1], 0, pos[0]);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, charVertexBuffer);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, charElementBuffer);
    glDrawElements(GL_TRIANGLES, charIndexCount, GL_UNSIGNED_INT, nullptr);

    glDisableVertexAttribArray(0);
    glfwSwapBuffers(this->window);
}


gltactics::input_status gltactics::glfw_platform::get_input() {
    glfwPollEvents();
    if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_PRESS) {
        camX += 0.5;
    }
    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) {
        camX -= 0.5;
    }
    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
        camY += 0.5;
    }
    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
        camY -= 0.5;
    }
    if (glfwGetKey(this->window, GLFW_KEY_E) == GLFW_PRESS) {
        camZ += 0.5;
    }
    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) {
        camZ -= 0.5;
    }
    input_status iStatus = status;
    status = input_status{};
    return iStatus;
}

GLuint gltactics::LoadShaders(const char *vertex_file_path, const char *fragment_file_path) {
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShaderCode;
    std::ifstream vertexShaderStream(vertex_file_path, std::ios::in);
    if (vertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << vertexShaderStream.rdbuf();
        vertexShaderCode = sstr.str();
        vertexShaderStream.close();
    } else return -1;

    std::string fragmentShaderCode;
    std::ifstream fragmentShaderStream(fragment_file_path, std::ios::in);
    if (fragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << fragmentShaderStream.rdbuf();
        fragmentShaderCode = sstr.str();
        fragmentShaderStream.close();
    } else return -1;
    GLint result = GL_FALSE;
    int infoLogLength;
    char const *vertexSourcePtr = vertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &vertexSourcePtr, NULL);
    glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, infoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    char const *fragmentSourcePtr = fragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &fragmentSourcePtr, NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, infoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    GLuint programID = glCreateProgram();
    glAttachShader(programID, VertexShaderID);
    glAttachShader(programID, FragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(programID, VertexShaderID);
    glDetachShader(programID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return programID;
}
