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
	const int CELL_PAD    = 1;
	const int CELL_WIDTH  = 10; // Z

//Wall dimensions
	const int WALL_HEIGHT = 10;
	const int WALL_THICK  = 1;
}

enum Direction { NORTH, EAST, SOUTH, WEST };
struct Node;

struct Boundaries {
	float farDim;
	float nearDim;
};

struct Borders {
	Boundaries east;
	Boundaries north;
	Boundaries south;
	Boundaries west;
};

struct Intersection {
	Intersection() { east = NULL; north = NULL; south = NULL; west = NULL; }
	~Intersection() { delete east; delete north; delete south; delete west; }

	Node *east;
	Node *north;
	Node *south;
	Node *west;
};

struct Dimension {
	int x;
	int z;
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
	Maze();
	~Maze();
	
public : 
	void build();
	bool collided(Location px);
	void draw(ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX);
	void init(Dimension dim, ID3D10EffectMatrixVariable *mfxWVPVar, ID3D10EffectMatrixVariable* fx2,ID3D10Device *md3dDevice);
	void update(float dt);
	
public : 
	Location cellToPx(Location cell);
	Location pxToCell(Location px);

public : 
	Location getCenterCell();
	Location getCenterPx();
	Location getEndPosition();
	Dimension getDimension();
	Location getStartPosition();

public : 
	void setCeilingVisibility(bool visible);
	void setCeilTex(ID3D10EffectShaderResourceVariable *diffuseLoc, ID3D10EffectShaderResourceVariable *specLoc, wchar_t *diffuseMap, wchar_t *specMap);
	void setFloorTex(ID3D10EffectShaderResourceVariable *diffuseLoc, ID3D10EffectShaderResourceVariable *specLoc, wchar_t *diffuseMap, wchar_t *specMap);
	void setStartPosition(Location location);
	void setTex(ID3D10EffectShaderResourceVariable *diffuseLoc, ID3D10EffectShaderResourceVariable *specLoc, wchar_t *diffuseMap, wchar_t *specMap);

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
	bool alreadyInit;
	Dimension dim;
	Location end;
	Dimension size;
	Location start;
	int totalCells;
	int visited;
	int wallCount;
	int wallsConstructed;
};