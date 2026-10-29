#include<glad/gl.h>
#include<Shader.h>
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<Random.h>

float Wrap(float Pos,float size,float margin);
glm::vec2 WrappedDelta(glm::vec2 a,glm::vec2 b,glm::vec2 world);
glm::vec2 Limit(glm::vec2 vector,float maxLength);

class BoidTriangle{
    protected:
        static unsigned int VAO;
        static unsigned int VBO;
        static std::vector<float> vertices;
        static Shader* shader;
        static int count;
        glm::vec3 color = glm::vec3(0.0f,1.0f,0.0f);
        float size = 15.0f;
        float Perception = 100;
        float seperationPerception = 30;

    public:
        const float maxSpeed = 3.0f;
        glm::vec2 Position;
        float maxForce = 0.1;
        glm::vec2 velocity;
        float angle;

        BoidTriangle(glm::vec2 Position)
        {
           this->Position = Position;
           angle = Random::Range(0.0f,glm::pi<float>());
           velocity = glm::vec2(glm::cos(angle),glm::sin(angle))*maxSpeed;
           if(count == 0)
           {
                Setup();
           }
           count++;
        }
          glm::vec2 SteerToward(glm::vec2 desiredDirection)
    {
        if(glm::length(desiredDirection) == 0)
            return glm::vec2(0.0f);
        desiredDirection = glm::normalize(desiredDirection)*maxSpeed;
        glm::vec2 steering = desiredDirection - velocity;
        return Limit(steering,maxForce);
    }

        void Setup()
        {
            // Generate VBO and VAO
            glGenBuffers(1,&VBO);
            glGenVertexArrays(1,&VAO);

            glBindVertexArray(VAO);

            //Buffer Definition on the GPU
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);

            // Vertex Attributes defintions
            glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
            glEnableVertexAttribArray(0);

            shader  =  new Shader("Shaders/Shader2D.vs","Shaders/Shader2D.fs");
        }

        void Draw(glm::mat4 Projection)
        {
           glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(Position,0.0f)) ;
 

            if (glm::length(velocity) > 0.0001f)
            {
                float target = glm::atan(velocity.y, velocity.x);

                float diff = target - angle;
                // wrap to [-pi, pi] so it turns the short way
                diff = std::fmod(diff + glm::pi<float>(), glm::two_pi<float>());
                if (diff < 0.0f) diff += glm::two_pi<float>();
                diff -= glm::pi<float>();

                angle += diff * 0.2f;   // 0.1 = lazier, 1.0 = instant
            }


           model = glm::rotate(model,angle,glm::vec3(0.0f,0.0f,1.0f));
           model = glm::scale(model,glm::vec3(size,size,1.0f));
           shader->use();
           shader->setMat4("model",model);
           shader->setMat4("projection",Projection);
           shader->setVec3("Color",color);

           glBindVertexArray(VAO);
           glDrawArrays(GL_TRIANGLES,0,3);
        }

        ~BoidTriangle()
        {
           count--;
           if(count == 0) 
           {
                Clean();
           }
        }
        void Clean()
        {
            glDeleteVertexArrays(1,&VAO);
            glDeleteBuffers(1,&VBO);
        }
      
        void Update()
    {
        // temporary hardcoding
        Position.x = Wrap(Position.x,1000,size);
        Position.y = Wrap(Position.y,800,size);
    }

    glm::vec2 Align(const std::vector<BoidTriangle*> Flock)
    {
        glm::vec2 average(0.0f);
        int Neighbours = 0;
        for(BoidTriangle* boid: Flock)
        {
            if(boid == this)continue;
            glm::vec2 d = WrappedDelta(Position,boid->Position,glm::vec2(1000.0f,800.0f));
            float distance = glm::length(d);
            if(distance < Perception)
            {
                average += boid->velocity;
                Neighbours ++;
            }
        }
        if(Neighbours == 0)return glm::vec2(0.0f);
        average = average/(float)Neighbours;
        return SteerToward(average);
    }

    glm::vec2 Cohesion(const std::vector<BoidTriangle*>& Flock)
    {
        glm::vec2 averagePos(0.0f);
        int Neighbours = 0;
        for(BoidTriangle* boid: Flock)
        {
            if(boid == this)continue;
            glm::vec2 d = WrappedDelta(boid->Position,Position,glm::vec2(1000.0f,800.0f));
            float distance = glm::length(d);
            if(distance < Perception)
            {
                averagePos += d;
                Neighbours++;
            }
        }
        if(Neighbours == 0) return glm::vec2(0.0f);
        glm::vec2 desired = averagePos/(float)Neighbours;
        return SteerToward(desired);
        }

    glm::vec2 Seperation(const std::vector<BoidTriangle*>& Flock)
    {
        glm::vec2 desired(0.0f);
        int Neighbours = 0;
        for(BoidTriangle* boid:Flock)
        {
            if(boid==this)continue;
            glm::vec2 d = WrappedDelta(Position,boid->Position,glm::vec2(1000.0f,800.0f));
            float distance = glm::length(d);
            if(distance==0)continue;
            if(distance < seperationPerception)
            {
                desired += (d/distance);
                Neighbours++;
            }
        }
        if(Neighbours == 0) return glm::vec2(0.0f);
        return SteerToward(desired/(float)Neighbours);
        // return SteerToward(desired);
    }
    glm::vec2 Limit(glm::vec2 vector,float maxLength)
{
    if(glm::length(vector) > maxLength)
    {
        return glm::normalize(vector)*maxLength;
    }
    return vector;
}

float Wrap(float Pos,float size,float margin)
{


    float period = size + 2.0f*margin;
    Pos = std::fmod(Pos + margin,period);
    if(Pos < 0.0f) Pos += period;
    return Pos-margin;
}

glm::vec2 WrappedDelta(glm::vec2 a,glm::vec2 b,glm::vec2 world)
{
    glm::vec2 d = a - b;
    if(d.x > world.x*0.5f) d.x -= world.x;
    if(d.x < -world.x * 0.5f) d.x += world.x;
    if(d.y > world.y*0.5f) d.y -= world.y;
    if(d.y < -world.y*0.5f) d.y += world.y;
    return d;
}
};


// Define all Static Varibles
std::vector<float> BoidTriangle::vertices = {1.0f,0.0f,
                                            -1.0f,0.5f,
                                            -1.0f,-0.5f};
int BoidTriangle::count = 0;
unsigned int BoidTriangle::VAO = 0;
unsigned int BoidTriangle::VBO = 0;
Shader* BoidTriangle::shader = nullptr;




