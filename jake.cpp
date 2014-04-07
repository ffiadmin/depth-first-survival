//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================


#include "d3dApp.h"
#include <d3dx9math.h>

#include "Maze.h"

namespace gameNS {
	const int X = 10;
	const int Z = 10;
}

class ColoredCubeApp : public D3DApp {
public : 
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void drawScene(); 
	void initApp();
	void onResize();
	void updateScene(float dt);

private : 
	void buildFX();
	void buildVertexLayouts();
 
private : 
	Maze *maze;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;

	D3DXMATRIX mProj;
	D3DXMATRIX mView;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	ColoredCubeApp theApp(hInstance);
	theApp.initApp();
	return theApp.run();
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance) : D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0), mfxWVPVar(0) {
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
}

ColoredCubeApp::~ColoredCubeApp() {
	if (md3dDevice)	md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void ColoredCubeApp::initApp() {
	D3DApp::initApp();
	buildFX();
	buildVertexLayouts();

//Create the maze
	Dimension dim = {gameNS::X, gameNS::Z};
	maze = new Maze(dim, mfxWVPVar, md3dDevice);
	maze->build();
}

void ColoredCubeApp::onResize() {
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);
	maze->update(dt);

	// Build the view matrix.
	//D3DXVECTOR3 pos(100.0f,350.0f,99.0f); //Top-down view
	D3DXVECTOR3 pos(25.0f,25.0f,25.0f);
	D3DXVECTOR3 target(100.0f, 0.0f, 100.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
}

void ColoredCubeApp::drawScene() {
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

//Set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

//Draw the maze
    maze->draw(mTech, mView, mProj);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{

	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
		"fx_4_0", 0, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();

}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
