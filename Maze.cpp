#include "Maze.h"

Maze::Maze() : alreadyInit(false), solvedCounter(1), startSet(false) { }

Maze::~Maze() {
	delete root;

//Deallocate the maze resolution matrix
	for(int i = 0; i < this->dim.x; ++i) {
		delete [] solved[i];
	}

	delete [] solved;
}

void Maze::addWalls(Node *cell) {
//Place the east wall
	if (cell->children.east == NULL && cell->location.x >= 0 && cell->location.x < dim.x - 1) {
		int xEast = 2 * (cell->location.x + 1) * mazeNS::CELL_LENGTH;
		int zEast = 2 * (mazeNS::CELL_WIDTH / 2.0f + (cell->location.z * mazeNS::CELL_WIDTH));

		walls[wallsConstructed].setPosition(D3DXVECTOR3(xEast, 0, zEast));
		++wallsConstructed;
	}
	
//Place the south wall
	if (cell->children.south == NULL && cell->location.z > 0 && cell->location.z < dim.z) {
		int xSouth = 2 * (mazeNS::CELL_LENGTH / 2.0f + (cell->location.x * mazeNS::CELL_LENGTH));
		int zSouth = 2 * cell->location.z * mazeNS::CELL_WIDTH;

		walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_LENGTH));
		walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
		walls[wallsConstructed].setPosition(D3DXVECTOR3(xSouth, 0, zSouth));
		++wallsConstructed;
	}
}

void Maze::build() {
	Node *currentCell = root;
	Node *lastCell = NULL;
	Node *nextCell = NULL;

	#ifdef DEBUG_ENABLED
	//Debug output
		wchar_t visit[10], xCur[10], zCur[10], xNext[10], zNext[10];
		_itow_s(currentCell->location.x, xCur, 10);
		_itow_s(currentCell->location.z, zCur, 10);
	
		OutputDebugString(L"Start: (");
		OutputDebugString(xCur);
		OutputDebugString(L", ");
		OutputDebugString(zCur);
		OutputDebugString(L")\n");
		OutputDebugString(L"Visited: 1 cell\n");
	#endif

	do {
		nextCell = inaccessableSiblingCell(currentCell);

		if (nextCell != NULL) {
			#ifdef DEBUG_ENABLED
			//Debug ouputput
				_itow_s(currentCell->location.x, xCur, 10);
				_itow_s(currentCell->location.z, zCur, 10);
				_itow_s(nextCell->location.x, xNext, 10);
				_itow_s(nextCell->location.z, zNext, 10);

				OutputDebugString(L"Advance: (");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L") binding ");
			#endif

		//Assume this is part of the solution to the maze
			currentCell->toEnd = true;

		//What is the direction of this new cell?
			if (currentCell->location.x > nextCell->location.x) { // WEST
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"WEST");
				#endif

				currentCell->children.west = nextCell;
				nextCell->children.east = currentCell;
			}

			if (currentCell->location.x < nextCell->location.x) { // EAST
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"EAST");
				#endif

				currentCell->children.east = nextCell;
				nextCell->children.west = currentCell;
			}

			if (currentCell->location.z > nextCell->location.z) { // SOUTH
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"SOUTH");
				#endif

				currentCell->children.south = nextCell;
				nextCell->children.north = currentCell;
			}

			if (currentCell->location.z < nextCell->location.z) { // NORTH
				#ifdef DEBUG_ENABLED
					OutputDebugString(L"NORTH");
				#endif

				currentCell->children.north = nextCell;
				nextCell->children.south = currentCell;
			}

		//Always have a path back to the root
			nextCell->children.toRoot = currentCell;
			
		//Log this transaction
			locations.push(currentCell);
			currentCell = nextCell;
			lastCell = currentCell;
			++visited;

			#ifdef DEBUG_ENABLED
			//Debug output
				_itow_s(visited, visit, 10);
			
				OutputDebugString(L" with (");
				OutputDebugString(xNext);
				OutputDebugString(L", ");
				OutputDebugString(zNext);
				OutputDebugString(L")\n");
				OutputDebugString(L"Visited: ");
				OutputDebugString(visit);
				OutputDebugString(L" cells\n");
			#endif
		} else {
			#ifdef DEBUG_ENABLED
			//Debug ouputput
				_itow_s(currentCell->location.x, xCur, 10);
				_itow_s(currentCell->location.z, zCur, 10);

				OutputDebugString(L"   Retreat: (");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L") falling back to ");
			#endif

		//Since we are backtracking, this is not part of the solution
			currentCell->toEnd = false;

		//Go back one step
			currentCell = locations.top();
			locations.pop();

			#ifdef DEBUG_ENABLED
			//Debug output
				_itow_s(currentCell->location.x, xCur, 10);
				_itow_s(currentCell->location.z, zCur, 10);

				OutputDebugString(L"(");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L")\n");
			#endif
		}
	} while (visited < totalCells);

//The last cell is the solution
	lastCell->toEnd = true;

//Save the end cell
	end = lastCell->location;

	#ifdef DEBUG_ENABLED
	//Debug output
		_itow_s(lastCell->location.x, xCur, 10);
		_itow_s(lastCell->location.z, zCur, 10);

		OutputDebugString(L"End: (");
		OutputDebugString(xCur);
		OutputDebugString(L", ");
		OutputDebugString(zCur);
		OutputDebugString(L")\n\n");

	//Display all the connections!
		OutputDebugString(L"Connected edges:\n");

		for(int i = 0; i < dim.x; ++i) {
			for(int j = 0; j < dim.z; ++j) {
				_itow_s(i, xCur, 10);
				_itow_s(j, zCur, 10);

				OutputDebugString(L"(");
				OutputDebugString(xCur);
				OutputDebugString(L", ");
				OutputDebugString(zCur);
				OutputDebugString(L") is connected with: ");

			//East connection
				if (grid[i][j]->children.east != NULL) {
					_itow_s(i + 1, xNext, 10);
					_itow_s(j, zNext, 10);

					OutputDebugString(L"[East] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

			//West connection
				if (grid[i][j]->children.west != NULL) {
					_itow_s(i - 1, xNext, 10);
					_itow_s(j, zNext, 10);

					OutputDebugString(L"[West] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

			//North connection
				if (grid[i][j]->children.north != NULL) {
					_itow_s(i, xNext, 10);
					_itow_s(j + 1, zNext, 10);

					OutputDebugString(L"[North] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

			//South connection
				if (grid[i][j]->children.south != NULL) {
					_itow_s(i, xNext, 10);
					_itow_s(j - 1, zNext, 10);

					OutputDebugString(L"[South] (");
					OutputDebugString(xNext);
					OutputDebugString(L", ");
					OutputDebugString(zNext);
					OutputDebugString(L") ");
				}

				OutputDebugString(L"\n");
			}
		}
	#endif

//Position the walls
	for(int i = 0; i < dim.x; ++i) {
		for(int j = 0; j < dim.z; ++j) {
			addWalls(grid[i][j]);
		}
	}

//Add the north bounding wall
	for(int i = 0; i < dim.x; ++i) {
		walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_LENGTH));
		walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
		walls[wallsConstructed].setPosition(D3DXVECTOR3(mazeNS::CELL_LENGTH + (2 * i * mazeNS::CELL_LENGTH), 0, 2 * mazeNS::CELL_WIDTH * dim.z));
		++wallsConstructed;
	}

//Add the east bounding wall
	for(int i = 0; i < dim.z; ++i) {
		walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH));
		walls[wallsConstructed].setPosition(D3DXVECTOR3(0, 0, mazeNS::CELL_WIDTH + (2 * i * mazeNS::CELL_WIDTH)));
		++wallsConstructed;
	}

//Add the south bounding wall
	for(int i = 0; i < dim.x; ++i) {
		walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_LENGTH));
		walls[wallsConstructed].setRotation(D3DXVECTOR3(0, ToRadian(90), 0));
		walls[wallsConstructed].setPosition(D3DXVECTOR3(mazeNS::CELL_LENGTH + (2 * i * mazeNS::CELL_LENGTH), 0, 0));
		++wallsConstructed;
	}

//Add the west bounding wall
	for(int i = 0; i < dim.z; ++i) {
		walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH));
		walls[wallsConstructed].setPosition(D3DXVECTOR3(2 * mazeNS::CELL_LENGTH * dim.x, 0, mazeNS::CELL_WIDTH + (2 * i * mazeNS::CELL_WIDTH)));
		++wallsConstructed;
	}

//Add the floor
	walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::CELL_LENGTH * dim.x, mazeNS::CELL_WIDTH * dim.z));
	walls[wallsConstructed].setRotation(D3DXVECTOR3(0, 0, ToRadian(90)));
	walls[wallsConstructed].setPosition(D3DXVECTOR3(mazeNS::CELL_LENGTH * dim.x, -mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH * dim.z));
	++wallsConstructed;

//Add the ceiling
	walls[wallsConstructed].setScale(D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::CELL_LENGTH * dim.x, mazeNS::CELL_WIDTH * dim.z));
	walls[wallsConstructed].setRotation(D3DXVECTOR3(0, 0, ToRadian(90)));
	walls[wallsConstructed].setPosition(D3DXVECTOR3(mazeNS::CELL_LENGTH * dim.x, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH * dim.z));
	++wallsConstructed;
}

Location Maze::cellToPx(Location cell) {
//Ensure this request is not out of bounds
	if (cell.x >= dim.x || cell.z >= dim.z || cell.x < 0 || cell.z < 0) {
		Location loc = {-1, -1};
		return loc;
	}

//Calculate the grid location
	Location loc;
	loc.x = 2 * ((mazeNS::CELL_LENGTH / 2.0f) + (cell.x * mazeNS::CELL_LENGTH));
	loc.z = 2 * ((mazeNS::CELL_WIDTH / 2.0f) + (cell.z * mazeNS::CELL_WIDTH));
	return loc;
}

bool Maze::collided(Location px) {
	#ifdef DEBUG_ENABLED
	//Debug output
		wchar_t xLoc[10], zLoc[10];
		_itow_s(px.x, xLoc, 10);
		_itow_s(px.z, zLoc, 10);
	
		OutputDebugString(L"Checking collision at location: (");
		OutputDebugString(xLoc);
		OutputDebugString(L", ");
		OutputDebugString(zLoc);
		OutputDebugString(L")\n");
	#endif

//Get the cell grid location
	Location cell = pxToCell(px);

	if (cell.x == -1) return true;

	#ifdef DEBUG_ENABLED
	//Debug output
		_itow_s(cell.x, xLoc, 10);
		_itow_s(cell.z, zLoc, 10);
	
		OutputDebugString(L"Location resolves to cell: (");
		OutputDebugString(xLoc);
		OutputDebugString(L", ");
		OutputDebugString(zLoc);
		OutputDebugString(L")\n");
	#endif

//Get the cell center
	Location center = cellToPx(cell);

	if (center.x == -1) return true;

	#ifdef DEBUG_ENABLED
	//Debug output
		_itow_s(center.x, xLoc, 10);
		_itow_s(center.z, zLoc, 10);
	
		OutputDebugString(L"Center of current cell: (");
		OutputDebugString(xLoc);
		OutputDebugString(L", ");
		OutputDebugString(zLoc);
		OutputDebugString(L")\n");
	#endif

//Get the cell data
	Borders borders;
	Node *location = grid[cell.x][cell.z];

/**
 * Note that CELL_LENGTH and CELL_WIDTH are not divided by two
 * since the unit length of a cube is 2x2x2, and the maze class
 *(correctly) provides and interface assuming that cubes are
 * 1x1x1.
*/

//North wall boundaries
	if (location->children.north == NULL) {
		#ifdef DEBUG_ENABLED
		//Debug output
			OutputDebugString(L"Checking northern boundary... \n");
		#endif

		borders.north.farDim = center.z + mazeNS::CELL_WIDTH;
		borders.north.nearDim = center.z + mazeNS::CELL_WIDTH - (mazeNS::CELL_PAD * 2);

		#ifdef DEBUG_ENABLED
		//Debug output
			_itow_s(borders.north.nearDim, xLoc, 10);
			_itow_s(borders.north.farDim, zLoc, 10);
	
			OutputDebugString(L"Near: ");
			OutputDebugString(xLoc);
			OutputDebugString(L", Far: ");
			OutputDebugString(zLoc);
			OutputDebugString(L"\n");
		#endif

		if (px.z <= borders.north.farDim && px.z >= borders.north.nearDim) {
			#ifdef DEBUG_ENABLED
			//Debug output
				OutputDebugString(L"   COLLIDED WITH NORTH\n");
			#endif

			return true;
		}
	}

//East wall boundaries
	if (location->children.east == NULL) {
		#ifdef DEBUG_ENABLED
		//Debug output
			OutputDebugString(L"Checking eastern boundary... \n");
		#endif

		borders.east.farDim = center.x + mazeNS::CELL_LENGTH;
		borders.east.nearDim = center.x + mazeNS::CELL_LENGTH - (mazeNS::CELL_PAD * 2);

		#ifdef DEBUG_ENABLED
		//Debug output
			_itow_s(borders.east.nearDim, xLoc, 10);
			_itow_s(borders.east.farDim, zLoc, 10);
	
			OutputDebugString(L"Near: ");
			OutputDebugString(xLoc);
			OutputDebugString(L", Far: ");
			OutputDebugString(zLoc);
			OutputDebugString(L"\n");
		#endif

		if (px.x <= borders.east.farDim && px.x >= borders.east.nearDim) {
			#ifdef DEBUG_ENABLED
			//Debug output
				OutputDebugString(L"   COLLIDED WITH EAST\n");
			#endif

			return true;
		}
	}

//South wall boundaries
	if (location->children.south == NULL) {
		#ifdef DEBUG_ENABLED
		//Debug output
			OutputDebugString(L"Checking southern boundary... \n");
		#endif

		borders.south.farDim = center.z - mazeNS::CELL_WIDTH;
		borders.south.nearDim = center.z - mazeNS::CELL_WIDTH + (mazeNS::CELL_PAD * 2);

		#ifdef DEBUG_ENABLED
		//Debug output
			_itow_s(borders.south.nearDim, xLoc, 10);
			_itow_s(borders.south.farDim, zLoc, 10);
	
			OutputDebugString(L"Near: ");
			OutputDebugString(xLoc);
			OutputDebugString(L", Far: ");
			OutputDebugString(zLoc);
			OutputDebugString(L"\n");
		#endif

		if (px.z >= borders.south.farDim && px.z <= borders.south.nearDim) {
			#ifdef DEBUG_ENABLED
			//Debug output
				OutputDebugString(L"   COLLIDED WITH SOUTH\n");
			#endif

			return true;
		}
	}

//West wall boundaries
	if (location->children.west == NULL) {
		#ifdef DEBUG_ENABLED
		//Debug output
			OutputDebugString(L"Checking western boundary... \n");
		#endif

		borders.west.farDim = center.x - mazeNS::CELL_LENGTH;
		borders.west.nearDim = center.x - mazeNS::CELL_LENGTH + (mazeNS::CELL_PAD * 2);

		#ifdef DEBUG_ENABLED
		//Debug output
			_itow_s(borders.west.nearDim, xLoc, 10);
			_itow_s(borders.west.farDim, zLoc, 10);
	
			OutputDebugString(L"Near: ");
			OutputDebugString(xLoc);
			OutputDebugString(L", Far: ");
			OutputDebugString(zLoc);
			OutputDebugString(L"\n");
		#endif

		if (px.x >= borders.west.farDim && px.x <= borders.west.nearDim) {
			#ifdef DEBUG_ENABLED
			//Debug output
				OutputDebugString(L"   COLLIDED WITH WEST\n");
			#endif

			return true;
		}
	}

	#ifdef DEBUG_ENABLED
	//Debug output
		OutputDebugString(L"NO COLLISION\n\n");
	#endif

	return false;
}

void Maze::draw(ID3D10EffectTechnique *technique, D3DXMATRIX viewMTX, D3DXMATRIX projMTX,ID3D10EffectTechnique *ceilTech) {
	D3DXMATRIX worldViewProj;
	D3DXMatrixIdentity(&worldViewProj);

	if (!ceilTech) {
		for(int i = 0; i < wallsConstructed; ++i) {
			walls[i].draw(viewMTX, projMTX, technique);
		}
	} else {
		for(int i = 0; i < wallsConstructed-2; ++i) {
			walls[i].draw(viewMTX, projMTX, technique);
		}

		walls[wallsConstructed - 1].draw(viewMTX, projMTX, ceilTech);
		walls[wallsConstructed - 2].draw(viewMTX, projMTX, ceilTech);
	}
}
Location Maze::getCenterCell() {
	Location center;
	center.x = dim.x / 2;
	center.z = dim.z / 2;

	return center;
}

Location Maze::getCenterPx() {
	Location center;
	center.x = size.x / 2;
	center.z = size.z / 2;

	return center;
}

Location Maze::getEndPosition() {
	return end;
}

Dimension Maze::getDimension() {
	return size;
}

Location Maze::getStartPosition() {
	return start;
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
				loc.z = currentCell->location.z - 1;

				if (loc.z >= 0) {
					operable = grid[loc.x][loc.z];
				}

				break;

			case EAST : 
				loc.x = currentCell->location.x + 1;
				loc.z = currentCell->location.z;
				
				if (loc.x < dim.x) {
					operable = grid[loc.x][loc.z];
				}
				
				break;

			case SOUTH : 
				loc.x = currentCell->location.x;
				loc.z = currentCell->location.z + 1;

				if (loc.z < dim.z) {
					operable = grid[loc.x][loc.z];
				}
				
				break;

			case WEST : 
				loc.x = currentCell->location.x - 1;
				loc.z = currentCell->location.z;
				
				if (loc.x >= 0) {
					operable = grid[loc.x][loc.z];
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

void Maze::init(Dimension dim, ID3D10EffectMatrixVariable *mfxWVPVar, ID3D10EffectMatrixVariable* fx2,ID3D10Device *md3dDevice) {
//Is this maze being re-initialized?
	if (alreadyInit) {
	//Remove the grid
		for(int i = 0; i < this->dim.x; ++i) {
			grid[i].clear();
		}
		
		grid.clear();

	//Empty the stack
		for(int i = 0; i < locations.size(); ++i) {
			locations.pop();
		}
		
	//Deallocate the maze resolution matrix
		for(int i = 0; i < this->dim.x; ++i) {
			delete [] solved[i];
		}

		delete [] solved;

	//Delete the root node
		delete root;

	//Delete all the walls
		walls.clear();

	//Reset variables
		solvedCounter = 1;
		totalCells = 0;
		visited = 0;
		wallCount = 0;
		wallsConstructed = 0;
	}

//Set up the random generator
	srand(time(NULL));

//Set up the maze size and DFS algorithm
	this->dim = dim;
	size.x = 2 * dim.x * mazeNS::CELL_LENGTH;
	size.z = 2 * dim.z * mazeNS::CELL_WIDTH;
	totalCells = dim.x * dim.z;
	visited = 1;

//Create the matrix
	Location loc;

	for(int i = 0; i < dim.x; ++i) {
		vector<Node*> row;

		for(int j = 0; j < dim.z; ++j) {
			loc.x = i;
			loc.z = j;
			
			row.push_back(new Node(loc));
		}

		grid.push_back(row);
	}

//Starting position
	if (startSet) {
		loc = start;
		startSet = false;
	} else {
		loc.x = rand() % dim.x;
		loc.z = rand() % dim.z;

		start = loc;
	}
	
	root = grid[loc.x][loc.z];

//Determine the number of walls needed for the maze
	wallCount = (2 * dim.x * dim.z) + dim.x + dim.z; //Number of walls, 2xy + x + y
	walls.resize(wallCount);
	wallsConstructed = 0;
	
	#ifdef DEBUG_ENABLED
	//Debug output
		wchar_t buffer[10];
		_itow_s(wallCount, buffer, 10);

		OutputDebugString(L"\n*******************************************\n");
		OutputDebugString(L"MAZE CLASS OUTPUT\n");
		OutputDebugString(L"*******************************************\n");
		OutputDebugString(L"Generated: ");
		OutputDebugString(buffer);
		OutputDebugString(L" walls\n\n");
	#endif

//Initialize the box object for the walls
	box.init(md3dDevice, 1.0f);

	for(int i = 0; i < wallCount; ++i) {
		walls[i].init(&box, mfxWVPVar, fx2, sqrt(2.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0), 0,
				D3DXVECTOR3(mazeNS::WALL_THICK, mazeNS::WALL_HEIGHT, mazeNS::CELL_WIDTH));
	}

//Allocate the maze resolution matrix
	solved = new int*[dim.x];

	for(int i = 0; i < dim.x; ++i) {
		solved[i] = new int[dim.z];

		for(int j = 0; j < dim.z; ++j) {
			solved[i][j] = 0;
		}
	}

	alreadyInit = true;
}

Location Maze::pxToCell(Location px) {
//Ensure this request is not out of bounds
	if (px.x >= size.x || px.z >= size.z || px.x <= 0 || px.z <= 0) {
		Location loc = {-1, -1};
		return loc;
	}

//Calculate the cell position
	Location loc;
	loc.x = floor(static_cast<double>(px.x / (2 * mazeNS::CELL_LENGTH)));
	loc.z = floor(static_cast<double>(px.z / (2 * mazeNS::CELL_WIDTH)));

	return loc;
}

void Maze::setCeilingVisibility(bool visible) {
	if (visible) {
		walls[wallsConstructed - 1].setActive();
	} else {
		walls[wallsConstructed - 1].setInActive();
	}
}

void Maze::setCeilTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture) {
	walls[wallsConstructed - 1].setTex(diffuseTexture, specTexture);
}

void Maze::setFloorTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture) {
	walls[wallsConstructed - 2].setTex(diffuseTexture, specTexture);
}

void Maze::setStartPosition(Location location) {
	start = location;
	startSet = true;
}

void Maze::setTex(ID3D10ShaderResourceView* diffuseTexture, ID3D10ShaderResourceView* specTexture) {
	for(int i = 0; i < wallsConstructed; ++i) {
		walls[i].setTex(diffuseTexture, specTexture);
	}
}

void Maze::setTexLocVariable(ID3D10EffectShaderResourceVariable* diffuseLoc, ID3D10EffectShaderResourceVariable* specLoc) {
	for(int i = 0; i < wallsConstructed; ++i) {
		walls[i].setTexLocVariable(diffuseLoc, specLoc);
	}
}

Solution Maze::solve() {
	return solve(start, end);
}

Solution Maze::solve(Location from) {
	return solve(from, end);
}

Solution Maze::solve(Location from, Location to) {
	Node *cEndNode, *cStartNode, *endNode = grid[to.x][to.z], *startNode = grid[from.x][from.z];
	Solution *cSolEnd, *cSolStart, *pSolEnd = NULL, *pSolStart = NULL, solEnd, solStart;
	Backtrack backtrack = START;

//Initialize the values
	cEndNode = endNode;
	cStartNode = startNode;

	solEnd = endNode;
	solEnd.end = true;
	solStart = startNode;
	solStart.start = true;

	cSolEnd = &solEnd;

	if (cEndNode->children.toRoot != NULL) {
		cSolEnd->next = new Solution();
		pSolEnd = cSolEnd;
		cSolEnd = cSolEnd->next;
	} else {
		pSolEnd = cSolEnd;
	}

	cSolStart = &solStart;

	if (cStartNode->children.toRoot != NULL) {
		cSolStart->next = new Solution();
		pSolStart = cSolStart;
		cSolStart = cSolStart->next;
	} else {
		pSolStart = cSolStart;
	}

//Are these the same? We're home free!
	if (startNode == endNode) {
		return solStart;
	}

//Set the corresponding values in the maze resolution matrix
	solved[cStartNode->location.x][cStartNode->location.z] = solvedCounter;
	solved[cEndNode->location.x][cEndNode->location.z] = solvedCounter;

//Loop until a meeting point between the start and end paths is achieved
	do {
	//Set up the current and previous cell pointers
		if (cStartNode->children.toRoot != NULL) {
			cStartNode = cStartNode->children.toRoot;

		//Have we visited this location before?
			if (solved[cStartNode->location.x][cStartNode->location.z] == solvedCounter) {
			//Form another node one last time
				cSolStart->next = new Solution();
				*cSolStart = cStartNode;
				cSolEnd = pSolEnd;
				
				if (pSolStart != NULL) cSolStart->previous = pSolStart;

			//Which path should do the backtracking?
				backtrack = END;

				break;
			}

		//Add a new link to the start-side of the solution list, pointing one cell closer to the root
			cSolStart->next = new Solution();
			*cSolStart = cStartNode;

			if (pSolStart != NULL) cSolStart->previous = pSolStart;
			pSolStart = cSolStart;
			cSolStart = cSolStart->next;

		//Set the corresponding values in the maze resolution matrix
			solved[cStartNode->location.x][cStartNode->location.z] = solvedCounter;	
		}

		if (cEndNode->children.toRoot != NULL) {
			cEndNode = cEndNode->children.toRoot;

		//Have we visited this location before?
			if (solved[cEndNode->location.x][cEndNode->location.z] == solvedCounter) {
			//Form another node one last time
				cSolEnd->next = new Solution();
				*cSolEnd = cEndNode;
				cSolStart = pSolStart;
				
				if (pSolEnd != NULL) cSolEnd->previous = pSolEnd;

			//Which path should do the backtracking?
				backtrack = START;

				break;
			}

		//Add a new link to the end-side of the solution list, pointing one cell closer to the root
			cSolEnd->next = new Solution();
			*cSolEnd = cEndNode;

			if (pSolEnd != NULL) cSolEnd->previous = pSolEnd;
			pSolEnd = cSolEnd;
			cSolEnd = cSolEnd->next;

		//Set the corresponding values in the maze resolution matrix
			solved[cEndNode->location.x][cEndNode->location.z] = solvedCounter;
		}
	} while(true);

//Backtrack to the meeting point between the two paths
	Solution *cBackPointer, *compare, *delBackPointer;
	Solution *stash; // All the links which will be deleted

//The START path reached the END point on its own
	if (cSolStart->location == to) {
		solStart.previous = new Solution();
//The END path reached the START point on its own
	} else if (cSolEnd->location == from) {
		cSolStart = &solStart;
		pSolEnd = new Solution();

	//Just reverse the direction of the linked list!
		do { 
			cSolStart->next = cSolEnd->previous;
			cSolStart->previous = pSolEnd;

		//Advance the pointers
			pSolEnd = cSolEnd;
			cSolEnd = cSolStart->next;
			cSolStart = cSolStart->next;
		} while(cSolEnd->previous != NULL);

	//The algorithm above will cause the last two links to point back to each other endlessly, just cap the last link
		cSolStart->next = new Solution();
		cSolStart->previous = pSolEnd;
//Both the start and end points were crawling to the root
	} else {
	//Did these paths meet at the same point?
		if (cSolEnd->location == cSolStart->location) {
			cBackPointer = cSolEnd;

		//Join the END path onto the START path
			cBackPointer = cBackPointer->previous;
			cSolStart->next = cBackPointer;
			delete cBackPointer->next; // We've abandoned a node, delete it
			cBackPointer->next = cBackPointer->previous;
			cBackPointer->previous = cSolStart;

		//Now, reverse the direction of the linked list which was just joined onto START
			while(cBackPointer != NULL && cBackPointer->next != NULL) {
				cSolStart = cSolStart->next;
				cBackPointer = cBackPointer->next;
				cBackPointer->next = cBackPointer->previous;
				cBackPointer->previous = cSolStart;
			}

		//Cap the end of the linked list
			cBackPointer->next = new Solution();
	//The START path progressed farther, so backtrack START to the meeting point
		} else if (backtrack == START) {
			compare = cSolEnd;
			delBackPointer = cSolStart;

			do {
			//Move back the linked list
				cBackPointer = delBackPointer->previous;

			//This node is no longer needed
				stash = delBackPointer;

			//Get ready to move back again
				delBackPointer = cBackPointer;
			} while (cBackPointer != NULL && cBackPointer->location != compare->location);

		//Join the END path onto the START path
			cSolEnd = cSolEnd->previous;
			cBackPointer->next = cSolEnd;
			delete cSolEnd->next; // We've abandoned a node, delete it
			cSolEnd->next = cSolEnd->previous;
			cSolEnd->previous = cBackPointer;

		//Now, reverse the direction of the linked list which was just joined onto START
			while(cSolEnd->next != NULL) {
				cBackPointer = cBackPointer->next;
				cSolEnd = cBackPointer->next;
				cSolEnd->next = cSolEnd->previous;
				cSolEnd->previous = cBackPointer;
			}

		//Cap the end of the linked list
			cSolEnd->next = new Solution();
	//The END path progressed farther, so backtrack END to the meeting point
		} else if (backtrack == END) {
			compare = cSolStart;
			delBackPointer = cSolEnd;

			do {
			//Move back the linked list
				cBackPointer = delBackPointer->previous;

			//This node is no longer needed
				stash = delBackPointer;

			//Get ready to move back again
				delBackPointer = cBackPointer;
			} while (cBackPointer->location != compare->location);

		//Join the END path onto the START path
			cBackPointer = cBackPointer->previous;
			cSolStart->next = cBackPointer;
			delete cBackPointer->next; // We've abandoned a node, delete it
			cBackPointer->next = cBackPointer->previous;
			cBackPointer->previous = cSolStart;

		//Now, reverse the direction of the linked list which was just joined onto START
			while(cBackPointer->next != NULL) {
				cSolStart = cSolStart->next;
				cBackPointer = cBackPointer->next;
				cBackPointer->next = cBackPointer->previous;
				cBackPointer->previous = cSolStart;
			}

		//Cap the end of the linked list
			cBackPointer->next = new Solution();
		}

	//Destroy the nodes on the stash
		delete stash;
	}

//Fix strange edge case where the last node is invalid
	bool boomGotcha = false;
	Location boomTo;
	Solution *cur = &solStart;
	Node *endSauce = grid[to.x][to.z];

	while(true) {
		if (endSauce->children.east != NULL && endSauce->children.east->location == cur->location) {
			boomGotcha = true;
			boomTo = endSauce->location;
		}

		if (endSauce->children.north != NULL && endSauce->children.north->location == cur->location) {
			boomGotcha = true;
			boomTo = endSauce->location;
		}

		if (endSauce->children.south != NULL && endSauce->children.south->location == cur->location) {
			boomGotcha = true;
			boomTo = endSauce->location;
		}

		if (endSauce->children.west != NULL && endSauce->children.west->location == cur->location) {
			boomGotcha = true;
			boomTo = endSauce->location;
		}

		if (boomGotcha) {
			cur->next = new Solution();
			cur->next->end = true;
			cur->next->location = boomTo;
			cur->next->next = new Solution();
			cur->next->previous = cur;
			cur->next->start = false;

			cur->end = false;
			cur->start = false;

			break;
		}

		cur = cur->next;
	}

	//while(true) {
	////This is an indication of the problem
	//	if (cur->location.x < -1 || cur->location.z < -1  ||
	//		(cur->end == true && cur->start == true)) {
	//			prev->next = new Solution();
	//			prev->next->end = true;
	//			prev->next->location = this->end;
	//			prev->next->next = new Solution();
	//			prev->next->previous = prev;
	//			prev->next->start = false;

	//			break;
	//	}

	////This is an indication of the end
	//	if (cur->end) {
	//		break;
	//	}

	//	prev = cur;
	//	cur = cur->next;
	//}

	++solvedCounter;
	return solStart;
}

void Maze::update(float dt) {
	for(int i = 0; i < wallsConstructed; ++i) {
		walls[i].update(dt);
	}
}