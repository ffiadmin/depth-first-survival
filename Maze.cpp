#include "Maze.h"

Maze::Maze(Dimension dim, ID3D10Device *md3dDevice) {
//Set up the random generator
	srand(time(NULL));

//Set up the maze size and DFS algorithm
	size = dim;
	totalCells = dim.x * dim.y;
	visited = 1;

//Create the matrix
	Location loc;

	for(int i = 0; i < dim.x; ++i) {
		vector<Node*> row;

		for(int j = 0; j < dim.y; ++j) {
			loc.x = i;
			loc.y = j;
			
			row.push_back(new Node(loc));
		}

		grid.push_back(row);
	}

//Starting position
	loc.x = rand() % dim.x;
	loc.y = rand() % dim.y;
	
	root = grid[loc.x][loc.y];

//Determine the number of walls needed for the maze
	wallCount = (2 * dim.x * dim.y) - dim.x - dim.y; //Number of inner walls, 2xy - x - y
	walls.resize(wallCount);

//Initialize the box object for the walls
	box.init(md3dDevice, 1.0f, CYAN);

	for(int i = 0; i < wallCount; ++i) {
		walls[i].init(&box, sqrt(2.0f), Vector3(4, 0, 0), Vector3(0, 0, 0), 0, 1);
	}
}

Maze::~Maze() {
}

void Maze::build() {
	Node *currentCell = root;
	Node *nextCell = NULL;
	locations.push(currentCell);

	while(visited < totalCells) {
		nextCell = inaccessableSiblingCell(currentCell);

		if (nextCell != NULL) {
		//What is the direction of this new cell?
			if (currentCell->location.x > nextCell->location.x) { // NORTH
				currentCell->children.north = nextCell;
			}

			if (currentCell->location.x < nextCell->location.x) { // SOUTH
				currentCell->children.south = nextCell;
			}

			if (currentCell->location.y > nextCell->location.y) { // WEST
				currentCell->children.west = nextCell;
			}

			if (currentCell->location.y < nextCell->location.y) { // EAST
				currentCell->children.east = nextCell;
			}
			
		//Log this transaction
			locations.push(currentCell);
			currentCell = nextCell;
			++visited;
		} else {
			currentCell = locations.top();
			locations.pop();
		}
	}
}

void Maze::draw(ID3D10EffectMatrixVariable *mfxWVPVar, ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX) {
	D3DXMATRIX worldViewProj;
	D3DXMatrixIdentity(&worldViewProj);

	//for(int i = 0; i < wallCount; ++i) {
		worldViewProj = walls[0].getWorldMatrix() * viewMTX * projMTX;
		mfxWVPVar->SetMatrix((float*)&worldViewProj);
		walls[0].setMTech(technique);
		walls[0].draw();
	//}
}

Node *Maze::inaccessableSiblingCell(Node *currentCell) {
//Choose a random direction
	Direction direction = static_cast<Direction>(rand() % 4);

//Find a cell with no knocked down walls
	Location loc;
	Node *operable = NULL;

	for(int i = 0; i <= 4; ++i) {
		direction = static_cast<Direction>((direction + 1) % 4);

	//Map the chosen direction to a node and stay within the matrix bounds
		switch(direction) {
			case NORTH : 
				loc.x = currentCell->location.x;
				loc.y = currentCell->location.y - 1;

				if (loc.y > 0) {
					operable = grid[loc.x][loc.y];
				}

				break;

			case EAST : 
				loc.x = currentCell->location.x + 1;
				loc.y = currentCell->location.y;
				
				if (loc.x < size.x) {
					operable = grid[loc.x][loc.y];
				}
				
				break;

			case SOUTH : 
				loc.x = currentCell->location.x;
				loc.y = currentCell->location.y + 1;

				if (loc.y < size.y) {
					operable = grid[loc.x][loc.y];
				}
				
				break;

			case WEST : 
				loc.x = currentCell->location.x - 1;
				loc.y = currentCell->location.y;
				
				if (loc.x > 0) {
					operable = grid[loc.x][loc.y];
				}
				
				break;
		}

	//Are all the walls intact?
		if (operable != NULL &&
			operable->children.north == NULL &&
			operable->children.east  == NULL &&
			operable->children.south == NULL &&
			operable->children.west  == NULL) {
				return operable;
		} else {
			operable = NULL;
		}
	}

	return NULL;
}

void Maze::update(float dt) {
	//for(int i = 0; i < wallCount; ++i) {
		walls[0].update(dt);
	//}
}
