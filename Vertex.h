

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		float nx, float ny, float nz, 
		float u, float v, D3DXCOLOR d, D3DXCOLOR s)
		: pos(x,y,z), normal(nx,ny,nz), texC(u,v), diffuse(d), spec(s){}
	Vertex(float x, float y, float z, float h, float w) : pos(x,y,z), size(h,w)
	{}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	D3DXCOLOR color;
	D3DXVECTOR2 size;//used only for billboards
};

#endif // VERTEX_H

