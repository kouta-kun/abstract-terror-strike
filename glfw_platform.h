//
// Created by kouta on 28/10/20.
//

#ifndef ABSTRACT_TERROR_STRIKE_GLFW_PLATFORM_H
#define ABSTRACT_TERROR_STRIKE_GLFW_PLATFORM_H

#include <GL/glew.h>
#include <memory>
#include <platform_module.h>
#include <game_manager.h>
#include <map.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <string>
#include <fstream>
#include <sstream>

namespace gltactics {
    static const int width = 800;

    static const int height = 600;

    GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path);

    struct model {
        std::vector<float> vertex;
        std::vector<unsigned int> indexes;

        model(std::string input_file);

        void insertNode(const aiNode *node, const aiScene *scene);

    public:
        static model playerModel;
        static model wallModel;
    };

    class glfw_platform : public gltactics::platform_module {
        gltactics::game_manager &manager;

        GLuint VertexArrayId;

        GLuint floorVertexBuffer;

        size_t charIndexCount;
        GLuint charElementBuffer;
        GLuint charVertexBuffer;

        size_t wallIndexCount;
        GLuint wallElementBuffer;
        GLuint wallVertexBuffer;

        GLuint ShaderProgram;
        GLuint MatrixID;
        GLuint ModelColor;

        float camX = 0;
        float camY = 12;
        float camZ = -1;

        float floorBlockModel[6 * 3] = {
                -0.5f, 0.0f, 0.5f,//t1
                -0.5f, 0.0f, -0.5f,
                0.5f, 0.0f, 0.5f,
                -0.5f, 0.0f, -0.5f,//t2
                0.5f, 0.0f, 0.5f,
                0.5f, 0.0f, -0.5f,
        };

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);


        GLFWwindow *window;

        glm::mat4 getMVPmat(float x, float y, float z) const;

        void loadShaders();

        void makeBuffers();

    public:
        glfw_platform(game_manager &manager);

        void initialize() override;

        void render() override;

        input_status get_input() override;

    };
};

#endif //ABSTRACT_TERROR_STRIKE_GLFW_PLATFORM_H
