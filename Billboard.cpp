#include "Billboard.h"

Billboard::Billboard() : Geometry(){}

Billboard::~Billboard()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Billboard::init(ID3D10Device* device, Vector3 position, Vector2 size)
{
	md3dDevice = device;
	mNumVertices = 1;
	mNumFaces = 1;
	Vertex* v = new Vertex[1];
	v[0] = Vertex(position.x,position.y,position.z,size.x,size.y);

	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
	delete[] v;
}

void Billboard::draw()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
	//md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->Draw(mNumVertices, 0);
}