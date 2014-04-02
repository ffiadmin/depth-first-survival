#pragma once

#include <cmath>
#include <ctime>
#include <stack>
#include <vector> // *Snicker*

#include "Box.h"
#include "GameObject.h"

using std::stack;
using std::vector;

namespace mazeNS {
	const int CELL_HEIGHT = 10;
	const int CELL_LENGTH = 10;
	const int CELL_WIDTH  = 10;
}

enum Direction { NORTH, EAST, SOUTH, WEST };
struct Node;

struct Intersection {
	Intersection() { east = NULL; north = NULL; south = NULL; west = NULL; }

	Node *east;
	Node *north;
	Node *south;
	Node *west;
};

struct Dimension {
	int x;
	int y;
};

struct Location {
	int x;
	int y;
};

struct Node {
	Node() : toEnd(false) { location.x = 0; location.y = 0; }
	Node(Location location) : location(location), toEnd(false) { }

	Intersection children;
	Location location;
	bool toEnd;
};

class Maze {
public : 
	Maze(Dimension dim, ID3D10Device *md3dDevice);
	~Maze();

public : 
	void build();
	void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX);
	void update(float dt);

public : 
	Location getEndPosition();
	Location getStartPosition();

public : 
	void setEndPosition(Location location);
	void setStartPosition(Location location);
vector<GameObject> walls;
private : 
	Node *inaccessableSiblingCell(Node *currentCell);

private : 
	Box box;
	vector<vector<Node*>> grid;
	stack<Node*> locations;
	ID3D10Device *md3dDevice;
	Node *root;
	

private : 
	Location end;
	Dimension size;
	Location start;
	int totalCells;
	int visited;
	int wallCount;
};
