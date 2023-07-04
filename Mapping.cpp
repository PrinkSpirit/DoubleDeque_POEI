#include "Mapping.h"


std::ostream& operator<<(std::ostream& os, const Square& n)
{
	os << "(" << n.x << ", " << n.y << ")";
	return os;
}

// Hex value. second to last digit = bg color, last one is foreground color
// 0000 -> xrgb so 4 is red, 2 is green, 1 is blue. Added it mean 6 is yellow, 7 is white
void drawMap(DoubleDeque<Square>* map)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < map->getHeight(); i++)
	{
		for (int j = 0; j < map->getWidth(); j++)
		{
			if (map->get(i, j)->spdPenalty != -1 && map->get(i, j)->visited) // is part of the final path
			{
				SetConsoleTextAttribute(hConsole, 0x0067); // Yellow bg, white fg
				std::cout << map->get(i, j)->spdPenalty;   
			}
			else if (map->get(i, j)->spdPenalty == 1) // Has minimal speed penalty 
			{
				SetConsoleTextAttribute(hConsole, 0x0027); // green bg, white fg
				std::cout << " ";
			}
			else if (map->get(i, j)->spdPenalty == -1) // Is impassible
			{
				SetConsoleTextAttribute(hConsole, 0x0004); // Black bg, Red fg
				std::cout << "#";
			}
			else									  // Has speed penalty
			{
				SetConsoleTextAttribute(hConsole, 0x0017); // Blue bg, white fg
				std::cout << map->get(i, j)->spdPenalty;
			}
		}
		SetConsoleTextAttribute(hConsole, 7);
		std::cout << std::endl;
	}
	SetConsoleTextAttribute(hConsole, 7); // Reset console color to default
}

void loadFromFile(DoubleDeque<Square>* map, std::string path)
{
	std::ifstream file(path);
	std::string line;
	int i = 0;
	int v = 0;

	while (std::getline(file, line)) 
	{
		for (int j = 0; j < line.size(); j++) 
		{
			if (line[j] == 'X' || line[j] == '#') // Impassible terrain
			{ 
				v = -1;
			}
			else
			{
				v = (line[j] - 48); // Convert from char to int
			}
			map->addAt({ i,j,v, false }, i, j);
		}
		i++;
	}
}

std::vector<Square*>* reconstructPath(std::map<Square*, Square*> cameFrom, Square* current)
{
	std::vector<Square*>* path = new std::vector<Square*>();
	current->visited = true; // Mark the square so we can display them later
	path->push_back(current);
	while (cameFrom.count(current) != 0)
	{
		current = cameFrom[current];
		current->visited = true;
		path->push_back(current);
	}
	std::reverse(path->begin(), path->end()); // Since path is build from destination to origin, we reverse the order
	return path;
}

/* A* implementation from Wikipedia pseudocode 
*	https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
*/
std::vector<Square*>* A_Star(Square* origin, Square* destination, DoubleDeque<Square>* map, std::function<float(Square*, Square*)> h, bool hWrap)
{

	std::vector<Square*>* path = new std::vector<Square*>();

	if (map->get(origin->x, origin->y)->spdPenalty == -1)
	{
		std::cerr << "[ERROR] AStar: Origin point is impassible." << std::endl;
		return path;
	}
	if (map->get(destination->x, destination->y)->spdPenalty == -1)
	{
		std::cerr << "[ERROR] AStar: Destination is impassible." << std::endl;
		return path;
	}

	if (map->get(origin->x, origin->y) == map->get(destination->x, destination->y))
	{
		std::cerr << "[ERROR] AStar: Origin and destination are the same." << std::endl;
		return path;
	}

	std::set<Square*> openSet;
	std::map<Square*, Square*> cameFrom; 

	Square* current = nullptr;
	bool finished = false;
	float tentativeGScore;
	float minFScore;

	// Initialize the squares so we can reuse the function.
	for (int x = 0; x < map->getWidth(); x++)
	{
		for (int y = 0; y < map->getHeight(); y++)
		{
			current = map->get(x, y);
			current->fScore = std::numeric_limits<float>::infinity();
			current->gScore = std::numeric_limits<float>::infinity();
			current->visited = false;
		}
	}

	openSet.insert(origin);
	origin->gScore = 0;
	origin->fScore = h(origin, destination);
	std::vector<Square*> neighbors;

	while (!openSet.empty())
	{
		minFScore = std::numeric_limits<float>::infinity();
		// Since the while condition break on openSet being empty current is assured to have a value
		for (auto it = openSet.begin(); it != openSet.end(); it++)
		{ 
			if ((*it)->fScore < minFScore) {
				current = *it;
				minFScore = (*it)->fScore;
			}
		}

		if (current == destination)
		{
			finished = true;
			break;
		}
		openSet.erase(current);

		// for each neighbor of current square
		if (map->isInbound(current->x, current->y + 1)) //up
		{
			neighbors.push_back(map->get(current->x, current->y + 1));
		}
		
		if (map->isInbound(current->x, current->y - 1))  //down
		{
			neighbors.push_back(map->get(current->x, current->y - 1));
		}
		
		if (map->isInbound(current->x - 1, current->y)) //left
		{
			neighbors.push_back(map->get(current->x - 1, current->y));
		}
		else if(hWrap)
		{
			neighbors.push_back(map->get(map->getWidth() - 1, current->y));
		}
		
		if (map->isInbound(current->x + 1, current->y)) //right
		{
			neighbors.push_back(map->get(current->x + 1, current->y));
		}
		else if (hWrap)
		{
			neighbors.push_back(map->get(0, current->y));
		}

		for (Square* neighbor : neighbors)
		{
			if (neighbor->spdPenalty == -1) // if impassible we skip
			{
				continue;
			}

			tentativeGScore = current->gScore + neighbor->spdPenalty;

			if (tentativeGScore < neighbor->gScore)
			{
				cameFrom[neighbor] = current;
				neighbor->gScore = tentativeGScore;
				neighbor->fScore = tentativeGScore + h(neighbor, destination);
				if (openSet.count(neighbor) == 0) { // if neighbor is not in openSet
					openSet.insert(neighbor);
				}
			}
		}
		neighbors.clear();
	}
	if (!finished)
	{
		std::cerr << "[ERROR] AStar: No path found." << std::endl;
		return path;
	}

	path = reconstructPath(cameFrom, current);
	return path;
}


void generateMap(DoubleDeque<Square>* map)
{
	std::vector<Wave> waves;
	waves.push_back({ 1,0.03f,1.0f });
	waves.push_back({ 124,0.1f,0.5f });

	DoubleDeque<float>* noiseMap = generate(map->getWidth(), map->getHeight(), 1.0f, &waves, .5f, .5f);

	for (int i = 0; i < map->getHeight(); i++) {
		for (int j = 0; j < map->getWidth(); j++)
		{
			float d = *noiseMap->get(i,j) * 10 + 5; // * 10 else values are too small, +5 to get more positive values
			if (d < 0)
				d = -1;
			if (d >= 10)
				d = 9;

			map->get(i, j)->spdPenalty = d;
		}
	}
}

