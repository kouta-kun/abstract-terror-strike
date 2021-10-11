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
#include <cmath>

namespace gltactics {
    static const int width = 800;

    static const int height = 600;

    GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path);

    struct shader {
        GLuint programId;

        std::map<std::string, GLuint> uniforms;

        shader(std::string vertex_file, std::string fragment_file, std::vector<std::string> uniformNames);

        inline shader() {}
    };

    struct mesh {
        std::vector<float> vertex;
        std::vector<float> normals;
        std::vector<unsigned int> indexes;

        mesh(std::string input_file);

        void insertNode(const aiNode *node, const aiScene *scene);

    public:

        static mesh playerModel;
        static mesh wallModel;
        static mesh doorModel;
        static mesh stairModel;
    };

    struct glMesh {
        shader &_shader;

        std::map<std::string, std::function<void(void)>> uniformGetters;

        GLuint vaoId;
        GLuint elementBufferId;
        size_t elementCount;
        GLuint vertexBufferId;
        GLuint normalBufferId;

        glMesh(const mesh &mesh, shader &_shader, std::map<std::string, std::function<void(GLuint, const glMesh &)>>);

        void addUniformFunction(const std::string &uniformName,
                                const std::function<void(GLuint, const glMesh &)> &uniformFn);

        void draw();
    };

    class glfw_platform : public gltactics::platform_module {

        gltactics::shader CCS;
        gltactics::shader ACS;

        std::unique_ptr<gltactics::glMesh> wall;
        std::unique_ptr<gltactics::glMesh> door;
        std::unique_ptr<gltactics::glMesh> player;
        std::unique_ptr<gltactics::glMesh> stair;

        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

        gltactics::game_manager &manager;

        size_t floorVertexCount;
        GLuint floorVertexBuffer;


//        size_t playerIndexCount;
//        GLuint playerElementBuffer;
//        GLuint playerVertexBuffer;
//        GLuint playerNormalBuffer;
//
//        size_t wallIndexCount;
//        GLuint wallElementBuffer;
//        GLuint wallVertexBuffer;
//
//        size_t doorIndexCount;
//        GLuint doorElementBuffer;
//        GLuint doorVertexBuffer;
//
//        size_t stairIndexCount;
//        GLuint stairElementBuffer;
//        GLuint stairVertexBuffer;

//        GLuint ConstantColorShader;
//        GLuint CCSMatrixId;
//        GLuint CCSModelColor;
//
//        GLuint AttributeColorShader;
//        GLuint ACSModelId;
//        GLuint ACSViewId;
//        GLuint ACSProjectionId;
//        GLuint ACSModelColor;
//        GLuint ACSLightPosition;

        static constexpr float camX = 0;
        static constexpr float camY = 12;
        static constexpr float camZ = -1;

        float doorRotation = 90 * (M_PI / 180.0f);

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

        glm::mat4 getTranslateMVPmat(float x, float y, float z) const;

        glm::mat4 getTranslateRotateMVPmat(float x, float y, float z, float ry) const;

        glm::mat4 getModelMat(float x, float y, float z, float ry) const;

        glm::mat4 getViewMat() const;

        void loadShaders();

    public:
        glfw_platform(game_manager &manager);

        void initialize() override;

        void render() override;

        input_status get_input() override;

    };
};

#endif //ABSTRACT_TERROR_STRIKE_GLFW_PLATFORM_H
