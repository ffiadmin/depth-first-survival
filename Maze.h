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

enum Backtrack { END, START };
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
	Intersection() { east = NULL; north = NULL; south = NULL; toRoot = NULL; west = NULL; }
	~Intersection() { delete east; delete north; delete south; delete west; }

	Node *east;
	Node *north;
	Node *south;
	Node *toRoot;
	Node *west;
};

struct Dimension {
	friend bool operator==(const Dimension &d1, const Dimension &d2) {
		return (d1.x == d2.x) && (d1.z == d2.z);
	}

	int x;
	int z;
};

struct Location {
	friend bool operator==(const Location &l1, const Location &l2) {
		return (l1.x == l2.x) && (l1.z == l2.z);
	}

	friend bool operator!=(const Location &l1, const Location &l2) {
		return (l1.x != l2.x) || (l1.z != l2.z);
	}

	int x;
	int z;
};

struct Node {
	Node() : toEnd(false) { location.x = 0; location.z = 0; }
	Node(Location location) : location(location), toEnd(false) { }

	friend bool operator==(const Node &n1, const Node &n2) {
		return (n1.location.x == n2.location.x) && (n1.location.z == n2.location.z);
	}

	Intersection children;
	Location location;
	bool toEnd;
};

struct Solution {
	Solution() { end = false; location.x = -1; location.z = -1; next = NULL; previous = NULL; start = false; }
	~Solution() {
		/*
 		if(next != NULL && location.x != NULL && location.z != NULL && location.x >= 0 && location.z >= 0) {
			delete next;
			next = NULL;

			if(previous &&
			previous != NULL &&
			previous->location.x != NULL &&
			previous->location.z != NULL &&
			previous->location.x < 0 &&
			previous->location.z < 0) {
				delete previous;
				previous = NULL;
			}
		}

	//Delete the previous pointer, if it is actually the first item
		if (0 != previous && previous->location.x <= 0 && previous->location.z <= 0) {
			delete previous;
			previous = 0;
		}

	//Delete the next pointer
		if (0 != next && next->location.x > 0 && next->location.z > 0) {
			delete next;
			next = 0;
		}

	//If this node wasn't left at is initialized state of {-1, -1}, delete next
		if (next) {
			delete (next);
			(next) = NULL;
		}

		if (!delFlag) {
			delFlag = true;
			vector<Solution*> del;

		//Go to the end of the linked-list, and push them onto a vector
			for(Solution *current = next;
				current->location.x >= 0 && current->location.z >= 0; 
				current = current->next) {
				current->delFlag = true;
				del.push_back(current);
			}

		//Now nuke each of them!
			for(int i = 0; i < del.size(); ++i) {
				delete del[i];
			}

			del.clear();

		//Is there a previous node which could use deletion?
			if (previous && previous != NULL && previous->location.x < 0 && previous->location.z < 0) {
				delete previous;
			}
		}*/
	}

	void operator=(const Node &n) {
		location.x = n.location.x;
		location.z = n.location.z;
	}

	void operator=(const Node *n) {
		location.x = n->location.x;
		location.z = n->location.z;
	}

	bool end;
	Location location;
	Solution *next;
	Solution *previous;
	bool start;
};

class Maze {
public : 
	Maze();
	~Maze();
	
public : 
	void build();
	bool collided(Location px);
	void draw(ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX, ID3D10EffectTechnique *ceilTech = 0);
	void init(Dimension dim, ID3D10EffectMatrixVariable *mfxWVPVar, ID3D10EffectMatrixVariable* fx2, ID3D10Device *md3dDevice);
	void update(float dt);
	
public : 
	Location cellToPx(Location cell);
	Location pxToCell(Location px);
	Solution solve();
	Solution solve(Location from);
	Solution solve(Location from, Location to);

public : 
	Location getCenterCell();
	Location getCenterPx();
	Location getEndPosition();
	Dimension getDimension();
	Location getStartPosition();

public : 
	void setCeilingVisibility(bool visible);
	void setCeilTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture);
	void setFloorTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture);
	void setStartPosition(Location location);
	void setTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture);
	void setTexLocVariable(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc);

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
	int **solved;
	int solvedCounter;
	Location start;
	bool startSet;
	int totalCells;
	int visited;
	int wallCount;
	int wallsConstructed;
};