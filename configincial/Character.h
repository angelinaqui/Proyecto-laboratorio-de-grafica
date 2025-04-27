#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <glm/glm.hpp>

struct Character {
    std::string name;
    glm::vec3* position; // Puntero a la posici�n original
    int team;
};

#endif // CHARACTER_H
