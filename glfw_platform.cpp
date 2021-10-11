//
// Created by kouta on 28/10/20.
//
#include "glfw_platform.h"
#include "glfwInitialization.h"
#include <iostream>

static gltactics::input_status status;

void gltactics::glfw_platform::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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

glm::mat4 gltactics::glfw_platform::getTranslateMVPmat(float x, float y, float z) const {
    glm::mat4 vp = projection * getViewMat();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    glm::mat4 mvp = vp * model;

    return mvp;
}


glm::mat4 gltactics::glfw_platform::getTranslateRotateMVPmat(float x, float y, float z, float ry) const {
    glm::mat4 vp = projection * getViewMat();

    glm::mat4 rotateMat = getModelMat(x, y, z, ry);
    glm::mat4 mvp = vp * rotateMat;
    return mvp;
}

glm::mat4 gltactics::glfw_platform::getModelMat(float x, float y, float z, float ry) const {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    glm::mat4 rotateMat = glm::rotate(model, ry, glm::vec3(0, 1, 0));
    return rotateMat;
}

glm::mat4 gltactics::glfw_platform::getViewMat() const {
    const std::array<size_t, 2> &pos = this->manager.getPlayerCharacter().position();
    unsigned long posX = pos[1];
    unsigned long posY = pos[0];
    glm::mat4 view = glm::lookAt(glm::vec3(this->camX + posX, this->camY, this->camZ + posY),
                                 glm::vec3(posX, 0, posY),
                                 glm::vec3(0, -1, 0));
    return view;
}

void gltactics::glfw_platform::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float color[3] = {1.0, 0.0, 0.0};


//    for (int x = 0; x < DEFAULT_MAPSIZE; x++) {
//        for (int y = 0; y < DEFAULT_MAPSIZE; y++) {
//            setMVPTransformation(x, 0, y, this->CCSMatrixId);
//            drawTriangleArray(floorVertexBuffer, floorVertexCount, color);
//        }
//    }

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;

    for (size_t x = 0; x < DEFAULT_MAPSIZE; x++) {
        for (size_t y = 0; y < DEFAULT_MAPSIZE; y++) {
            if (manager.getMap()[{y, x}].tileType == gltactics::type::WALL) {
                wall->addUniformFunction("MVP", [this, x, y](GLuint modelUniform, const glMesh &mesh) {
                    const glm::mat4 &pmat = getTranslateMVPmat(x, 0, y);
                    glUniformMatrix4fv(modelUniform, 1, false, &pmat[0][0]);
                });
                wall->draw();
            }
        }
    }

    color[0] = 1.0f;
    color[1] = 0.5f;
    color[2] = 0.25f;

    for (size_t x = 0; x < DEFAULT_MAPSIZE; x++) {
        for (size_t y = 0; y < DEFAULT_MAPSIZE; y++) {
            if (manager.getMap()[{y, x}].tileType == gltactics::type::EXIT) {
                stair->addUniformFunction("Model", [this, x, y](GLuint modelUniform, const glMesh &mesh) {
                    const glm::mat4 &pmat = getModelMat(x, 0, y, 0);
                    glUniformMatrix4fv(modelUniform, 1, false, &pmat[0][0]);
                });
                stair->draw();
            }
        }
    }

    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 0.0f;

    for (size_t x = 0; x < DEFAULT_MAPSIZE; x++) {
        for (size_t y = 0; y < DEFAULT_MAPSIZE; y++) {
            tile &tile = manager.getMap()[{y, x}];
            if (tile.tileType == gltactics::type::DOOR) {
                float dY = y;
                float dX = x;
                float angle = tile.isHorizontal() ? 0 : doorRotation;
                if (tile.isOpen()) {
                    if (tile.isHorizontal())
                        dX += 0.35f;
                    else
                        dY += 0.35f;
                    angle += doorRotation;
                }
                door->addUniformFunction("MVP", [this, dX, dY, angle](GLuint modelUniform, const glMesh &mesh) {
                    glUniformMatrix4fv(modelUniform, 1, false,
                                       &(this->getTranslateRotateMVPmat(dX, 0, dY, angle)[0][0]));
                });
                door->draw();
            }
        }
    }

    color[0] = 0.0f;
    color[1] = 1.0f;
    color[2] = 0.0f;


    const std::array<size_t, 2> &pos = manager.getPlayerCharacter().position();

    glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(pos[1], 0, pos[0]));
    glm::mat4 viewMat = getViewMat();
    player->addUniformFunction("Model", [this, pos](GLuint modelUniform, const glMesh &mesh) {
        glUniformMatrix4fv(modelUniform, 1, false, &(this->getModelMat(pos[1], 0, pos[0], 0)[0][0]));
    });
    player->draw();

    glfwSwapBuffers(this->window);
}

//void gltactics::glfw_platform::drawTriangleArray(GLuint vertexBuffer, size_t vertexCount, float color[]) {
//    glUseProgram(ConstantColorShader);
//
//    glUniform3fv(CCSModelColor, 1, color);
//    glEnableVertexAttribArray(0);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//    glVertexAttribPointer(
//            0,
//            3,
//            GL_FLOAT,
//            GL_FALSE,
//            0,
//            nullptr
//    );
//    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
//
//    glDisableVertexAttribArray(0);
//}
//


gltactics::input_status gltactics::glfw_platform::get_input() {
    glfwPollEvents();

    if (glfwWindowShouldClose(this->window)) {
        glfwDestroyWindow(this->window);
        exit(0);
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


void gltactics::glMesh::draw() {
    glUseProgram(_shader.programId);

    for (auto uniformSetup : this->uniformGetters) {
        uniformSetup.second();
    }
    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, nullptr);
}