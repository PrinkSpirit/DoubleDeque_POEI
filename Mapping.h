/*
* Contains structures and functions pertaining to map génération or pathfinding
*/

#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <queue>
#include <map>
#include <functional>
#include <set>
#include <windows.h>
#include <fstream>

#include "Wave.h"
#include "DoubleDeque.h"


/*
* Simple structure to represents the tile of a 2d map.
*/
struct Square {
	int x = 0;
	int y = 0;
	int spdPenalty = 1; // -1 mean terrain is inpassible
	bool visited = false; // If this square is part of the found path
	float gScore;
	float fScore;

	bool operator<(const Square& other) {
		return spdPenalty < other.spdPenalty;
	}

	bool operator>(const Square& other) {
		return spdPenalty > other.spdPenalty;
	}
};
// Print square coordinates in console
std::ostream& operator<<(std::ostream& os, const Square& n);

// Draw the map in console
void drawMap(DoubleDeque<Square>* map);

/* Load a map from a file
* format:
*   - X / # => impassible, will be saved as 0 in the map
*   - 0-9   => Terrain
*   - New line in file => new row in map
*/
void loadFromFile(DoubleDeque<Square>* map, std::string path);
// Generate a map using perlin noise
void generateMap(DoubleDeque<Square>* map);


// Look for the shortest path to the destination square using the A* algorithm
std::vector<Square*>* A_Star(Square* origin, Square* destination, DoubleDeque<Square>* map, std::function<float(Square*, Square*)> h, bool hWrap);
// Part of the A* algorithm, allow to build the path from the result of A*
std::vector<Square*>* reconstructPath(std::map<Square*, Square*> cameFrom, Square* current);
