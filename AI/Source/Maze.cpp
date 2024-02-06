#include "Maze.h"
#include "MyMath.h"

#pragma warning (disable : 4996)
#include "csv.h"
#include <iostream>

float MazePt::length()
{
	return std::sqrt(x*x + y*y);
}

Maze::Maze()
{
}

Maze::~Maze()
{
}

/*
 * size
 * The width and height of the maze (Square Maze)
 */
void Maze::Generate(unsigned key, unsigned size, MazePt start, float wallLoad)
{
	m_key = key;
	m_size = size;
	m_numMove = 0;
	m_grid.clear();
	if (size == 0)
		return;
	start.x = Math::Clamp(start.x, 0, (int)size - 1);
	start.y = Math::Clamp(start.y, 0, (int)size - 1);
	wallLoad = Math::Clamp(wallLoad, 0.f, 0.8f);
	unsigned total = size * size;
	m_grid.resize(total);
	std::fill(m_grid.begin(), m_grid.end(), TILE_EMPTY);
	io::CSVReader<20> worldFile("Image/world.csv");
	int i = 0;
	while (char* line = worldFile.next_line())
	{
		std::string number = line;
		std::string next;
		std::vector<std::string> result;

		// For each character in the string
		for (const char a : number) {
			// If we've hit the terminal character
			if (a == ',') {
				// If we have some characters accumulated
				if (!next.empty()) {
					// Add them to the result vector
					result.push_back(next);
					next.clear();
				}
			} else {
				// Accumulate the next character into the sequence
				next += a;
			}
		}
		if (!next.empty())
			result.push_back(next);
		
		for (int j = 0; j < size; ++j)
		{
			TILE_CONTENT tile = static_cast<TILE_CONTENT>(std::stoi(result[j]));
			m_grid[convertPointToIndex({i,j})] = tile;
		}
		std::cout << std::endl;
		++i;
	}
	// unsigned startId = start.y * size + start.x;
	// srand(key);
	// for (int i = 0; i < (int)total * wallLoad;)
	// {
	// 	unsigned chosen = rand() % total;
	// 	if (chosen == startId)
	// 		continue;
	// 	if (m_grid[chosen] == TILE_EMPTY)
	// 	{
	// 		m_grid[chosen] = TILE_WALL;
	// 		++i;
	// 	}
	// }
	std::cout << "Maze " << key << std::endl;
	for (int row = (int)size - 1; row >= 0; --row)
	{
		for (int col = 0; col < (int)size; ++col)
		{
			std::cout << m_grid[convertPointToIndex({row,col})];
			// if (m_grid[row * size + col] == TILE_WALL)
			// 	std::cout << "1 ";
			// else
			// 	std::cout << "0 ";
		}
		std::cout << std::endl;
	}
}

unsigned Maze::GetKey()
{
	return m_key;
}

unsigned Maze::GetSize()
{
	return m_size;
}

MazePt Maze::GetCurr()
{
	return m_curr;
}

int Maze::GetNumMove()
{
	return m_numMove;
}

void Maze::SetCurr(MazePt newCurr)
{
	++m_numMove;
	m_curr = newCurr;
}

void Maze::SetNumMove(int num)
{
	m_numMove = num;
}

int Maze::convertPointToIndex(MazePt pt)
{
	return pt.y * m_size + pt.x;
}

const std::vector<Maze::TILE_CONTENT>& Maze::getGrid()
{
	return m_grid;
}

bool Maze::Move(DIRECTION direction)
{
	++m_numMove;
	MazePt temp = m_curr;
	switch (direction)
	{
	case DIR_LEFT:
		if (temp.x == 0)
			return false;
		temp.x -= 1;
		break;
	case DIR_RIGHT:
		if (temp.x == (int)m_size - 1)
			return false;
		temp.x += 1;
		break;
	case DIR_UP:
		if (temp.y == (int)m_size - 1)
			return false;
		temp.y += 1;
		break;
	case DIR_DOWN:
		if (temp.y == 0)
			return false;
		temp.y -= 1;
		break;
	}
	int tempId = temp.y * m_size + temp.x;
	if (m_grid[tempId] == TILE_WALL)
		return false;
	m_curr = temp;
	return true;
}
