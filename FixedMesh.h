#pragma once
#include "stdafx.h"
#include "Mesh.h"
class CFixedMesh : public CMeshTexturedIlluminated
{
public:
	CFixedMesh(ID3D11Device *pd3dDevice, string strFileName);
	virtual ~CFixedMesh();
	virtual  void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

