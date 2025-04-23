#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class GameObject;

class Component
{
public:
    virtual void update(float deltaTime) = 0;

    void setOwner(GameObject* owner) { m_owner = owner; }
    GameObject* getOwner() const { return m_owner; }
    virtual ~Component() = default;

private:
    GameObject* m_owner{nullptr};
};

#endif //COMPONENT_HPP
