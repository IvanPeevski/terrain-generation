

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "TerrainChunk.h"
#include "PerlinNoise.h"
#include <vector>
#include <thread>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title);
void generateVertices(int terrain_width, int terrain_height, float* noise, float* vertices);
void generateIndices(int terrain_width, int terrain_height, unsigned int* indices);
void generateBuffers(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO, int verticesSize, int indicesSize, float* vertices, unsigned int* indices);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
GLuint getWaterVAO();
void generateSeed(int chunkSize, int numChunks, float* seed);
void generateTerrainChunks(int numChunks, int chunkSize, float* noise, std::vector<TerrainChunk>& chunks);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void toggleCursor(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

int main() {
    GLFWwindow* window = initWindow("Terrain Generation");
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // ImGui initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 1.5f;


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader terrainShader("terrain.vert", "terrain.frag");
    Shader waterShader("water.vert", "water.frag");
    Camera camera(glm::vec3(-2.0f, -1.0f, -2.0f));
    glfwSetWindowUserPointer(window, &camera);

    int chunkSize = 256;
    int numChunks = 8;
    int octaves = 5;
    float bias = 0.5f;

    // Vector to store terrain chunks
    std::vector<TerrainChunk> chunks;

    // Initialize terrain chunks
    float* seed = new float[chunkSize * numChunks * chunkSize * numChunks];
    generateSeed(chunkSize, numChunks, seed);

    float* noise = PerlinNoise2D(chunkSize * numChunks, chunkSize * numChunks, seed, octaves, bias);

    generateTerrainChunks(numChunks, chunkSize, noise, chunks);

    delete[] noise;


    GLuint waterVAO = getWaterVAO();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();

        camera.ProcessKeyboard(window, currentFrame);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI code
        ImGui::SetNextWindowSize(ImVec2(600, 400));
        ImGui::Begin("Terrain Generation Settings");
        ImGui::Text("Press right mouse button to toggle cursor");
        if (ImGui::Button("New Seed")) {
            generateSeed(chunkSize, numChunks, seed);
            noise = PerlinNoise2D(chunkSize * numChunks, chunkSize * numChunks, seed, octaves, bias);
            chunks.clear();
            generateTerrainChunks(numChunks, chunkSize, noise, chunks);
            
        }
        ImGui::SliderInt("Chunk Size", &chunkSize, 64, 512);
        ImGui::SliderInt("Number of Chunks", &numChunks, 1, 16);
        ImGui::SliderInt("Octaves", &octaves, 1, 10);
        ImGui::SliderFloat("Bias", &bias, 0.1f, 1.0f);
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        terrainShader.use();

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        terrainShader.setMat4("view", view);
        terrainShader.setMat4("projection", projection);


        // Draw each terrain chunk
        for (auto& chunk : chunks) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), chunk.getPosition());
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            terrainShader.setMat4("model", model);
            chunk.draw(terrainShader);
        }

        //Water terrain
        waterShader.use();
        glm::mat4 waterModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f));
        waterModel = glm::scale(waterModel, glm::vec3(100.0f, 100.0f, 100.0f));
        waterShader.setMat4("view", view);
        waterShader.setMat4("projection", projection);
        waterShader.setMat4("model", waterModel);

        glBindVertexArray(waterVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void toggleCursor(GLFWwindow* window) {
    static bool cursorEnabled = false;
    cursorEnabled = !cursorEnabled;
    glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Toggle cursor on right mouse button press
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        toggleCursor(window);
    }
}

GLuint getWaterVAO() {
    GLfloat waterVertices[] = {
        // Positions
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f,  0.5f,
    };
    GLuint waterVBO;
    glGenBuffers(1, &waterVBO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);

    GLuint waterVAO;
    glGenVertexArrays(1, &waterVAO);
    glBindVertexArray(waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    return waterVAO;
}

void generateSeed(int chunkSize, int numChunks, float* seed) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < chunkSize * numChunks * chunkSize * numChunks; i++) {
        seed[i] = static_cast<float>(rand()) / RAND_MAX;
    }

    for (int i = 0; i < chunkSize * numChunks; i++) {
        seed[i * chunkSize * numChunks] = 0;
        seed[i * chunkSize * numChunks + chunkSize - 1] = 0;
        seed[i] = 0;
        seed[(chunkSize * numChunks - 1) * chunkSize * numChunks + i] = 0;
    }
};

void generateTerrainChunks(int numChunks, int chunkSize, float* noise, std::vector<TerrainChunk>& chunks) {

    for (int x = 0; x < numChunks; x++) {
        for (int y = 0; y < numChunks; y++) {
            float* chunkNoise = new float[chunkSize * chunkSize];
            for (int i = 0; i < chunkSize; i++) {
                for (int j = 0; j < chunkSize; j++) {
                    chunkNoise[i * chunkSize + j] = noise[((y * chunkSize + i) * chunkSize * numChunks) + (x * chunkSize + j)];
                }
            }
            glm::vec3 chunkPosition = glm::vec3(x * 0.5, 0, y);
            TerrainChunk chunk(chunkPosition, chunkSize, chunkSize);
            chunk.generateTerrain(chunkNoise);
            chunks.push_back(std::move(chunk));

            delete[] chunkNoise;
        }
    }
}

//inits windoww
GLFWwindow* initWindow(const char* title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);

    return window;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		return;
	}

    static float lastX = 800 / 2.0;
    static float lastY = 600 / 2.0;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->ProcessMouseMovement(xoffset, yoffset);
}


// execute on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
