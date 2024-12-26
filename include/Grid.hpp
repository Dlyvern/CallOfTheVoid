#ifndef GRID_HPP
#define GRID_HPP

#include "GameObject.hpp"
#include <vector>
#include <memory>

struct GridCell
{
public:
    std::vector<std::shared_ptr<GameObject>> objects;
};

class Grid
{
public:
    Grid();
    void addObject(const std::shared_ptr<GameObject> gameObject);
    void checkCollisions(const std::vector<std::shared_ptr<GameObject>>& gameObjects);
    std::vector<std::shared_ptr<GameObject>>& getCellObjectsByIndex(int cellIndex);

private:
    int m_width{10};
    int m_height{5};
    int m_depth{10};
    float m_cellSize{10};
    std::vector<GridCell> m_cells;

    int getCellIndexByPosition(const glm::vec3& position);
};

#endif //GRID_HPP