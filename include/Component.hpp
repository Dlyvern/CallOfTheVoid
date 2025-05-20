#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class GameObject;

class Component
{
public:
    virtual void update(float deltaTime) = 0;

    virtual void setOwner(GameObject* owner) { m_owner = owner; }
    [[nodiscard]] GameObject* getOwner() const { return m_owner; }

    virtual void destroy() {}
    virtual ~Component() = default;
private:
    GameObject* m_owner{nullptr};
};

#endif //COMPONENT_HPP
