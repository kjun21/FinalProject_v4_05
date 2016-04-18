#pragma once
//#include "stdafx.h"
#include "Mesh.h"
using namespace std;
class CWaveMesh : public CMeshTexturedIlluminated
{
private:
	ID3D11Buffer *m_ppd3dTangentBuffer;
public:
	CWaveMesh(ID3D11Device *pd3dDevice, UINT m, UINT n,
		float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CWaveMesh();
	

};