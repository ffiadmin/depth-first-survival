#include "Mesh.h"

Mesh::Mesh() : Geometry()
{

}

//make polygon is working.
//does work for multiple
//weird issue with culling I think that it only displays on one side need to fix that
//something with the rotation?
void Mesh::init(ID3D10Device* device, float scale)
{
	md3dDevice = device;
	Vector3 line[4];
	line[0] = Vector3(0,0,0);
	line[1] = Vector3(0,1,0);
	line[2] = Vector3(1,1,0);
	line[3] = Vector3(1,0,0);
	surfRev(20,line,4);
	//makePolygon(line,4);

	Vector3 line2[4];
	line2[0] = Vector3(3,0,0);
	line2[1] = Vector3(3,1,0);
	line2[2] = Vector3(4,1,0);
	line2[3] = Vector3(4,0,0);
	//makePolygon(line2,4);

	Vertex* v = new Vertex[vertices.size()];
	for(int i = 0; i < vertices.size(); i++)
	{
		v[i] = vertices[i];
	}

	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	DWORD* index = new DWORD[indices.size()];
	for(int i = 0; i < indices.size(); i++)
	{
		index[i] = indices[i];
	}

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = index;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));

	delete v;
	delete index;
}

void Mesh::draw()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
}

void Mesh::surfRev(int numSlices, Vector3 linePoints[], int numPoints)
{
	float angleBetween = ToRadian(360 / numSlices);
	Matrix rotate;
	RotateY(&rotate,angleBetween);
	Vector3* originalPoints = linePoints;
	//arrays of the points for the caps if they are necessary
	//in the event the shap has a hole in one of the ends
	Vector3* topCap = new Vector3[numSlices];
	Vector3* bottomCap = new Vector3[numSlices];

	for(int i = 0; i < numSlices; i++)
	{
		topCap[i] = originalPoints[numPoints-1];
		bottomCap[i] = originalPoints[0];
		Vector3* newPoints = new Vector3[numPoints];
		for(int k = 0; k < numPoints; k++)
		{
			Vector4 point = Vector4(originalPoints[k].x,originalPoints[k].y,originalPoints[k].z,1);
			D3DXVec3Transform(&point,&originalPoints[k],&rotate);
			newPoints[k] = Vector3(point.x,point.y,point.z);
		}
		for(int j = 0; j < numPoints; j++)
		{
			Vector3* side = new Vector3[4];
			side[0] = originalPoints[j];
			if((j+1)<numPoints)
			{
				side[1] = originalPoints[j+1];
				side[2] = newPoints[j+1];
			}
			else
			{
				side[1] = originalPoints[0];
				side[2] = newPoints[0];
			}
			side[3] = newPoints[j];
			makePolygon(side,4);
		}
		originalPoints = newPoints;
		//delete[] newPoints;
	}

	//add end cap if necessary
	if(linePoints[numPoints-1].x!=0)
	{
		makePolygon(topCap,numSlices);
	}
	if(linePoints[0].x!=0)
	{
		makePolygon(bottomCap,numSlices);
	}
	delete[] topCap;
}

void Mesh::makePolygon(Vector3 base[], int numPoints)
{
	//triangulate the polygon
	for(DWORD i = 1; i < numPoints-1; i++)
	{
		mNumFaces++;
		indices.push_back(vertices.size());
		indices.push_back(vertices.size() + i);
		indices.push_back(vertices.size() + i+1);
	}

	//put in polygon base points
	Vertex v;
	Vector3 edge1 = (base[numPoints-2]-base[0]);
	Vector3 edge2 = (base[1]-base[0]);
	Vector3 norm;
	Cross(&norm,&edge1,&edge2);
	v.pos = Vector3(base[0].x,base[0].y,base[0].z);
	//have to calculate the normal for the first and last point slightly differently because they are the same point
	Normalize(&v.normal,&norm);
	v.texC = Vector2(0.0f, 1.0f);
	vertices.push_back(v);
	mNumVertices++;

	for(int i = 1; i < numPoints-1; i++)
	{
		v.pos = base[i];
		edge1 = (base[i-1]-base[i]);
		edge2 = (base[i+1]-base[i]);
		Cross(&norm,&edge1,&edge2);
		Normalize(&v.normal,&norm);
		v.texC = Vector2(0.0f, 1.0f);
		vertices.push_back(v);
		mNumVertices++;
	}
	v.pos = base[numPoints-1];
	edge1 = (base[numPoints-2]-base[numPoints-1]);
	edge2 = (base[0]-base[numPoints-1]);
	Cross(&norm,&edge1,&edge2);
	Normalize(&v.normal,&norm);
	v.texC = Vector2(0.0f, 1.0f);
	vertices.push_back(v);
	mNumVertices++;
}