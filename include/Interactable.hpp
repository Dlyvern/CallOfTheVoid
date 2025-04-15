#ifndef INTERACTABLE_HPP
#define INTERACTABLE_HPP

class Interactable
{
public:
    virtual void interact() = 0;
    virtual ~Interactable() = default;
};

#endif //INTERACTABLE_HPP
