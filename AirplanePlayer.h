#pragma once
#include "Player.h"
#include "stdafx.h"

class CAirplanePlayer : public CPlayer
{
public:
	//CAirplanePlayer(ID3D11Device *pd3dDevice);

	CAirplanePlayer(int nMeshes = 1);
	virtual ~CAirplanePlayer();

	virtual void ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();
};
