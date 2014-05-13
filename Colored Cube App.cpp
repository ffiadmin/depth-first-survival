//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
//=============================================================================

//update postion
//update world matrix of where to draw
//multiple world matrix tell it to draw

#include <math.h>
#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Billboard.h"
#include "flashLightObject.h"
#include "BatteryObject.h"
#include "Line.h"
#include "Quad.h"
#include "Origin.h"
#include "audio.h"
#include <d3dx9math.h>
#include "gameError.h"
#include "input.h"
#include <sstream>
#include "LineObject.h"
#include "SoundItem.h"
#include "Mesh.h"
#include "playerControls.h"
#include "EnemyHoard.h"
#include "LightObject.h"
#include "Maze.h"
#include "TrackingEnemies.h"
#include "Monster.h"

class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	void updateGameState();
	void reloadLevel(int x, int z, bool keys = false);

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	void updateL1(float dt);
	void updateL2(float dt);
	void updateL3(float dt);
	void updateL4(float dt);
	void updateSplashScreen(float dt,ID3D10ShaderResourceView* screen);
	void updateTransition(float dt);
	void updatePerspectiveSwitch(float dt);
	void updateLights();
	void drawTitle();
	void drawControls();
	void drawL1();
	void drawL2();
	void drawLose();
	void drawWin();

	void dropBread(Vector3 pos);
	void shootProjectile(Vector3 pos);

	ID3D10BlendState* mTransparentBS;

	float sceneAlpha;
	Gamestates gamestate;
	Gamestates nextState;
	Gamestates transitionState;
	Audio* audio;
	playerControls camera;
	Light lights[10];
	Light* keyLights[10];
	Mesh pokey;
	int lightsGoingInShader;
	float topDownTime;
	float maxTopDownTime;
	Light* totalLights[100];
	float score;
	Quad quad1;
	Line line, line2, line3;
	Box mBox;
	Monster mon;
	Billboard testBoard;
	Maze maze;
	bool perspective;

	int breadNumber;
	int projectileNum;
	int maxBread;
	int maxProjectile;

	bool goal;
	int currentKeys, totalKeys;

	D3DXCOLOR ambientLight;

	Mesh key;
	GameObject keyObject[10];

	GameObject billboard;
	GameObject player;
	GameObject endCube;
	Light endLight;
	Light ambientLighting;

	TrackingEnemies testTracker;
	TrackingEnemies testTracker2;
	TrackingEnemies dungeonMaster;

	EnemyHoard ghosts;
	FlashLightObject flashLightObject;
	BatteryObject batteries[20];
	LightObject lamps[30];

	GameObject breadCrumbs[30];

	GameObject projectile[10];

	int numLightObjects;
	int numBatteries;

	int l3Ghosts;
	int ghostsKilled;

	Origin origin;
	int oldBLevel;
	float spinAmount;
	int prevLightType;//used for switching light types
	//variable used to keep track of the lights added to the master array so that indexing is kept correctly
	int lightsAdded;
	int mazeX;
	int mazeZ;

	float timeBetweenTopDown;
	
	//Debounce checkers
	bool perspectiveDebounced;
	bool breadDebounced;
	bool projectileDebounced;
	bool splashScreenDebounced;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectTechnique* mTechColor2;
	ID3D10EffectTechnique* mTechBillboard;
	ID3D10EffectTechnique* mTechFogColor;
	ID3D10EffectTechnique* mTechFogCeil;
	ID3D10EffectTechnique* mTechEmmisive;
	ID3D10EffectTechnique* mTechFog;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	//my addition
	//the location of variable in the shader program like in openGL
	ID3D10EffectVariable* mfxFLIPVar;

	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxAlpha;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;
	ID3D10EffectMatrixVariable* mfxProjMatrix;
	ID3D10EffectScalarVariable* mfxNumLights;

	//textures and spec maps
	ID3D10ShaderResourceView* brickTexture;
	ID3D10ShaderResourceView* boxTexture;
	ID3D10ShaderResourceView* titleTexture;
	ID3D10ShaderResourceView* controlTexture;
	ID3D10ShaderResourceView* winScreenTexture;
	ID3D10ShaderResourceView* loseScreenTexture;
	ID3D10ShaderResourceView* floorTexture;
	ID3D10ShaderResourceView* ceilingTexture;
	ID3D10ShaderResourceView* l1Splash;
	ID3D10ShaderResourceView* l2Splash;
	ID3D10ShaderResourceView* l3Splash;
	ID3D10ShaderResourceView* l4Splash;
	ID3D10ShaderResourceView* projectileTexture;

	ID3D10ShaderResourceView* standardSpecMap;
	ID3D10ShaderResourceView* brickSpecMap;
	ID3D10ShaderResourceView* iceSpecMap;

	ID3D10ShaderResourceView* splashScreen;

	//Camera Variables
	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
	D3DXVECTOR3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;
	float sensitivity;

	float hitTimer;

	//Fog Switch
	bool fog;

	int guardsHit;

    float timer;
    std::wstring mTimer;
	HRESULT hr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif

	ColoredCubeApp theApp(hInstance);
	theApp.initApp();
	return theApp.run();
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.4f), player(), mRadius(5000), mEyePos(0.0f, 0.0f, 0.0f), ghosts(3,10,50,50)
{
	for(int i = 0; i < 100; i++)
	{
		totalLights[i] = 0;
	}
	hitTimer = 0;
	guardsHit = 0;
	maxTopDownTime = 10;
	topDownTime = 0;
	breadDebounced = false;
	projectileDebounced = false;
	perspectiveDebounced = false;
	splashScreenDebounced = false;
	goal = false;
	currentKeys = 0;
	totalKeys = 5;
	numLightObjects = 7;
	numBatteries = 10;
	player.setHealth(10);
	lightsGoingInShader = 10;
	prevLightType = 0;
	score = 0;
	timer = 0;
	lightsAdded = 0;
	l3Ghosts = 10;
	ghostsKilled = 0;
	perspective = false;
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
	oldBLevel = 0;
	gamestate = title;
	sceneAlpha = 1.0f;
	timeBetweenTopDown = 20;
	fog = false;
	sensitivity = 2.0f;
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	//delete audio;
	//delete input;

	brickTexture->Release();
	boxTexture->Release();
	titleTexture->Release();
	controlTexture->Release();
	winScreenTexture->Release();
	loseScreenTexture->Release();
	floorTexture->Release();
	ceilingTexture->Release();

	standardSpecMap->Release();
	brickSpecMap->Release();
	iceSpecMap->Release();


	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mTransparentBS);
	//delete testSound;
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	//create transparent blend state
	D3D10_BLEND_DESC blendDesc = {0};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.BlendEnable[0] = true;
	blendDesc.SrcBlend       = D3D10_BLEND_SRC_ALPHA;
	blendDesc.DestBlend      = D3D10_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp        = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha  = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha   = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	HR(md3dDevice->CreateBlendState(&blendDesc, &mTransparentBS));

	//set up textures and spec maps
	//brick texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"brickwork-texture.jpg", 0, 0, &brickTexture, 0 ));
	//box texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"WoodCrate01.dds", 0, 0, &boxTexture, 0 ));
	//title screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"Title Screen.jpg", 0, 0, &titleTexture, 0 ));
	//control screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"Rules Screen.jpg", 0, 0, &controlTexture, 0 ));
	//win screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"You Win.jpg", 0, 0, &winScreenTexture, 0 ));
	//lose screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"You Lose.jpg", 0, 0, &loseScreenTexture, 0 ));
	//floor texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"carpet_diffuse.jpg", 0, 0, &floorTexture, 0 ));
	//ceiling texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"13.free-brick-textures.jpg", 0, 0, &ceilingTexture, 0 ));
	//level 1 splash
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"l1Splash.jpg", 0, 0, &l1Splash, 0 ));
	//level 2 splash
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"l2Splash.jpg", 0, 0, &l2Splash, 0 ));
	//level 3 splash
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"l3Splash.jpg", 0, 0, &l3Splash, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"l4Splash.jpg", 0, 0, &l4Splash, 0 ));
	//Projectile Texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"projectile.jpg", 0, 0, &projectileTexture, 0 ));

	//standard spec map
	standardSpecMap;
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"13.free-brick-textures.jpg", 0, 0, &standardSpecMap, 0 ));
	//brick spec map
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"brickwork-bump-map.jpg", 0, 0, &brickSpecMap, 0 ));
	//ice spec map
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"ice.dds", 0, 0, &iceSpecMap, 0 ));

	//0: Parallel
	//1: Point
	//2: Spot

	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);
	//ambientLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	ambientLighting.ambient = ambientLight;
	ambientLighting.diffuse  = D3DXCOLOR(0.0f, 0.02f, 0.02f, 1.0f);
	ambientLighting.specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	ambientLighting.att.x    = 1.0f;
	ambientLighting.att.y    = 0.0f;
	ambientLighting.att.z    = 0.0f;
	ambientLighting.spotPow  = 20;
	ambientLighting.range    = 100;
	ambientLighting.pos = D3DXVECTOR3(-10,20,-10);
	ambientLighting.dir = D3DXVECTOR3(0, -1, 0);	
	ambientLighting.lightType.x = 0;

	buildFX();
	buildVertexLayouts();

	pokey.init(md3dDevice,1,"pokey.txt");

	//Build Maze
	Dimension d;
	d.x = 10;
	d.z = 10;
	mazeX = d.x;
	mazeZ = d.z;
	maze.init(d,mfxWVPVar,mfxWorldVar,md3dDevice);
	maze.build();
	Solution sol = maze.solve();
	maze.setTex(brickTexture,brickSpecMap);	
	maze.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	maze.setCeilTex(ceilingTexture,brickSpecMap);
	maze.setFloorTex(floorTexture,brickSpecMap);
	camera.init(&maze, input);

	mBox.init(md3dDevice, 1.0f);
	mon.init(md3dDevice, 1.0f);

	testBoard.init(md3dDevice,Vector3(0,0,0),Vector2(24,24));
	billboard.init(&testBoard,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(1,1,1));
	billboard.setProjection(mfxProjMatrix);
	billboard.setTex(boxTexture,standardSpecMap);

	//Projectile and Bread values
	breadNumber = 0;
	maxBread = 30;
	projectileNum = 10;
	maxProjectile = 10;

	line.init(md3dDevice, 1.0f, DARKBROWN);
	line2.init(md3dDevice, 1.0f, RED);
	line3.init(md3dDevice, 1.0f, GREEN);

	origin.init(&line,&line2,&line3,mfxWVPVar,10);

	quad1.init(md3dDevice,1,D3DXCOLOR(0.5,0.25,0.1,1.0));

	//Initialize Player
	player.init(&mBox,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(10,0,10),Vector3(0,0,0),0,Vector3(2,2,2));
	player.setTex(boxTexture,standardSpecMap);
	player.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	Location start = maze.getStartPosition();
	start = maze.cellToPx(start);
	player.setPosition(Vector3(start.x,0,start.z));

	testTracker.init(&mon,mfxWVPVar,mfxWorldVar,2*sqrt(2.0f),player.getPosition(),Vector3(0,0,0),10,Vector3(1,1,1));
	testTracker2.init(&mon,mfxWVPVar,mfxWorldVar,2*sqrt(2.0f),player.getPosition(),Vector3(0,0,0),10,Vector3(1,1,1));

	dungeonMaster.init(&pokey,mfxWVPVar,mfxWorldVar,2*sqrt(2.0f),Vector3(0,0,0),Vector3(0,0,0),20,Vector3(1,1,1));

	billboard.setPosition(Vector3(start.x,0,start.z));

	flashLightObject.init(md3dDevice,mfxWVPVar,mfxWorldVar,2,Vector3(0,0,0),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
	flashLightObject.setRotation(Vector3(ToRadian(90),0,0));

	//Intialize Lamp Lights
	for(int i = 0; i < numLightObjects; i++)
	{
		lamps[i].init(md3dDevice,mfxWVPVar,mfxWorldVar,2,Vector3(10,3,0),Vector3(0,0,0),0,Vector3(0.75,0.75,0.75));
		lamps[i].setTex(boxTexture,iceSpecMap);
		lamps[i].setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		lamps[i].setPosition(Vector3(spot.x,5,spot.z));
		lamps[i].setColor(D3DXCOLOR(0.2f, 0.5f, 0.3f, 1.0f));
		totalLights[lightsAdded] = lamps[i].getLight();
		lightsAdded++;
	}

	//Initialize Keys
	key.init(md3dDevice,1,"item1.txt");
	for(int i = 0; i < totalKeys; i++)
	{
		keyObject[i].init(&key,mfxWVPVar,mfxWorldVar,sqrt(3.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);

		keyObject[i].setPosition(Vector3(spot.x,-1,spot.z));
		keyLights[i] = new Light();
		keyLights[i]->ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		keyLights[i]->diffuse  = D3DXCOLOR(0.2f, 0.1f, 0.5f, 1.0f);
		keyLights[i]->specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		keyLights[i]->att.x    = 1.0f;
		keyLights[i]->att.y    = 0.0f;
		keyLights[i]->att.z    = 0.0f;
		keyLights[i]->spotPow  = 10;
		keyLights[i]->range    = 100;
		keyLights[i]->pos = D3DXVECTOR3(spot.x,5,spot.z);
		keyLights[i]->dir = D3DXVECTOR3(0, -1, 0);	
		keyLights[i]->lightType.x = 2;
		totalLights[lightsAdded] = keyLights[i];
		lightsAdded++;
	}

	//Initialize End Cube
	endCube.init(&mBox,mfxWVPVar,mfxWorldVar,sqrt(3.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(2,2,2));
	endCube.setTex(boxTexture,iceSpecMap);
	endCube.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	Location end = maze.getEndPosition();
	end = maze.cellToPx(end);
	endCube.setPosition(Vector3(end.x,0,end.z));

	//set up the end light
	endLight.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	endLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 0.3f, 1.0f);
	endLight.specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	endLight.att.x    = 1.0f;
	endLight.att.y    = 0.0f;
	endLight.att.z    = 0.0f;
	endLight.spotPow  = 10;
	endLight.range    = 100;
	endLight.pos = D3DXVECTOR3(end.x,10,end.z);
	endLight.dir = D3DXVECTOR3(0, -1, 0);	
	endLight.lightType.x = 2;

	totalLights[lightsAdded] = &endLight;
	lightsAdded++;

	//Initialize Batteries
	for(int i = 0; i < numBatteries; i++)
	{
		batteries[i].init(md3dDevice,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(0,0,5),Vector3(0,0,0),0,Vector3(0.5,0.5,0.5));
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		batteries[i].setPosition(Vector3(spot.x,-1,spot.z));
	}

	//Initialize Ghosts
	ghosts.init(md3dDevice,mfxWVPVar,mfxWorldVar,4*sqrt(2.0f),Vector3(5,0,0),Vector3(0,0,0),10,Vector3(1,1,1));
	ghosts.setBlend(mTransparentBS);
	ghosts.setTex(brickTexture,iceSpecMap);
	ghosts.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		totalLights[lightsAdded] = ghosts.getEnemies()[i].getLight();
		lightsAdded++;
	}

	//Initialize Bread
	for(int i = 0; i < maxBread; i++)
	{
		breadCrumbs[i].init(&mBox,mfxWVPVar,mfxWorldVar,sqrt(3.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(2,2,2));
		breadCrumbs[i].setInActive();
		breadCrumbs[i].setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
		breadCrumbs[i].setTex(ceilingTexture,iceSpecMap);
	}

	//Initialize Projectiles
	for(int i = 0; i < maxProjectile; i++)
	{
		projectile[i].init(&mBox,mfxWVPVar,mfxWorldVar,4*sqrt(2.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(1,1,1));
		projectile[i].setInActive();
		projectile[i].setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
		projectile[i].setTex(projectileTexture,iceSpecMap);
	}

	// init sound system
    audio = new Audio();
    if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
    {
        if( FAILED( hr = audio->initialize() ) )
        {
			exit(1);
            //if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
            //    throw(GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system because media file not found."));
            //else
            //    throw(GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system."));
        }
    }
	//Set up the camera
	camera.setHeightAndWidth(mClientWidth,mClientHeight);

	audio->playCue(MUSIC);
}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();
	//float aspect = (float)mClientWidth/mClientHeight;
	//D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
	camera.resize(mProj,0.33f*PI,1.0f,1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	ambientLighting.ambient = ambientLight;
	updateGameState();
	endCube.update(dt);

	billboard.update(dt);

	//player updates
	//sceneAlpha-=dt/100;
	//detect battery level
	//maybe give gameObjects an audio pointer so they can play cues when necessary...
	if(oldBLevel!=0 && flashLightObject.getPowerLevel()<=0)
	{
		audio->playCue(BATTERY_DIE);
	}

	if(!perspective && topDownTime != maxTopDownTime)
	{
		timer+=dt;
	}

	if(timer>=timeBetweenTopDown)
	{
		timer = 0;
		topDownTime = maxTopDownTime;
	}

	oldBLevel = flashLightObject.getPowerLevel();
	auto oldP = player.getPosition();
	//timer -= dt;

	//update the camera
	camera.update(mTheta,mPhi,mRadius,0,dt,player,mView,mEyePos,perspective,true,sensitivity);
	//move the player
	camera.movePlayer(player,30,camera.getTarget(),perspective);
	player.update(dt);
	Location playerLoc;
	playerLoc.x = player.getPosition().x;
	playerLoc.z = player.getPosition().z;
	//Player collision detection
	if(player.getPosition()!=oldP)
	{
		if(maze.collided(playerLoc))
		{
			player.setPosition(oldP);
			player.setVelocity(Vector3(0,0,0));
			player.update(dt);
		}
	}
	flashLightObject.update(dt);
	
	maze.update(dt);
	
	if(flashLightObject.getPosition()!=(player.getPosition()+(camera.getTarget()*5)))
	{
		flashLightObject.setPosition(player.getPosition() + camera.getTarget()*5);
		int i = 0;
	}

	//orientating the flashlight
	if(flashLightObject.lightSource.dir!=camera.getTarget())
	{
		//vectors for caluclating z-rotation
		Vector2 cameraXY = Vector2(camera.getTarget().x,camera.getTarget().y);
		Vector2 startXY = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.y);
		//vectors for calculating y-rotation
		Vector2 cameraXZ = Vector2(camera.getTarget().x,camera.getTarget().z);
		Vector2 startXZ = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.z);
		//vectors for calculating x-rotation
		Vector2 cameraYZ = Vector2(camera.getTarget().y,camera.getTarget().z);
		Vector2 startYZ = Vector2(flashLightObject.lightSource.dir.y,flashLightObject.lightSource.dir.z);

		float xAngle = flashLightObject.getRotation().x;
		float yAngle = flashLightObject.getRotation().y;
		float zAngle = flashLightObject.getRotation().z;
		float topEquation;
		float bottomEquation;

		topEquation = Dot2(&cameraXY,&startXY);
		bottomEquation = Length2(&cameraXY)*Length2(&startXY);
		if(bottomEquation>0)
		{
			zAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraXZ,&startXZ);
		bottomEquation = Length2(&cameraXZ)*Length2(&startXZ);
		if(bottomEquation>0)
		{
			yAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraYZ,&startYZ);
		bottomEquation = Length2(&cameraYZ)*Length2(&startYZ);
		if(bottomEquation>0)
		{
			xAngle+=acos((topEquation/bottomEquation));
		}

		flashLightObject.setRotation(Vector3(xAngle,yAngle,zAngle));
		flashLightObject.lightSource.dir = camera.getTarget();
	}

	//Sensitivity controls
	if(GetAsyncKeyState('1') & 0x8000)
	{
		sensitivity = 0.5f;
	}
	if(GetAsyncKeyState('2') & 0x8000)
	{
		sensitivity = 1.0f;
	}
	if(GetAsyncKeyState('3') & 0x8000)
	{
		sensitivity = 2.0f;
	}
	if(GetAsyncKeyState('4') & 0x8000)
	{
		sensitivity = 2.5f;
	}
	if(GetAsyncKeyState('5') & 0x8000)
	{
		sensitivity = 3.0f;
	}

	//Leave bread
	if(!breadDebounced && GetAsyncKeyState('C') & 0x8000)
	{
		dropBread(player.getPosition());
		breadDebounced = true;
	}
	if(!(GetAsyncKeyState('C') & 0x8000))
	{
		breadDebounced = false;
	}

	//Shoot projectile
	if(gamestate == level3 || gamestate == level4)
	{
		if(!projectileDebounced && GetAsyncKeyState('G') & 0x8000)
		{
			shootProjectile(player.getPosition());
			projectileDebounced = true;
		}
		if(!(GetAsyncKeyState('G') & 0x8000))
		{
			projectileDebounced = false;
		}
	}

	//Change perspective
	if(gamestate == level1 || gamestate == level2 || gamestate == level3 || gamestate == level4)
	{
		if(!perspective && !perspectiveDebounced && GetAsyncKeyState('R') & 0x8000)
		{
			perspective = !perspective;
			perspectiveDebounced = true;
			nextState = gamestate;
			gamestate = viewSwitch;
			//ambientLighting.ambient = D3DXCOLOR(1,0.5,0.5,1);
		}
		if(!(GetAsyncKeyState('R') & 0x8000))
		{
			perspectiveDebounced = false;
			//ambientLighting.ambient = ambientLight;
		}
	}

	if(perspective)
	{
		topDownTime -= dt;
	}

	if(topDownTime <= 0)
	{
		perspective = false;
	}

	//Update Lamp Lights
	for(int i = 0; i < numLightObjects; i++)
	{
		lamps[i].update(dt);
	}

	//Update Batteries
	for(int i = 0; i < numBatteries; i++)
	{
		batteries[i].update(dt);
		if(player.collided(&batteries[i]))
		{
			batteries[i].setInActive();
			flashLightObject.getBattery();
			audio->playCue(BATTERY_CHARGE);
			if(gamestate == level3 || gamestate == level4)
			{
				projectileNum--;
				if(projectileNum < 0)
					projectileNum = 0;	
			}
		}
	}

	//Update Bread
	for(int i = 0; i < maxBread; i++)
	{
		if(breadCrumbs[i].getActiveState())
			breadCrumbs[i].update(dt);
	}

	//Update Projectiles
	for(int i = 0; i < maxProjectile; i++)
	{
		if(projectile[i].getActiveState())
		{
			auto oldP = projectile[i].getPosition();
			projectile[i].update(dt);
			Location playerLoc;
			playerLoc.x = projectile[i].getPosition().x;
			playerLoc.z = projectile[i].getPosition().z;
			//Collision detection
			if(projectile[i].getPosition()!=oldP)
			{
				if(maze.collided(playerLoc))
				{
					projectile[i].setPosition(oldP);
					projectile[i].setVelocity(Vector3(0,0,0));
					projectile[i].setInActive();
					projectile[i].update(dt);
				}
			}
		}
	}
	//Game state switching
	switch(gamestate)
	{
	case title:
		updateSplashScreen(dt,titleTexture);
		lights[0] = ambientLighting;
		break;
	case controls:
		updateSplashScreen(dt,controlTexture);
		lights[0] = ambientLighting;
		break;
	case level1:
		updateLights();
		updateL1(dt);
		break;
	case level2:
		updateLights();
		updateL2(dt);
		break;
	case level3:
		updateLights();
		updateL3(dt);
		break;
	case level4:
		updateLights();
		updateL4(dt);
		break;
	case gameover:
		updateSplashScreen(dt,loseScreenTexture);
		lights[0] = ambientLighting;
		break;
	case win:
		updateSplashScreen(dt,winScreenTexture);
		lights[0] = ambientLighting;
		break;
	case splash:
		updateSplashScreen(dt,splashScreen);
		lights[0] = ambientLighting;
	case transition:
		updateTransition(dt);
		lights[1] = ambientLighting;
		break;
	case viewSwitch:
		updatePerspectiveSwitch(dt);
		lights[1] = ambientLighting;
		break;
	}
}

void ColoredCubeApp::updateTransition(float dt)
{
	sceneAlpha-=dt/5;
	if(sceneAlpha<=0)
	{
		gamestate = transitionState;
		sceneAlpha = 0.01f;
	}
}

void ColoredCubeApp::updateLights()
{
	int lightRadius = 50;
	//put in default lights
	int lightIndex = 0;
	if(flashLightObject.getOn())
	{
		lights[lightIndex] = flashLightObject.getLight();
		lightIndex++;
	}

	lights[lightIndex] = ambientLighting;
	lightIndex++;
	Light addingLights[8];
	//calculate the distance of each light from the player
	double distances[100];
	bool added[100];
	for(int i = 0; i < 100; i++)
	{
		added[i] = false;
	}
	int n = 0;
	for(int i = 0; i < lightsAdded; i++)
	{
		Vector3 diff = player.getPosition()-totalLights[i]->pos;
		double distance = sqrtf(abs(Dot(&diff,&diff)));
		distances[i] = distance;
		/*if(distances[i]<lightRadius)
		{
			addingLights[n] = *totalLights[n];
			n++;
		}
		if(n>=8)
		{
			break;
		}*/
	}
	for(int i = lightIndex; i < 10; i++)
	{
		//lights[i] = addingLights[i-2];
		for(int j = 0; j < lightsAdded; j++)
		{
			if(!added[j] && distances[j] < lightRadius)
			{
				added[j] = true;
				lights[i] = *totalLights[j];
				break;
			}
		}
	}
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();

	ID3D10EffectTechnique* colorTech;
	ID3D10EffectTechnique* texTech;
	if(fog)
	{
		colorTech = mTechFogColor;
		texTech = mTechFog;
	}
	else
	{
		colorTech = mTechColor2;
		texTech = mTech;
	}

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

    md3dDevice->IASetInputLayout(mVertexLayout);

	mfxEyePosVar->SetRawValue(&player.getPosition(), 0, sizeof(D3DXVECTOR3));

	if(gamestate==transition || gamestate == splash)
	{
		md3dDevice->OMSetBlendState(mTransparentBS, blendFactors, 0xffffffff);
		mfxAlpha->SetFloat(sceneAlpha);		
	}
	else
	{
		//set alpha value for ghosts
		mfxAlpha->SetFloat(0.4f);
		md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
	}

	//set the number of lights to use
	mfxNumLights->SetInt(lightsGoingInShader);

	//throw lights at shader boooom!!!!!!
	mfxLightVar->SetRawValue(&lights[0], 0, lightsGoingInShader*sizeof(Light));
 
	// Don't transform texture coordinates, so just use identity transformation.
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	if(gamestate == level3)
	{
		testTracker.draw(mView,mProj,mTechColor2);
		testTracker2.draw(mView,mProj,mTechColor2);
	}

	//draw the maze
	if(fog)
		maze.draw(texTech,mView,mProj,mTechFogCeil);
	else
		maze.draw(texTech,mView,mProj);

	//draw the keys
	if(gamestate == level1)
	{
		for(int i = 0; i < totalKeys; i++)
		{
			keyObject[i].draw(mView,mProj,texTech);
		}
	}

	//draw the end cube
	if(gamestate == level4)
	{
		endCube.draw(mView,mProj,texTech);
		dungeonMaster.draw(mView,mProj,mTechColor2);
	}

	//draw the origin
	//origin.draw(mView, mProj, mTech);
	
	if(gamestate == level3)
	{
		testTracker.draw(mView,mProj,mTechColor2);
	}

	//draw "crumbs" placed by player
	for(int i = 0; i < maxBread; i++)
	{
		if(breadCrumbs[i].getActiveState())
		{
			breadCrumbs[i].draw(mView,mProj,texTech);
		}
	}

	for(int i = 0; i < maxProjectile; i++)
	{
		if(projectile[i].getActiveState())
		{
			projectile[i].draw(mView,mProj,texTech);
		}
	}

	for(int i = 0; i < numLightObjects; i++)
	{
		lamps[i].draw(mView,mProj,texTech);
	}

	for(int i = 0; i < numBatteries; i++)
	{
		batteries[i].draw(mView,mProj,mTechEmmisive);
	}

	//flashLightObject.draw(mView,mProj,mTechColor2);
	//flashLightObject.hitBox.draw(mView,mProj,mTechColor2);
	
	player.draw(mView,mProj,texTech);

	billboard.draw(mView,mProj,mTechBillboard);

	if(gamestate == level2 || gamestate == level3 || gamestate == level4)
	{
		ghosts.draw(mView,mProj,texTech);
	}

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLUE);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::updateL1(float dt)
{
	std::wostringstream outs; 
	maze.setCeilTex(ceilingTexture,brickSpecMap);
	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);

	outs.precision(2);
	outs << L"Health: " << player.getHealth() << L"\n";
	outs.precision(3);
	outs << "Battery: " << flashLightObject.getPowerLevel();
	outs << "\nBreadcrumbs: " << maxBread-breadNumber;
	outs << "\nKeys Remaining: " << (totalKeys-currentKeys);
	if(topDownTime == maxTopDownTime)
		outs << "\nTop Down Available";
	if(perspective)
		outs << "\nTop Down Remaining: " << topDownTime;
	mTimer = outs.str();

	//testTracker.update(dt,&player,true,&maze);

	for(int i = 0; i < totalKeys; i++)
	{
		keyObject[i].update(dt);
		if(player.collided(&keyObject[i]))
		{
			currentKeys++;
			keyObject[i].setInActive();
			keyLights[i]->pos = Vector3(100,100,100);
			audio->playCue(ITEM);
		}
	}
}

void ColoredCubeApp::updateL3(float dt)
{
	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);
	std::wostringstream outs; 
	
	testTracker.update(dt,&player,false,&maze);
	testTracker2.update(dt,&player,false,&maze);

	for(int i = 0; i<maxProjectile; i++)
	{
		if(testTracker.collided(&projectile[i]))
		{
			projectile[i].setInActive();
			testTracker.setInActive();
			guardsHit++;
			audio->playCue(G_HIT);
		}
		if(testTracker2.collided(&projectile[i]))
		{
			projectile[i].setInActive();
			testTracker2.setInActive();
			guardsHit++;
			audio->playCue(G_HIT);
		}
	}

	if(guardsHit == 2)
	{
		goal = true;
	}

	if(testTracker.collided(&player))
	{
		hitTimer+=dt;
		if(hitTimer>=1)
		{
			player.setHealth(player.getHealth()-dt);
			audio->playCue(WILHELM);
			hitTimer = 0;
		}
	}
	else if(testTracker2.collided(&player))
	{
		hitTimer+=dt;
		if(hitTimer>=1)
		{
			player.setHealth(player.getHealth()-dt);
			audio->playCue(WILHELM);
			hitTimer = 0;
		}
	}
	else
	{
		hitTimer = 0;
	}

	outs.precision(2);
	outs << L"Health: " << player.getHealth() << L"\n";
	outs.precision(3);
	outs << "Battery: " << flashLightObject.getPowerLevel();
	outs << "\nBreadcrumbs: " << maxBread-breadNumber;
	outs << "\nProjectiles: " << maxProjectile-projectileNum;
	outs << "\nGuards Remaining: " << 2-guardsHit;
	if(topDownTime == maxTopDownTime)
		outs << "\nTop Down Available";
	if(perspective)
		outs << "\nTop Down Remaining: " << topDownTime;
	mTimer = outs.str();
	mTimer = outs.str();

	ghosts.update(dt,&player,camera.getTarget(),perspective);
	//ghost collision detection
	
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		//player gets hit by a ghost
		if(player.collided(&ghosts.getEnemies()[i]))
		{
			player.setHealth(player.getHealth()-1);
			//make flash take longer
			ghosts.getEnemies()[i].setInActive();
			audio->playCue(P_HIT);
		}
	}
	//flashlight ghost attack
	if(flashLightObject.getOn())
	{
		for(int i = 0; i < ghosts.getNumEnemies(); i++)
		{
			if(flashLightObject.hitTarget(&ghosts.getEnemies()[i]))
			{
				ghosts.getEnemies()[i].setHit(true);
				ghosts.getEnemies()[i].decreaseHealth();
				audio->playCue(G_HIT);
			}
			else
			{
				ghosts.getEnemies()[i].setHit(false);
			}
		}
	}
}

void ColoredCubeApp::updateL2(float dt)
{
	std::wostringstream outs;
	outs.precision(2);
	outs << L"Health: " << player.getHealth() << L"\n";
	outs.precision(3);
	outs << "Battery: " << flashLightObject.getPowerLevel();
	outs << "\nBreadcrumbs: " << maxBread-breadNumber;
	outs << "\nGhosts remaining: " << l3Ghosts - ghostsKilled;
	if(topDownTime == maxTopDownTime)
		outs << "\nTop Down Available";
	if(perspective)
		outs << "\nTop Down Remaining: " << topDownTime;
	mTimer = outs.str();
	mTimer = outs.str();

	ghosts.update(dt,&player,camera.getTarget(),perspective);
	//ghost collision detection
	
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		//player gets hit by a ghost
		if(player.collided(&ghosts.getEnemies()[i]))
		{
			player.setHealth(player.getHealth()-1);
			//make flash take longer
			ghosts.getEnemies()[i].setInActive();
			ghosts.getEnemies()[i].getLight()->pos = Vector3(100,100,100);
			audio->playCue(P_HIT);
		}
	}
	//flashlight ghost attack
	if(flashLightObject.getOn())
	{
		for(int i = 0; i < ghosts.getNumEnemies(); i++)
		{
			if(flashLightObject.hitTarget(&ghosts.getEnemies()[i]))
			{
				ghosts.getEnemies()[i].setHit(true);
				if(ghosts.getEnemies()[i].decreaseHealth())
					ghostsKilled++;
				audio->playCue(G_HIT);
			}
			else
			{
				ghosts.getEnemies()[i].setHit(false);
			}
		}
	}
	//win scenario
	if(ghostsKilled>=l3Ghosts)
	{
		goal = true;
	}
}

void ColoredCubeApp::updateL4(float dt)
{
	std::wostringstream outs;
	outs.precision(2);
	outs << L"Health: " << player.getHealth() << L"\n";
	outs.precision(3);
	outs << "Battery: " << flashLightObject.getPowerLevel();
	outs << "\nBreadcrumbs: " << maxBread-breadNumber;
	outs << "\nProjectiles: " << maxProjectile-projectileNum;
	if(topDownTime == maxTopDownTime)
		outs << "\nTop Down Available";
	if(perspective)
		outs << "\nTop Down Remaining: " << topDownTime;
	mTimer = outs.str();
	mTimer = outs.str();

	dungeonMaster.update(dt,&player,true,&maze);

	if(dungeonMaster.collided(&player))
	{
		player.setHealth(player.getHealth()-2);
		Location playerStart = maze.getStartPosition();
		playerStart = maze.cellToPx(playerStart);
		player.setPosition(Vector3(playerStart.x,0,playerStart.z));
		Location masterStart;
		masterStart.x = rand()%5;
		masterStart.z = rand()%5;
		Location temp = maze.cellToPx(masterStart);
		dungeonMaster.setPosition(Vector3(temp.x,0,temp.z));
		dungeonMaster.setPath(&maze,maze.pxToCell(masterStart),maze.getStartPosition());
		audio->playCue(WILHELM);
	}

	for(int i = 0; i<maxProjectile; i++)
	{
		if(dungeonMaster.collided(&projectile[i]))
		{
			projectile[i].setInActive();
			Location masterStart;
			masterStart.x = rand()%5;
			masterStart.z = rand()%5;
			Location temp = maze.cellToPx(masterStart);
			dungeonMaster.setPosition(Vector3(temp.x,0,temp.z));
			Location pp;
			pp.x = player.getPosition().x;
			pp.z = player.getPosition().z;
			pp = maze.pxToCell(pp);
			dungeonMaster.setPath(&maze,maze.pxToCell(masterStart),pp);
			audio->playCue(G_HIT);
		}
	}

	ghosts.update(dt,&player,camera.getTarget(),perspective);
	//ghost collision detection
	
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		//player gets hit by a ghost
		if(player.collided(&ghosts.getEnemies()[i]))
		{
			player.setHealth(player.getHealth()-1);
			//make flash take longer
			ghosts.getEnemies()[i].setInActive();
			ghosts.getEnemies()[i].getLight()->pos = Vector3(100,100,100);
			audio->playCue(P_HIT);
		}
	}

	if(endCube.collided(&player))
	{
		goal = true;
	}

	//flashlight ghost attack
	if(flashLightObject.getOn())
	{
		for(int i = 0; i < ghosts.getNumEnemies(); i++)
		{
			if(flashLightObject.hitTarget(&ghosts.getEnemies()[i]))
			{
				ghosts.getEnemies()[i].setHit(true);
				ghosts.getEnemies()[i].decreaseHealth();
				audio->playCue(G_HIT);
			}
			else
			{
				ghosts.getEnemies()[i].setHit(false);
			}
		}
	}
}

void ColoredCubeApp::updateSplashScreen(float dt,ID3D10ShaderResourceView* screen)
{
	ambientLighting.ambient = D3DXCOLOR(1,1,1,1);
	if(sceneAlpha<0.9)
	{
		sceneAlpha+=dt;
	}
	float rad = 0.0f;
	camera.update(mTheta,mPhi,rad,0,dt,player,mView,mEyePos,true,false,sensitivity);
	//set ceiling texture here
	maze.setCeilTex(screen,brickSpecMap);
	maze.update(dt);
}

void ColoredCubeApp::updatePerspectiveSwitch(float dt)
{
	gamestate = nextState;
}

void ColoredCubeApp::drawTitle()
{

}

void ColoredCubeApp::drawControls()
{

}

void ColoredCubeApp::drawL1()
{

}

void ColoredCubeApp::drawL2()
{

}

void ColoredCubeApp::drawLose()
{

}

void ColoredCubeApp::drawWin()
{

}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    //shaderFlags |= D3D10_SHADER_DEBUG;
	//shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
  
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"tex.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	}  

	mTech = mFX->GetTechniqueByName("TexTech");
	mTechColor2 = mFX->GetTechniqueByName("ColorTech");
	mTechBillboard = mFX->GetTechniqueByName("BillboardTech");
	mTechFog = mFX->GetTechniqueByName("fogTech");
	mTechFogColor = mFX->GetTechniqueByName("fogColorTech");
	mTechFogCeil = mFX->GetTechniqueByName("ceilingFogTech");
	mTechEmmisive = mFX->GetTechniqueByName("emmisiveTech");
	//mTechStatic = mFX->GetTechniqueByName("StaticTech");

	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxLightVar      = mFX->GetVariableByName("gLight");
	mfxAlpha		 = mFX->GetVariableByName("alphaValue")->AsScalar();
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxProjMatrix    = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mfxNumLights	 = mFX->GetVariableByName("gNumLights")->AsScalar();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 7, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

void ColoredCubeApp::updateGameState()
{
		if(gamestate == title && (GetAsyncKeyState('E') & 0x8000) && !splashScreenDebounced)
		{
			gamestate = controls;
			splashScreenDebounced = true;
		}
	   if(gamestate == controls && (GetAsyncKeyState('E') & 0x8000) && !splashScreenDebounced)
       {
		   gamestate = splash;
			nextState = level1;
			splashScreen = l1Splash;
			splashScreenDebounced = true;
       }
		if(!(GetAsyncKeyState('E') & 0x8000))
		{
			splashScreenDebounced = false;
		}
       if(gamestate == level1 && (currentKeys==totalKeys||(GetAsyncKeyState('P') & 0x8000)))
       {
		   lights[1] = ambientLighting;
		    gamestate = transition;
			transitionState = splash;
            nextState = level2;
			splashScreen = l2Splash;
			sceneAlpha = 1.0f;
			fog = true;
       }
       if(gamestate == level2 && goal ||(GetAsyncKeyState('Q') & 0x8000))
       {
		    goal = false;
			lights[1] = ambientLighting;
            nextState = level3;
		    gamestate = transition;
			transitionState = splash;
			splashScreen = l3Splash;
			sceneAlpha = 1.0f;
			fog = false;
       }
	   if(gamestate == level3 && goal||(GetAsyncKeyState('N') & 0x8000))
	   {
		   goal = false;
		   gamestate = transition;
		   nextState = level4;
		   lights[1] = ambientLighting;
		   //change to level 4 splash screen
		   splashScreen = l4Splash;
			transitionState = splash;
			sceneAlpha = 1.0f;
			fog = false;
	   }
	   if(gamestate == level4 && goal)
	   {
		   gamestate = transition;//WOOHOO!!!
		   transitionState = win;
	   }
	   if(gamestate == splash && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	   {
		   gamestate = nextState;
		   maze.setCeilTex(ceilingTexture,brickSpecMap);
		   reloadLevel(10,10);
	   }
       if((player.getHealth() <= 0))
       {
              gamestate = gameover;
			  ambientLighting.ambient = ambientLight;
       }
       if((gamestate == gameover ||gamestate==win) && (GetAsyncKeyState('P') & 0x8000))
       {
              gamestate = title;
			  reloadLevel(10,10,true);
			  player.setHealth(10);
			  goal = false;
       }
}

void ColoredCubeApp::reloadLevel(int x, int z, bool keys)
{
	//reset breadcrumbs
	breadNumber = 0;
	//Reset projectiles
	projectileNum = 9;
	for(int i = 0; i < maxBread; i++)
	{
		breadCrumbs[i].setInActive();
	}
	for(int i = 0; i < maxProjectile; i++)
	{
		projectile[i].setInActive();
	}

	guardsHit = 0;
	mazeX = x;
	mazeZ = z;
	currentKeys = 0;
	flashLightObject.getBattery();
	//add in the maze reset function
	Dimension d;
	d.x=x;
	d.z = z;
	maze.init(d,mfxWVPVar,mfxWorldVar,md3dDevice);
	maze.build();

	//put the player in the right spot
	Location start = maze.getStartPosition();
	start = maze.cellToPx(start);
	player.setPosition(Vector3(start.x,0,start.z));

	Location s;
	s.x = 2;
	s.z = 2;
	//testTracker.setPath(&maze,s,maze.getStartPosition());
	testTracker.setPosition(Vector3(maze.cellToPx(s).x,0,maze.cellToPx(s).z));

	//put the goal box in the correct location
	Location end = maze.getEndPosition();
	end = maze.cellToPx(end);
	if(gamestate == level4)
	{
		endCube.setPosition(Vector3(end.x,0,end.z));
		endLight.pos = D3DXVECTOR3(end.x,10,end.z);
		Location temp;
		temp.x = rand()%9;
		temp.z = rand()%9;
		dungeonMaster.setPath(&maze,temp,maze.getStartPosition());
		dungeonMaster.setPosition(Vector3(maze.cellToPx(temp).x,0,maze.cellToPx(temp).z));
	}
	else
	{
		endLight.pos = Vector3(0,1000,0);
	}

	if(gamestate == level3)
	{
		Location trackerStart;
		trackerStart.x = rand()%8;
		trackerStart.z = rand()%8;
		Location trackerEnd;
		trackerEnd.x = rand()%8;
		trackerEnd.z = rand()%8;
		testTracker.setPath(&maze,trackerStart,trackerEnd);
		testTracker.setPosition(Vector3(maze.cellToPx(trackerStart).x,0,maze.cellToPx(trackerStart).z));
		trackerStart.x = rand()%8;
		trackerStart.z = rand()%8;
		trackerEnd.x = rand()%8;
		trackerEnd.z = rand()%8;
		testTracker2.setPath(&maze,trackerStart,trackerEnd);
		testTracker2.setPosition(Vector3(maze.cellToPx(trackerStart).x,0,maze.cellToPx(trackerStart).z));
	}

	flashLightObject.turnOn();

	//reset the maze textures
	maze.setTex(brickTexture,brickSpecMap);	
	maze.setTexLocVariable(mfxDiffuseMapVar, mfxSpecMapVar);
	maze.setCeilTex(ceilingTexture,brickSpecMap);
	maze.setFloorTex(floorTexture,brickSpecMap);

	//place extra items in random cells
	for(int i = 0; i < numLightObjects; i++)
	{
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		lamps[i].setPosition(Vector3(spot.x,5,spot.z));
	}
	if(gamestate == level1 && keys)
	{
		for(int i = 0; i < totalKeys; i++)
		{
			Location l;
			l.x = rand()%mazeX;
			l.z = rand()%mazeZ;
			auto spot = maze.cellToPx(l);
			keyObject[i].setPosition(Vector3(spot.x,-1,spot.z));
			keyLights[i]->pos = Vector3(spot.x,5,spot.z);
			keyObject[i].setActive();
		}
	}
	//if(!keys)
	//{
	//	for(int i = 0; i < totalKeys; i++)
	//	{
	//		keyLights[i]->pos = Vector3(0,1000,0);
	//	}
	//}
	for(int i = 0; i < numBatteries; i++)
	{
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		batteries[i].setPosition(Vector3(spot.x,-1,spot.z));
		batteries[i].setActive();
	}
}

void ColoredCubeApp::dropBread(Vector3 pos)
 {
	 if(breadNumber>=maxBread)
		 return;
	 breadCrumbs[breadNumber].setPosition(pos);
	 breadCrumbs[breadNumber].setActive();
	 //breadCrumbs[breadNumber].setVelocity(camera.movementFP(camera.getTarget(),perspective));
	 //camera.moveObject(breadCrumbs[breadNumber],20.0f, camera.getTarget(),perspective);
	 breadNumber++;
 }

void ColoredCubeApp::shootProjectile(Vector3 pos)
 {
	 if(projectileNum>=maxProjectile)
		 return;
	 projectile[projectileNum].setPosition(pos);
	 projectile[projectileNum].setActive();
	 camera.moveObject(projectile[projectileNum],40.0f, camera.getTarget(),perspective);
	 projectileNum++;
 }