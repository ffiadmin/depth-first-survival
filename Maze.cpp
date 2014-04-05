#include "Maze.h"

Maze::Maze(Dimension dim, ID3D10EffectMatrixVariable *mfxWVPVar, ID3D10Device *md3dDevice) {
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
	wallsConstructed = 0;
	
//Debug output
	wchar_t buffer[10];
	_itow_s(wallCount, buffer, 10);

	OutputDebugString(L"\n*******************************************\n");
	OutputDebugString(L"MAZE CLASS OUTPUT\n");
	OutputDebugString(L"*******************************************\n");
	OutputDebugString(L"Generated: ");
	OutputDebugString(buffer);
	OutputDebugString(L" walls\n\n");

//Initialize the box object for the walls
	box.init(md3dDevice, 1.0f);

	for(int i = 0; i < wallCount; ++i) {
		walls[i].init(&box, mfxWVPVar, sqrt(2.0f), Vector3(0, 0, 0), Vector3(0, 0, 0), 0,
				D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_LENGTH));
	}
}

Maze::~Maze() {
}

void Maze::addWalls(Node *cell) {
	//int xEast = 2 * mazeNS::CELL_LENGTH * cell->location.x;
	int xEast = 2 * ((mazeNS::CELL_LENGTH / 2.0f) + cell->location.x * (mazeNS::CELL_LENGTH + mazeNS::WALL_THICK));
	//int zEast = 2 * (mazeNS::CELL_WIDTH * cell->location.y) + 2 * mazeNS::CELL_WIDTH;
	int zEast = 2 * ((1 + cell->location.y) * (mazeNS::CELL_WIDTH + mazeNS::WALL_THICK) - mazeNS::WALL_THICK / 2.0f);
	//int xSouth = 2 * mazeNS::CELL_LENGTH * cell->location.x + 2 * mazeNS::CELL_WIDTH;
	int xSouth = 2 * ((1 + cell->location.x) * (mazeNS::CELL_WIDTH + mazeNS::WALL_THICK));
	//int zSouth = 2 * mazeNS::CELL_WIDTH * cell->location.y;// + mazeNS::CELL_WIDTH;
	int zSouth = 2 * (mazeNS::CELL_WIDTH / 2.0f + cell->location.y * (mazeNS::CELL_WIDTH + mazeNS::WALL_THICK));

//Is this a non-last row, non-last column cell? Add at most two walls. [E/S]
	if (cell->location.x != size.x && cell->location.y != size.y) {
		if (cell->children.east == NULL) {
			walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
			walls[wallsConstructed].setPosition(D3DXVECTOR3(xEast, 0, zEast));

			++wallsConstructed;
		}
		
		if (cell->children.south == NULL) {
			walls[wallsConstructed].setPosition(D3DXVECTOR3(xSouth, 0, zSouth));

			++wallsConstructed;
		}
	}
	
//Is this a non-last row, last column cell? Add at most one wall. [S]
	if (cell->location.x != size.x && cell->location.y == size.y) {
		if (cell->children.south == NULL) {
			walls[wallsConstructed].setPosition(D3DXVECTOR3(xSouth, 0, zSouth));

			++wallsConstructed;
		}
	}

//Is this a last row, non-last column cell? Add at most one wall. [E]
	if (cell->location.x == size.x && cell->location.y != size.y) {
		if (cell->children.east == NULL) {
			walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
			walls[wallsConstructed].setPosition(D3DXVECTOR3(xEast, 0, zEast));

			++wallsConstructed;
		}
	}

//Is this a last row, last column cell?
	// DO NOTHING!!!!
	// *Beat box party*
}

void Maze::build() {
	Node *currentCell = root;
	Node *lastCell = NULL;
	Node *nextCell = NULL;

//Debug output
	wchar_t visit[10], xCur[10], yCur[10], xNext[10], yNext[10];
	_itow_s(currentCell->location.x, xCur, 10);
	_itow_s(currentCell->location.y, yCur, 10);

	OutputDebugString(L"Start: (");
	OutputDebugString(xCur);
	OutputDebugString(L", ");
	OutputDebugString(yCur);
	OutputDebugString(L")\n");
	OutputDebugString(L"Visited: 1 cell\n");

	do {
		nextCell = inaccessableSiblingCell(currentCell);

		if (nextCell != NULL) {
		//Debug ouputput
			_itow_s(currentCell->location.x, xCur, 10);
			_itow_s(currentCell->location.y, yCur, 10);
			_itow_s(nextCell->location.x, xNext, 10);
			_itow_s(nextCell->location.y, yNext, 10);

			OutputDebugString(L"Advance: (");
			OutputDebugString(xCur);
			OutputDebugString(L", ");
			OutputDebugString(yCur);
			OutputDebugString(L") binding ");

		//What is the direction of this new cell?
			if (currentCell->location.x > nextCell->location.x) { // WEST
				OutputDebugString(L"WEST");

				currentCell->children.west = nextCell;
				nextCell->children.east = currentCell;
			}

			if (currentCell->location.x < nextCell->location.x && nextCell->location.x < size.x - 1) { // EAST
				OutputDebugString(L"EAST");

				currentCell->children.east = nextCell;
				nextCell->children.west = currentCell;
			}

			if (currentCell->location.y > nextCell->location.y) { // SOUTH
				OutputDebugString(L"SOUTH");

				currentCell->children.south = nextCell;
				nextCell->children.north = currentCell;
			}

			if (currentCell->location.y < nextCell->location.y) { // NORTH
				OutputDebugString(L"NORTH");

				currentCell->children.north = nextCell;
				nextCell->children.south = currentCell;
			}
			
		//Log this transaction
			locations.push(currentCell);
			currentCell = nextCell;
			lastCell = currentCell;
			++visited;

		//Debug output
			_itow_s(visited, visit, 10);

			OutputDebugString(L" with (");
			OutputDebugString(xNext);
			OutputDebugString(L", ");
			OutputDebugString(yNext);
			OutputDebugString(L")\n");
			OutputDebugString(L"Visited: ");
			OutputDebugString(visit);
			OutputDebugString(L" cells\n");
		} else {
		//Debug ouputput
			_itow_s(currentCell->location.x, xCur, 10);
			_itow_s(currentCell->location.y, yCur, 10);

			OutputDebugString(L"   Retreat: (");
			OutputDebugString(xCur);
			OutputDebugString(L", ");
			OutputDebugString(yCur);
			OutputDebugString(L") falling back to ");

			currentCell = locations.top();
			locations.pop();

		//Debug output
			_itow_s(currentCell->location.x, xCur, 10);
			_itow_s(currentCell->location.y, yCur, 10);

			OutputDebugString(L"(");
			OutputDebugString(xCur);
			OutputDebugString(L", ");
			OutputDebugString(yCur);
			OutputDebugString(L")\n");
		}
	} while (locations.size() > 0);

//Debug output
	_itow_s(lastCell->location.x, xCur, 10);
	_itow_s(lastCell->location.y, yCur, 10);

	OutputDebugString(L"End: (");
	OutputDebugString(xCur);
	OutputDebugString(L", ");
	OutputDebugString(yCur);
	OutputDebugString(L")\n\n");

//Display all the connections!
	OutputDebugString(L"Connected edges:\n");

	for(int i = 0; i < size.x; ++i) {
		for(int j = 0; j < size.y; ++j) {
			_itow_s(i, xCur, 10);
			_itow_s(j, yCur, 10);

			OutputDebugString(L"(");
			OutputDebugString(xCur);
			OutputDebugString(L", ");
			OutputDebugString(yCur);
			OutputDebugString(L") is connected with: ");

		//East connection
			if (grid[i][j]->children.east != NULL) {
				_itow_s(i + 1, xNext, 10);
				_itow_s(j, yNext, 10);

				OutputDebugString(L"[East] (");
				OutputDebugString(xNext);
				OutputDebugString(L", ");
				OutputDebugString(yNext);
				OutputDebugString(L") ");
			}

		//West connection
			if (grid[i][j]->children.west != NULL) {
				_itow_s(i - 1, xNext, 10);
				_itow_s(j, yNext, 10);

				OutputDebugString(L"[West] (");
				OutputDebugString(xNext);
				OutputDebugString(L", ");
				OutputDebugString(yNext);
				OutputDebugString(L") ");
			}

		//North connection
			if (grid[i][j]->children.north != NULL) {
				_itow_s(i, xNext, 10);
				_itow_s(j - 1, yNext, 10);

				OutputDebugString(L"[North] (");
				OutputDebugString(xNext);
				OutputDebugString(L", ");
				OutputDebugString(yNext);
				OutputDebugString(L") ");
			}

		//South connection
			if (grid[i][j]->children.south != NULL) {
				_itow_s(i, xNext, 10);
				_itow_s(j + 1, yNext, 10);

				OutputDebugString(L"[South] (");
				OutputDebugString(xNext);
				OutputDebugString(L", ");
				OutputDebugString(yNext);
				OutputDebugString(L") ");
			}

			OutputDebugString(L"\n");
		}
	}

//Position the walls
	for(int i = 0; i < size.x; ++i) {
		for(int j = 0; j < size.y; ++j) {
			addWalls(grid[i][j]);
		}
	}
}

void Maze::draw(ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX) {
	D3DXMATRIX worldViewProj;
	D3DXMatrixIdentity(&worldViewProj);

	for(int i = 0; i < wallsConstructed; ++i) {
		walls[i].draw(viewMTX, projMTX, technique);
	}
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

				if (loc.y >= 0) {
					operable = grid[loc.x][loc.y];
				}

				break;

			case EAST : 
				loc.x = currentCell->location.x + 1;
				loc.y = currentCell->location.y;
				
				if (loc.x < size.x - 1) {
					operable = grid[loc.x][loc.y];
				}
				
				break;

			case SOUTH : 
				loc.x = currentCell->location.x;
				loc.y = currentCell->location.y + 1;

				if (loc.y < size.y - 1) {
					operable = grid[loc.x][loc.y];
				}
				
				break;

			case WEST : 
				loc.x = currentCell->location.x - 1;
				loc.y = currentCell->location.y;
				
				if (loc.x >= 0) {
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
	for(int i = 0; i < wallsConstructed; ++i) {
		walls[i].update(dt);
	}
}
