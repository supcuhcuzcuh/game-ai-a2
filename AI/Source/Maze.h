#ifndef MAZE_H
#define MAZE_H

#include <vector>

struct MazePt
{
	int x, y;
	MazePt(int _x = 0, int _y = 0) : x(_x), y(_y) {}
	void Set(int _x = 0, int _y = 0) {x = _x; y = _y;}
	float length();
};

class Maze
{
public:
	enum TILE_CONTENT
	{
		TILE_EMPTY,
		TILE_WALL,
		TILE_FOG,
	};
	enum DIRECTION
	{
		DIR_UP,
		DIR_DOWN,
		DIR_LEFT,
		DIR_RIGHT,
	};
	Maze();
	~Maze();
	void Generate(unsigned key, unsigned size, MazePt start, float wallLoad);
	bool Move(DIRECTION direction);
	unsigned GetKey();
	unsigned GetSize();
	MazePt GetCurr();
	int GetNumMove();
	void SetCurr(MazePt newCurr);
	void SetNumMove(int num);
	int convertPointToIndex(MazePt pt);

	const std::vector<TILE_CONTENT>& getGrid();
private:
	//Do not make these public
	unsigned m_key;
	unsigned m_size; // The Width and Height of the maze
	std::vector<TILE_CONTENT> m_grid; // The actual physical grid data
	MazePt m_curr;
	int m_numMove;
};

#endif