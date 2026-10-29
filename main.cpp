#include<Engine.h>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>

#include "BoidTriangle.h"
#include "Boid.h"

float Align_Weight = 0.8;
float Cohesion_Weight = .7;
float Seperation_weight = 1.3;
const int NumBoids = 200;
const int Boid_radius = 10;


int main()
{
    App app;
    app.Init(1000,800);

    std::vector<BoidTriangle*> boids;
    for(int i = 0;i<NumBoids;i++)
    {   
        glm::vec2 Position = glm::vec2(Random::Range(0.0f,app.Get_Width()),
                                        Random::Range(0.0f,app.Get_Height()));
        boids.push_back(new BoidTriangle(Position));
    }


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;               // looks better on detached windows
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    ImGui_ImplGlfw_InitForOpenGL(app.GetWindow(), true);
    ImGui_ImplOpenGL3_Init(nullptr);

    ImGui::StyleColorsLight();

    {


    while (!app.ShouldClose())
    {
        app.ProcessInputs();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                             ImGuiDockNodeFlags_PassthruCentralNode);


        ImGui::SetNextWindowBgAlpha(0.45f);
        ImGui::Begin("Hello, World", nullptr,ImGuiWindowFlags_NoTitleBar);
        ImGui::SliderFloat("Align \nWeight",&Align_Weight,0.0f,2.0f);
        ImGui::SliderFloat("Cohesion \nWeight",&Cohesion_Weight,0.0f,2.0f);
        ImGui::SliderFloat("Separation \nWeight",&Seperation_weight,0.0f,2.0f);
        ImGui::End();



        app.Update();

        app.DrawBG();
        glm::vec2 steer(0.0f);
        std::vector<glm::vec2> steering;
        // steering calculation

        for(BoidTriangle* boid:boids)
        {   
            steer = glm::vec2(0.0f);
            steer += boid->Align(boids)*Align_Weight;
            steer += boid->Cohesion(boids)*Cohesion_Weight;
            steer += boid->Seperation(boids)*Seperation_weight;
            steer = Limit(steer,boid->maxForce);
            steering.push_back(steer);
        }

        // apply steer
        for(int i = 0;i<boids.size();i++)
        {
            boids[i]->velocity += steering[i];
            boids[i]->velocity = Limit(boids[i]->velocity,boids[i]->maxSpeed);
            boids[i]->Position += boids[i]->velocity;
            boids[i]->Update();
        }

        //draw boids
        for(BoidTriangle* boid:boids)
        {
            boid->Draw(app.GetProjection());
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);   // the above switches contexts
        }


        app.SwapBuffers();
        app.PollEvents();
    }
    // triangle.Clean();
    }

    ;ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    app.Destroy();
    
}