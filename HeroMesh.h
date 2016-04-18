#pragma once
#include "Mesh.h"
class CHeroMesh : public CMeshTextured
{
public:
	CHeroMesh(ID3D11Device *pd3dDevice);
	virtual ~CHeroMesh();
};

class CArcherd