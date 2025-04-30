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

/*
* Clase querepresenta el tablero de juego
* cells --> Celdas de juego
* turn --> Turno del jugador: 1 --> Piezas Minecraft, 2 --> Piezas powe rangers
* Board() --> Constructor de la clase que inicializa las celdas
* move() --> Mueve una pieza de un punto X,Z a un punto Z,X
* getCellFromPosition() --> Calcula la celda perteneciente al punto X,Z proporcionado
* haveCharacter() --> Indica si la casilla tiene personaje
* initMinecraftBoard() --> Inicializa a los personajes de minecraft en sus casillas
*/
class Board {
public:
    cell cells[8][8];  // Casillas del tablero
    int turn = 1;

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
    //TODO: Esta funcion debe ser solo para mover, la logica de verificacion se debe mover a otra funcion
    bool move(float originX, float originZ, float destinationX, float destinationZ) {
        auto cellOrigin = this->getCellFromPosition(originX, originZ);                  // Calcula la casilla de origen
        auto cellDestination = this->getCellFromPosition(destinationX, destinationZ);   // Calcula la casilla de destino
        Character* piece = cells[cellOrigin.first][cellOrigin.second].character;        // Obtiene al personaje de dicha casilla

         //Si el movimiento no es valido
        if (!this->check_move(piece, cellOrigin.first, cellOrigin.second, cellDestination.first, cellDestination.second)) return false;
        
        if (cells[cellDestination.first][cellDestination.second].occupied) {            //Verificas si existe una pieza en esa casilla
            cells[cellDestination.first][cellDestination.second].character->position->x = 999.0f;
            cells[cellDestination.first][cellDestination.second].character->position->z = 999.0f; //Desaparece de forma rudimentaria la pieza xd
        }
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
        if (minecraftCharacters.size() != 16) {
            std::cerr << "Error: Numero de personajes no admitido por el juego.\n";
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

    // La metodologia para revisar si el movimiento es valido, es primero descartar todos los movimientos invalidos evidentes
    bool check_move(Character* piece, int cellOriginI, int cellOriginJ, int cellDestinationI, int cellDestinationJ) {
        if (cellOriginI == -1 || cellOriginJ == -1
            || cellDestinationI == -1 || cellDestinationJ == -1) return false;

        int direction = (piece->team == 1) ? 1 : -1;
        int deltaI = cellDestinationI - cellOriginI;
        int deltaJ = cellDestinationJ - cellOriginJ;
        int startRow = (piece->team == 1) ? 1 : 6;

        // Aliado en destino
        if (cells[cellDestinationI][cellDestinationJ].occupied &&
            cells[cellDestinationI][cellDestinationJ].character->team == piece->team) {
            return false;
        }

        switch (piece->type) {
        case 1: { // Torre
            if (deltaJ == 0 && deltaI != 0) {
                int step = (deltaI > 0) ? 1 : -1;
                for (int i = cellOriginI + step; i != cellDestinationI; i += step)
                    if (cells[i][cellOriginJ].occupied) return false;
                return true;
            }
            if (deltaI == 0 && deltaJ != 0) {
                int step = (deltaJ > 0) ? 1 : -1;
                for (int j = cellOriginJ + step; j != cellDestinationJ; j += step)
                    if (cells[cellOriginI][j].occupied) return false;
                return true;
            }
            return false;
        }
        case 2: { // Caballo
            if ((abs(deltaI) == 2 && abs(deltaJ) == 1) || (abs(deltaI) == 1 && abs(deltaJ) == 2))
                return true;
            return false;
        }
        case 3: { // Alfil
            if (abs(deltaI) != abs(deltaJ)) return false;
            int stepI = (deltaI > 0) ? 1 : -1;
            int stepJ = (deltaJ > 0) ? 1 : -1;
            for (int i = cellOriginI + stepI, j = cellOriginJ + stepJ; i != cellDestinationI; i += stepI, j += stepJ) {
                if (cells[i][j].occupied) return false;
            }
            return true;
        }
        case 4: { // Reina = torre + alfil
            if (deltaI == 0 || deltaJ == 0) { // movimiento tipo torre
                if (deltaJ == 0) {
                    int step = (deltaI > 0) ? 1 : -1;
                    for (int i = cellOriginI + step; i != cellDestinationI; i += step)
                        if (cells[i][cellOriginJ].occupied) return false;
                    return true;
                }
                if (deltaI == 0) {
                    int step = (deltaJ > 0) ? 1 : -1;
                    for (int j = cellOriginJ + step; j != cellDestinationJ; j += step)
                        if (cells[cellOriginI][j].occupied) return false;
                    return true;
                }
            }
            if (abs(deltaI) == abs(deltaJ)) { // movimiento tipo alfil
                int stepI = (deltaI > 0) ? 1 : -1;
                int stepJ = (deltaJ > 0) ? 1 : -1;
                for (int i = cellOriginI + stepI, j = cellOriginJ + stepJ; i != cellDestinationI; i += stepI, j += stepJ) {
                    if (cells[i][j].occupied) return false;
                }
                return true;
            }
            return false;
        }
        case 5: { // Rey
            if (abs(deltaI) <= 1 && abs(deltaJ) <= 1) return true;
            return false;
        }
        case 6: { // Peón
            if (direction == 1 && deltaI < 0) return false;
            if (direction == -1 && deltaI > 0) return false;
            if (cellOriginI == startRow && deltaI == 2 * direction) {
                if (!cells[cellOriginI + direction][cellOriginJ].occupied &&
                    !cells[cellDestinationI][cellDestinationJ].occupied)
                    return true;
            }
            if (deltaJ == 0 && deltaI == direction && !cells[cellDestinationI][cellDestinationJ].occupied)
                return true;
            if (abs(deltaJ) == 1 && deltaI == direction &&
                cells[cellDestinationI][cellDestinationJ].occupied &&
                cells[cellDestinationI][cellDestinationJ].character->team != piece->team)
                return true;
            return false;
        }
        default:
            return false;
        }
    }

};

#endif

/* Posiciones y casillas
//Esquina inferior derecha minecraft
[0,0]-->(-2.1,0,-2.1) --Creeper 2
[0,1]-->(-1.5,0,-2.1) 
[0,2]-->(-0.9,0,-2.1)
[0,3]-->(-0.3,0,-2.1)
[0,4]-->(0.3,0,-2.1)
[0,5]-->(0.9,0,-2.1)
[0,6]-->(1.5,0,-2.1)
[0,7]-->(2.1,0,-2.1) --Creeper 1
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