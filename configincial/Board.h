#ifndef BOARD_H
#define BOARD_H

#include <glm/glm.hpp>
#include "Character.h"
#include <iostream>
#include <utility>

/*
* Estructura que define una casilla
* occupied -> Si la casilla está ocupada
* position -> Posición de la casilla en el mundo
* character -> Puntero a personaje que alberga si es que lo tiene
*/
struct cell {
    bool occupied;
    glm::vec3 position;
    Character* character;
};

class Board {
public:
    cell cells[8][8];  // Casillas del tablero

    Board() {  // Constructor de la clase
        float startX = -2.1f;  // Coordenada X inicial
        float startZ = -2.1f;  // Coordenada Z inicial
        float spacing = 0.6f;  // Espacio entre casillas

        // Inicializar todas las casillas
        for (int i = 0; i < 8; i++) {          // Filas (eje Z)
            for (int j = 0; j < 8; j++) {       // Columnas (eje X)
                cells[i][j].occupied = false;
                cells[i][j].position = glm::vec3(
                    startX + (j * spacing),  // X: Avanza en columnas
                    0.0f,                    // Y: Todas en el mismo plano
                    startZ + (i * spacing)   // Z: Avanza en filas
                );
                cells[i][j].character = nullptr;
            }
        }

    }
    void printBoard() {
        for (int i = 0; i < 8; i++) {          // Filas (eje Z)
            for (int j = 0; j < 8; j++) {       // Columnas (eje X)
                std::cout << "[" << i << "," << j << "]" << "-->" << "(" << cells[i][j].position.x << "," << cells[i][j].position.y << "," << cells[i][j].position.z << ")" << cells[i][j].character->name << std::endl;
            }
        }
    }

    bool move(float originX, float originZ, float destinationX, float destinationZ) {
        auto cellOrigin = this->getCellFromPosition(originX, originZ);                  // Calcula la casilla de origen
        auto cellDestination = this->getCellFromPosition(destinationX, destinationZ);   // Calcula la casilla de destino
        if (cellOrigin.first == -1 || cellOrigin.second == -1 
            || cellDestination.first == -1 || cellDestination.second == -1) return false; // Verifica que sean validas

        if (!cells[cellOrigin.first][cellOrigin.second].occupied) return false;         // Verifica que haya una pieza en la casilla

        Character* piece = cells[cellOrigin.first][cellOrigin.second].character;        // Obtiene al personaje de dicha casilla
        
        if (cells[cellDestination.first][cellDestination.second].occupied) {            //Verificas si existe una pieza en esa casilla
            if (cells[cellDestination.first][cellDestination.second].character->team == piece->team) return false;  // Es de mi equipo?
            cells[cellDestination.first][cellDestination.second].character->position->x = 999.0f;
            cells[cellDestination.first][cellDestination.second].character->position->z = 999.0f; //Desaparece de forma rudimentaria la pieza xd
        }

        std::cout << "En [" << cellOrigin.first << "][" << cellOrigin.second << "] esta " << piece->name << std::endl;
        piece->position->x = destinationX;
        piece->position->z = destinationZ;
        std::cout << "Se mueve " << piece->name << std::endl;
        std::cout << "De (" << originX << "," << "0.0" << "," << originZ << ")" << " --> [" << cellOrigin.first << "," << cellOrigin.second << "]" << std::endl;
        std::cout << "A (" << destinationX << "," << "0.0" << "," << destinationZ << ")" << "--> [" << cellDestination.first << "," << cellDestination.second << "]" << std::endl;
        cells[cellDestination.first][cellDestination.second].character = piece; //Mueve la pieza
        cells[cellDestination.first][cellDestination.second].occupied = true;   
        cells[cellOrigin.first][cellOrigin.second].character = nullptr;         // Limpia el origen
        cells[cellOrigin.first][cellOrigin.second].occupied = false;
        return true;
    }

    // Función que devuelve {fila, columna} o {-1, -1} si está fuera del tablero
    std::pair<int, int> getCellFromPosition(float x, float z) {
        const float startX = -2.1f; // Coordenada X de [0][0]
        const float startZ = -2.1f; // Coordenada Z de [0][0]
        const float spacing = 0.6f; // Espaciado entre celdas
        // Asume que (x, z) ya están "snapped" a una celda válida
        const int j = static_cast<int>(round((x - startX) / spacing));
        const int i = static_cast<int>(round((z - startZ) / spacing));
        if (i < 0 || i >= 8 || j < 0 || j >= 8) {
            return { -1, -1 };  // Opcional: fallo silencioso o assert
        }
        return { i, j };  // Ya validado por ScreenToWorld
    }

    bool haveCharacter(int row, int col) {
        if (row == -1 || col == -1) return false; // Verifica que sean validas
        return cells[row][col].occupied;
    }

    void initMinecraftBoard(std::vector<Character>& minecraftCharacters) {
        // Verificar que hay suficientes personajes
        if (minecraftCharacters.size() < 16) {
            std::cerr << "Error: No hay suficientes personajes para asignar al tablero.\n";
            return;
        }
        // Asignar personajes principales a la primera fila (cells[0][0] a cells[0][7])
        for (int j = 0; j < 8; j++) {
            cells[0][j].character = &minecraftCharacters[j];
            cells[0][j].occupied = true;
        }
        // Asignar zombies a la segunda fila (cells[1][0] a cells[1][7])
        for (int j = 0; j < 8; j++) {
            cells[1][j].character = &minecraftCharacters[8 + j];  // Índices 8 a 15
            cells[1][j].occupied = true;
        }
    }
};

#endif

/* Posiciones y casillas
//Esquina inferior derecha minecraft
[0,0]-->(-2.1,0,-2.1) --Creeper 1
[0,1]-->(-1.5,0,-2.1) 
[0,2]-->(-0.9,0,-2.1)
[0,3]-->(-0.3,0,-2.1)
[0,4]-->(0.3,0,-2.1)
[0,5]-->(0.9,0,-2.1)
[0,6]-->(1.5,0,-2.1)
[0,7]-->(2.1,0,-2.1) --Creeper 2
[1,0]-->(-2.1,0,-1.5) --Zombie 0
[1,1]-->(-1.5,0,-1.5)
[1,2]-->(-0.9,0,-1.5)
[1,3]-->(-0.3,0,-1.5)
[1,4]-->(0.3,0,-1.5)
[1,5]-->(0.9,0,-1.5)
[1,6]-->(1.5,0,-1.5)
[1,7]-->(2.1,0,-1.5) --Zombie 7
[2,0]-->(-2.1,0,-0.9)
[2,1]-->(-1.5,0,-0.9)
[2,2]-->(-0.9,0,-0.9)
[2,3]-->(-0.3,0,-0.9)
[2,4]-->(0.3,0,-0.9)
[2,5]-->(0.9,0,-0.9)
[2,6]-->(1.5,0,-0.9)
[2,7]-->(2.1,0,-0.9)
[3,0]-->(-2.1,0,-0.3)
[3,1]-->(-1.5,0,-0.3)
[3,2]-->(-0.9,0,-0.3)
[3,3]-->(-0.3,0,-0.3)
[3,4]-->(0.3,0,-0.3)
[3,5]-->(0.9,0,-0.3)
[3,6]-->(1.5,0,-0.3)
[3,7]-->(2.1,0,-0.3)
[4,0]-->(-2.1,0,0.3)
[4,1]-->(-1.5,0,0.3)
[4,2]-->(-0.9,0,0.3)
[4,3]-->(-0.3,0,0.3)
[4,4]-->(0.3,0,0.3)
[4,5]-->(0.9,0,0.3)
[4,6]-->(1.5,0,0.3)
[4,7]-->(2.1,0,0.3)
[5,0]-->(-2.1,0,0.9)
[5,1]-->(-1.5,0,0.9)
[5,2]-->(-0.9,0,0.9)
[5,3]-->(-0.3,0,0.9)
[5,4]-->(0.3,0,0.9)
[5,5]-->(0.9,0,0.9)
[5,6]-->(1.5,0,0.9)
[5,7]-->(2.1,0,0.9)
[6,0]-->(-2.1,0,1.5)
[6,1]-->(-1.5,0,1.5)
[6,2]-->(-0.9,0,1.5)
[6,3]-->(-0.3,0,1.5)
[6,4]-->(0.3,0,1.5)
[6,5]-->(0.9,0,1.5)
[6,6]-->(1.5,0,1.5)
[6,7]-->(2.1,0,1.5)
[7,0]-->(-2.1,0,2.1)
[7,1]-->(-1.5,0,2.1)
[7,2]-->(-0.9,0,2.1)
[7,3]-->(-0.3,0,2.1)
[7,4]-->(0.3,0,2.1)
[7,5]-->(0.9,0,2.1)
[7,6]-->(1.5,0,2.1)
[7,7]-->(2.1,0,2.1)
*/