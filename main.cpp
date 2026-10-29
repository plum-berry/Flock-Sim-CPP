#include<Engine.h>
#include "BoidTriangle.h"
#include "Boid.h"

const float Align_Weight = 0.8;
const float Cohesion_Weight = .7;
const float Seperation_weight = 1.3;
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
    {


    while (!app.ShouldClose())
    {
        app.ProcessInputs();
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
        app.SwapBuffers();
        app.PollEvents();
    }
    // triangle.Clean();
    }
    app.Destroy();
    
}