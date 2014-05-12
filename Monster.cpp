#include "Monster.h"
#include "Vertex.h"

Monster::Monster()
: Geometry()
{
}
 
Monster::~Monster()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Monster::init(ID3D10Device* device, float scale)
{
	md3dDevice = device;
 
	mNumVertices = 56;
	mNumFaces    = 28; // 2 per quad

	// Create vertex buffer
    Vertex v[56];

	// Fill in the front face vertex data.
 	v[0] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[1] = Vertex(-1.0f, 3.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[2] = Vertex( 1.0f, 3.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[3] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	// Fill in the back face vertex data.
	v[4] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[5] = Vertex( 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[6] = Vertex( 1.0f,  3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[7] = Vertex(-1.0f,  3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	// Fill in the top face vertex data.
	v[8]  = Vertex(-1.0f, 3.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[9]  = Vertex(-1.0f, 3.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[10] = Vertex( 1.0f, 3.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[11] = Vertex( 1.0f, 3.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[13] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[14] = Vertex( 1.0f, 1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[15] = Vertex(-1.0f, 1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	// Fill in the left face vertex data.
	v[16] = Vertex(-1.0f, 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[17] = Vertex(-1.0f, 3.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[18] = Vertex(-1.0f, 3.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[19] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	// Fill in the right face vertex data.
	v[20] = Vertex( 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[21] = Vertex( 1.0f, 3.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[22] = Vertex( 1.0f, 3.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[23] = Vertex( 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Right Face 2
	v[24] = Vertex( 2.0f, -1.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[25] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[26] = Vertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[27] = Vertex( 2.0f, -1.0f,  2.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Left Face 2
	v[28] = Vertex(-2.0f, -1.0f,  2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[29] = Vertex(-1.0f, 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[30] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[31] = Vertex(-2.0f, -1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Front Face 2
	v[32] = Vertex(-2.0f, -1.0f, -2.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[33] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[34] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[35] = Vertex( 2.0f, -1.0f, -2.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Back Face 2
	v[36] = Vertex(-2.0f, -1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[37] = Vertex( 2.0f, -1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[38] = Vertex( 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[39] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Right Face 3
	v[40] = Vertex( 2.0f, -3.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[41] = Vertex( 2.0f, -1.0f, -2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[42] = Vertex( 2.0f, -1.0f,  2.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[43] = Vertex( 2.0f, -3.0f,  2.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Left Face 3
	v[44] = Vertex(-2.0f, -3.0f,  2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[45] = Vertex(-2.0f, -1.0f,  2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[46] = Vertex(-2.0f, -1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[47] = Vertex(-2.0f, -3.0f, -2.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Front Face 3
	v[48] = Vertex(-2.0f, -3.0f, -2.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[49] = Vertex(-2.0f, -1.0f, -2.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[50] = Vertex( 2.0f, -1.0f, -2.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[51] = Vertex( 2.0f, -3.0f, -2.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	//Back Face 3
	v[52] = Vertex(-2.0f, -3.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[53] = Vertex( 2.0f, -3.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[54] = Vertex( 2.0f, -1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));
	v[55] = Vertex(-2.0f, -1.0f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,D3DXCOLOR(1,1,1,1),D3DXCOLOR(1,1,1,1));

	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
		v[i].pos *= scale;

    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer

	DWORD i[84];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	//Right Face 2
	i[36] = 24; i[37] = 25; i[38] = 26;
	i[39] = 24; i[40] = 26; i[41] = 27;

	//Left Face 2
	i[42] = 28; i[43] = 29; i[44] = 30;
	i[45] = 28; i[46] = 30; i[47] = 31;

	//Front Face 2
	i[48] = 32; i[49] = 33; i[50] = 34;
	i[51] = 32; i[52] = 34; i[53] = 35;

	//Back Face 2
	i[54] = 36; i[55] = 37; i[56] = 38;
	i[57] = 36; i[58] = 38; i[59] = 39;

	//Right Face 3
	i[60] = 40; i[61] = 41; i[62] = 42;
	i[63] = 40; i[64] = 42; i[65] = 43;

	//Left Face 3
	i[66] = 44; i[67] = 45; i[68] = 46;
	i[69] = 44; i[70] = 46; i[71] = 47;

	//Front Face 3
	i[72] = 48; i[73] = 49; i[74] = 50;
	i[75] = 48; i[76] = 50; i[77] = 51;

	//Back Face 3
	i[78] = 52; i[79] = 53; i[80] = 54;
	i[81] = 52; i[82] = 54; i[83] = 55;

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = i;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Monster::draw()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
	//md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
}