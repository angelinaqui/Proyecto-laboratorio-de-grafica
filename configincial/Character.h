#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <glm/glm.hpp>

/*
* Estructura de datos que guarda informacion de los personajes (piezas)
* name --> Nombre de la pieza (Prints en consola)
* position --> Puntero a la posicion de la pieza en el mundo
* team  --> Indica de que equipo es la pieza. Valores:
*           1 --> Equipo Minegraft
*           2 --> Equipo Power Rangers
* type --> Tipo de pieza que represaenta el personaje:
*           1 --> Torre
*           2 --> Caballo
*           3 --> Alfil
*           4 --> Reyna
*           5 --> Rey
*           6 --> Peon
*/
struct Character {
    std::string name;
    glm::vec3* position;
    unsigned int team;
    unsigned int type;
};

#endif // CHARACTER_H
