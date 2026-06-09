#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.hpp"
#include "video_reader.hpp"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

void processInput(GLFWwindow *window) {
    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int screenWidth = 800, screenHeight = 600;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, width, height);
}

int main() {
    // Initialize window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window =
        glfwCreateWindow(screenWidth, screenHeight, "Bad Apple", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    Shader shader("assets/shaders/shader.vert", "assets/shaders/shader.frag");

    VideoReader videoReader("scripts/bad_apple.bin");

    float videoAspect =
        (float)videoReader.header.width / (float)videoReader.header.height;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, videoReader.header.width,
                 videoReader.header.height, 0, GL_RED, GL_UNSIGNED_BYTE,
                 nullptr);

    float vertices[] = {
        // Position   Texture coords
        -1.0f, -1.0f, 0.0f, 1.0f, // bottom-left
        1.0f,  -1.0f, 1.0f, 1.0f, // bottom-right
        1.0f,  1.0f,  1.0f, 0.0f, // top-right
        -1.0f, 1.0f,  0.0f, 0.0f, // top-left
    };

    GLuint elements[] = {
        0, 1, 2, // bottom triangle
        2, 3, 0  // top triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader.use();

    const double fps = 30;
    const double frameTime = 1 / fps;
    double startTime = glfwGetTime();

    unsigned int uScaleLoc = glGetUniformLocation(shader.id, "uScale");

    // Setup audio playback
    ma_engine audioEngine;
    ma_sound videoSound;

    ma_result result = ma_engine_init(NULL, &audioEngine);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine\n";
        return -1;
    }

    // Preload first frame to prevent showing white while audio engine boots up
    std::span<std::uint8_t> firstFrame = videoReader.getFrameData(0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoReader.header.width,
                    videoReader.header.height, GL_RED, GL_UNSIGNED_BYTE,
                    firstFrame.data());

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    ma_sound_init_from_file(&audioEngine, "scripts/audio.wav",
                            MA_SOUND_FLAG_DECODE, NULL, NULL, &videoSound);
    ma_sound_start(&videoSound);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        float currentAudioTime;
        ma_sound_get_cursor_in_seconds(&videoSound, &currentAudioTime);

        std::uint32_t frameIndex =
            static_cast<std::uint32_t>(currentAudioTime * fps);
        if (frameIndex >= videoReader.indexTable.size()) {
            frameIndex = videoReader.indexTable.size() - 1;
            ma_sound_stop(&videoSound);
        }

        std::span<std::uint8_t> newFrame = videoReader.getFrameData(frameIndex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoReader.header.width,
                        videoReader.header.height, GL_RED, GL_UNSIGNED_BYTE,
                        newFrame.data());

        float screenAspect = (float)screenWidth / (float)screenHeight;

        float scaleX = 1.0f;
        float scaleY = 1.0f;

        if (screenAspect > videoAspect) {
            scaleX = videoAspect / screenAspect;
        } else {
            scaleY = screenAspect / videoAspect;
        }

        glUniform2f(uScaleLoc, scaleX, scaleY);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    ma_sound_uninit(&videoSound);
    ma_engine_uninit(&audioEngine);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
