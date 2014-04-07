#pragma once
//#define DEBUG_ENABLED

#include <cmath>
#include <ctime>
#include <stack>
#include <vector> // *Snicker*

#include "Box.h"
#include "GameObject.h"

using std::stack;
using std::vector;

namespace mazeNS {
//Cell dimensions
	const int CELL_LENGTH = 10; // X
	const int CELL_PAD    = 2;
	const int CELL_WIDTH  = 10; // Z

//Wall dimensions
	const int WALL_HEIGHT = 10;
	const int WALL_THICK  = 1;
}

enum Direction { NORTH, EAST, SOUTH, WEST };
struct Boundaries;
struct Node;

struct Borders {
	Borders() { east = NULL; north = NULL; south = NULL; west = NULL; }
	~Borders() { delete east; delete north; delete south; delete west; }

	Boundaries *east;
	Boundaries *north;
	Boundaries *south;
	Boundaries *west;
};

struct Boundaries {
	float farDim;
	float nearDim;
};

struct Dimension {
	int x;
	int z;
};

struct Intersection {
	Intersection() { east = NULL; north = NULL; south = NULL; west = NULL; }
	~Intersection() { delete east; delete north; delete south; delete west; }

	Node *east;
	Node *north;
	Node *south;
	Node *west;
};

struct Location {
	int x;
	int z;
};

struct Node {
	Node() : toEnd(false) { location.x = 0; location.z = 0; }
	Node(Location location) : location(location), toEnd(false) { }

	Intersection children;
	Location location;
	bool toEnd;
};

class Maze {
public : 
	Maze(Dimension dim, ID3D10EffectMatrixVariable *mfxWVPVar, ID3D10Device *md3dDevice);
	~Maze();

public : 
	void build();
	void draw(ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX);
	void update(float dt);

public : 
	Location cellToPx(Location cell);
	bool collided(Location px);
	Location pxToCell(Location px);

public : 
	Location getEndPosition();
	Location getStartPosition();

public : 
	void setStartPosition(Location location);

private : 
	void addWalls(Node *cell);
	Node *inaccessableSiblingCell(Node *currentCell);

private : 
	Box box;
	vector<vector<Node*>> grid;
	stack<Node*> locations;
	ID3D10Device *md3dDevice;
	Node *root;
	vector<GameObject> walls;

private : 
	Location end;
	Dimension size;
	Location start;
	int totalCells;
	int visited;
	int wallCount;
	int wallsConstructed;
};
