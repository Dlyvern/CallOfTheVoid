#ifndef SCENE_HPP
#define SCENE_HPP

class Scene
{
public:
    Scene();

    virtual ~Scene();

    virtual void update(float deltaTime) = 0;

    virtual void create() = 0;

    virtual bool isOver() = 0;
};

#endif //SCENE_HPP