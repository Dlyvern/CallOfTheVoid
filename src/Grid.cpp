#include "Grid.hpp"
#include "Collision.hpp"
#include <iostream>
#include <algorithm>

Grid::Grid()
{
    m_cells.resize(m_width * m_height * m_depth);
}

void Grid::addObject(const std::shared_ptr<GameObject> gameObject)
{
    int cellIndex = getCellIndexByPosition(gameObject->getPosition());
    std::cout << "Game object " << gameObject->getName() << " placed at " << cellIndex << " cell" << std::endl;
    m_cells[cellIndex].objects.push_back(gameObject);
}


void Grid::checkCollisions(const std::vector<std::shared_ptr<GameObject>> &gameObjects)
{
    for (auto& object : gameObjects) 
    {
        int cellIndex = getCellIndexByPosition(object->getPosition());

        // Check for collisions with objects in the same and adjacent cells
        for (int dx = -1; dx <= 1; ++dx) 
        {
            for (int dy = -1; dy <= 1; ++dy) 
            {
                for (int dz = -1; dz <= 1; ++dz)
                {
                    // Calculate neighbor cell index
                    int neighborCellIndex = cellIndex + dx + m_width * (dy + m_height * dz);

                    // Check if neighbor cell index is within grid boundaries
                    if (neighborCellIndex >= 0 && neighborCellIndex < m_cells.size())
                    {
                        // Check for collisions with objects in the neighbor cell
                        for (auto& other : getCellObjectsByIndex(neighborCellIndex)) 
                        {
                            if (&object != &other)
                            {
                                if (physics::collision::doAABBIntersectABBB(object->getBoundingBox(), other->getBoundingBox()))
                                {
                                    if(object->getName() == "player" || other->getName() == "player")
                                    {
                                        glm::vec3 depth = physics::collision::calculatePenetrationDepth(object->getBoundingBox(), other->getBoundingBox());
                                        
                                        std::cout << "Penetration x: " << depth.x << std::endl;
                                        std::cout << "Penetration y: " << depth.y << std::endl;
                                        std::cout << "Penetration z: " << depth.z << std::endl;

                                        // object->setPosition(object->getPosition() - depth * 0.5f);
                                    }
                                    //     std::cout << "Fuck my mother" << std::endl;

                                    // Handle collision response (e.g., move objects, apply physics)
                                    // ... (Your collision response logic)
                                }
                                else
                                {
                                    // if(object->getName() == "player" || other->getName() == "player")
                                    //     std::cout << "Do not fuck my mother" << std::endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

std::vector<std::shared_ptr<GameObject>> &Grid::getCellObjectsByIndex(int cellIndex)
{
    return m_cells[cellIndex].objects;
}

int Grid::getCellIndexByPosition(const glm::vec3 &position)
{
    int x = static_cast<int>(position.x / m_cellSize);
    int y = static_cast<int>(position.y / m_cellSize);
    int z = static_cast<int>(position.z / m_cellSize);

    // Clamp indices to grid boundaries
    x = std::clamp(x, 0, m_width - 1);
    y = std::clamp(y, 0, m_height - 1);
    z = std::clamp(z, 0, m_depth - 1);

    return x + m_width * (y + m_height * z);
}
