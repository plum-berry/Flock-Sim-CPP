#include<Engine.h>


float Wrap(float Pos,float size,float margin);
glm::vec2 WrappedDelta(glm::vec2 a,glm::vec2 b,glm::vec2 world);
glm::vec2 Limit(glm::vec2 vector,float maxLength);

class Boid:public Circle{

    private:
        float speed = 1;
        float angle;
        float Perception = 100;
        float seperationPerception = 30;
    glm::vec2 SteerToward(glm::vec2 desiredDirection)
    {
        if(glm::length(desiredDirection) == 0)
            return glm::vec2(0.0f);
        desiredDirection = glm::normalize(desiredDirection)*maxSpeed;
        glm::vec2 steering = desiredDirection - velocity;
        return Limit(steering,maxForce);
    }
    public:
    const float maxSpeed = 2.0;
    glm::vec2 velocity;
    const float maxForce = 0.1;
    Boid(int radius,glm::vec2 Position):Circle(radius,Position)
    {
        angle = Random::Range(0.f,glm::two_pi<float>());
        velocity = glm::vec2(glm::cos(angle),glm::sin(angle));
    }
    
    void Update()
    {
        // temporary hardcoding
        Position.x = Wrap(Position.x,1000,radius);
        Position.y = Wrap(Position.y,800,radius);
    }

    glm::vec2 Align(const std::vector<Boid*>& Flock)
    {
        glm::vec2 average(0.0f);
        int Neighbours = 0;
        for(Boid* boid: Flock)
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

    glm::vec2 Cohesion(const std::vector<Boid*>& Flock)
    {
        glm::vec2 averagePos(0.0f);
        int Neighbours = 0;
        for(Boid* boid: Flock)
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

    glm::vec2 Seperation(const std::vector<Boid*>& Flock)
    {
        glm::vec2 desired(0.0f);
        int Neighbours = 0;
        for(Boid* boid:Flock)
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
};


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

// Steering vector = desired - current velocity
// Alignment function

glm::vec2 Limit(glm::vec2 vector,float maxLength)
{
    if(glm::length(vector) > maxLength)
    {
        return glm::normalize(vector)*maxLength;
    }
    return vector;
}



