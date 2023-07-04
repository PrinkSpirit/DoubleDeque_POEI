#include <iostream>
#include <deque>
#include <vector>
#include <cmath>
#include <string>
#include <queue>
#include <map>
#include <functional>
#include <set>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "Mapping.h"


//TP: Recherche optimisée de chemin dans un environnement indéfini
//
//1 - Comprendre l'utilisation du deque et ses spécificités - DONE
//2 - Parvenir à creer un tableau bidimensionnel de deque - DONE
//3 - Etablir un tableau de deque(TDD) de points(x; y) - DONE
//4 - Rendre possible l'ajour de points dans les quatre directions possibles - DONE
//5 - Prendre en compte les cas spécifiques(haut - gauche, bas - droit par exemple) - DONE
//6 - Permettre un affichage du tableau en console - DONE
//7 - Algorithme de recherche de chemin simple dans une carte définie en TDD de
//		points(x; y; passable) passables et impassables - DONE
//8 - Algorithme de recherche de chemin complexe avec pondération de vitesse de progression - DONE
//9 - Facultatif : Affichage via SFML - DONE
//10 - Generation de carte semi - realiste avec procéduralité - DONE
//11 - Vérification du fonctionnement dans une carte de grande taille(> 500 * 500) - DONE

//12 - Optimisation finale du code - In progress


// Change the color of the sfml tiles to reflect the terrain difficulty
void coloration(DoubleDeque<Square>* map, std::vector<sf::RectangleShape*>* tiles, bool drawPath) {
	for (int i = 0; i < map->getHeight(); i++) {
		for (int j = 0; j < map->getWidth(); j++) {
			if (map->get(i, j)->spdPenalty != -1 && map->get(i, j)->visited && drawPath) {
				tiles->at(j + map->getWidth() * i)->setFillColor(sf::Color::Yellow);
			}
			else if (map->get(i, j)->spdPenalty == -1) {

				tiles->at(j + map->getWidth() * i)->setFillColor(sf::Color(92, 36, 10));
			}
			else {
				tiles->at(j + map->getWidth() * i)->setFillColor(sf::Color(13, 255-map->get(i,j)->spdPenalty*25, 25));
			}
		}
	}

}


/*
* A* times:
* Before A* optimization
* - with sqrt => 153.572s
* - with approx => 157.795s - not using this one
* - with manathan distance => 128s
* Switched from map to keeping scores in Squares
* - manathan => 35s
* With wrapAround
* - 82s
*/

// Return the distance between the current point and the destination
float distance(Square* a, Square* b) {
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

// Implementation of https://www.flipcode.com/archives/Fast_Approximate_Distance_Functions.shtml
float approxDistance(Square* a, Square* b) {
	int min, max, approx, dx, dy;
	dx = abs(b->x - a->x);
	dy = abs(b->y - a->y);

	if (dx < dy)
	{
		min = dx;
		max = dy;
	}
	else
	{
		min = dy;
		max = dx;
	}

	approx = (max * 1007) + (min * 441);
	if (max < (min << 4))
	{
		approx -= (max * 40);
	}

	return (float)((approx + 512) >> 10);
}

float manhattanDistance(Square* a, Square* b) {
	return abs(b->x + a->x) + abs(b->y + a->y);
}

float distanceWrap(Square* a, Square* b) {
	int dx = abs(b->x + a->x);
	int dy = abs(b->y + a->y);

	if(dx > 0.5f)
	{
		dx = 1.0f - dx;
	}

	return sqrt(dx * dx + dy * dy);
}

int main()
{
	DoubleDeque<Square> DD(500, 500);
	std::vector<sf::RectangleShape*> tiles;
	int tileSize = 2;
	sf::RectangleShape* rect = nullptr;

	for (int i = 0; i < DD.getHeight(); i++)
	{
		for (int j = 0; j < DD.getWidth(); j++)
		{
			DD.set({i,j,-1}, i, j);
			rect = new sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
			rect->setPosition(i* tileSize, j * tileSize);

			tiles.push_back(rect);
		}
	}

	generateMap(&DD);
	
	sf::Clock clock;
	std::vector<Square*>* path = A_Star(DD.get(0, 0), DD.get(499, 499), &DD, manhattanDistance, false);
	std::cout << "Elapsed time: " << clock.getElapsedTime().asSeconds() << "s" << std::endl;

	//drawMap(&DD);
	coloration(&DD, &tiles, true);

	sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML works!");
	sf::CircleShape shape(400.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		for (auto tile : tiles) { window.draw(*tile); }
		window.display();
	}

	for (auto tile : tiles) {
		delete tile;
	}
	tiles.clear();

	return 0;
}
