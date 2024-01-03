#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "InitShader.h"
#include "Framebuffer.h"
#include "Utilities.h"
#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

using namespace std;

// camera parameters
int screen_width, screen_height;
int w, h;
int screen_scale = 3;
int viewport_width, viewport_height;
float focal_length;
glm::vec3 camera_center;

// shader variables
static const std::string vertex_shader("vertex.glsl");
static const std::string fragment_shader("fragment.glsl");
glm::u8vec3 bgColor = glm::u8vec3(255, 167, 50);
GLuint shader_program = -1;
GLuint uniXform = -1;
GLuint texture = -1;
GLsizei vcount = 0;
GLuint vao, vbo, ibo;

// framebuffer
Framebuffer* buf;
Camera cam;


void reload_shader()
{
    GLuint new_shader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());

    if (new_shader == -1) // loading failed
    {
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f); //change clear color if shader can't be compiled
    }
    else
    {
        glClearColor(0.521, 0.576, 0.596, 0.0f);

        if (shader_program != -1)
        {
            glDeleteProgram(shader_program);
        }
        shader_program = new_shader;
    }
}

void initData()
{
    buf = new Framebuffer(w, h);
}

void initOpenGL()
{
    glewInit();

    //Print out information about the OpenGL version supported by the graphics driver.	
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // shader
    reload_shader();
    
    // quad mesh
    struct vert {
        glm::vec3 pos;
        glm::vec2 tc;
    };
    vector<vert> verts = {
        { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
        { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
        { glm::vec3(1.0f,  1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
    };
    vector<GLubyte> indices = { 0, 1, 2, 2, 3, 0 };
    vcount = indices.size();

    // Create vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // Create vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vert), verts.data(), GL_STATIC_DRAW);
    // Specify vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (GLvoid*)sizeof(glm::vec3));
    // Create index buffer
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLubyte), indices.data(), GL_STATIC_DRAW);

    // Cleanup state
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create texture object
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buf->texData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // uniform
    glUseProgram(shader_program);
    uniXform = glGetUniformLocation(shader_program, "xform");
    GLuint tex_id = glGetUniformLocation(shader_program, "tex");
    glUniform1i(tex_id, 0);
    glUseProgram(0);

    // physics
    // camera
    // init vao and vbo

    assert(glGetError() == GL_NO_ERROR);
}

void initImgui(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void drawImgui(GLFWwindow* window)
{
    //Begin ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //Draw Gui
    ImGui::Begin("Debug window");
    if (ImGui::Button("Quit"))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (ImGui::Button("Export PNG"))
    {
        buf->saveToPNG("./images/output_final.png");
    }
    


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    /*static bool show_test = false;
    ImGui::ShowDemoWindow(&show_test);*/

    //End ImGui Frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "key : " << key << ", " << char(key) << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }
    }
}

void idle()
{

}

void cleanup()
{
    // delete framebuffer;
    delete buf;
}

void display(GLFWwindow* window)
{
    //Clear the screen to the color previously specified in the glClearColor(...) call.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glViewport(0, 0, viewport_width, viewport_height);

    glUseProgram(shader_program);

    // Fix aspect ratio (trick)
    glm::mat4 xform(1.0f);
    float winAspect = (float)w/ (float)h;
    float texAspect = (float)viewport_width / (float)viewport_height;
    /*xform[0][0] = glm::min(1.0f, texAspect / winAspect);
    xform[1][1] = glm::min(1.0f, -winAspect / texAspect);*/
    xform[0][0] = glm::min(0.5f, 0.5f * texAspect / winAspect);
    xform[1][1] = glm::min(-0.5f, 0.5f * (1.0f - (winAspect / texAspect)));
    /*xform[0][0] = 1.0;
    xform[1][1] = -1.0;*/
    
    // Send transformation matrix to shader
    glUniformMatrix4fv(uniXform, 1, GL_FALSE, value_ptr(xform));

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buf->texData.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, vcount, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    
    drawImgui(window);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
}

void render()
{
    HittableList world;

    auto ground_material = make_shared<Lambertian>(glm::vec3(0.5, 0.5, 0.5));
    world.add(make_shared<Sphere>(glm::vec3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            glm::vec3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

            if (glm::length(center - glm::vec3(4, 0.2, 0)) > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random() * random();
                    sphere_material = make_shared<Lambertian>(albedo);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(glm::vec3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(glm::vec3(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0, material3));

    /*auto material_ground = make_shared<Lambertian>(glm::vec3(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5));
    auto material_left = make_shared<Dielectric>(1.5);
    auto material_right = make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 0.0);
    
    world.add(make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), -0.4, material_left));
    world.add(make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, material_right));*/

    cam.image_width = w;
    cam.image_height = h;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = glm::vec3(13, 2, 3);
    cam.lookat = glm::vec3(0, 0, 0);
    cam.vup = glm::vec3(0, 1, 0);

    // set up defocus blur
    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.render(buf, world);
}

int main() 
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    /* Get physical size of the current monitor */
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    glfwGetMonitorPhysicalSize(monitor, &screen_width, &screen_height);
    std::cout << "screen w and h: " << screen_width << ", " << screen_height<< std::endl;

    /* Create a windowed mode window and its OpenGL context */
    w = screen_width * screen_scale;
    h = screen_height* screen_scale;
    window = glfwCreateWindow(w, h, "OneWeekRayTracing", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // init frambuffer and camera
    initData();

    initOpenGL();

    initImgui(window);
    
    // ray trace
    render();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        idle();

        // ray trace
        //render();

        display(window);
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    cleanup();

    return 0;

    
    //// Image

    //int image_width = 256;
    //int image_height = 256;

    //// Render

    //std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    //for (int j = 0; j < image_height; ++j) {
    //    for (int i = 0; i < image_width; ++i) {
    //        auto r = double(i) / (image_width - 1);
    //        auto g = double(j) / (image_height - 1);
    //        auto b = 0;

    //        int ir = static_cast<int>(255.999 * r);
    //        int ig = static_cast<int>(255.999 * g);
    //        int ib = static_cast<int>(255.999 * b);

    //        std::cout << ir << ' ' << ig << ' ' << ib << '\n';
    //    }
    //}

    //return 0;
    
}
