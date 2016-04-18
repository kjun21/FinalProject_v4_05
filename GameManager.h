#pragma once
#include "stdafx.h"
#include "Object.h"
#include "Player.h"
class CGameManager
{
private:
	static CGameManager* GameManager;
public:
	CGameManager();

	~CGameManager();
	static CGameManager* GetCGameTimer()
	{
		if (!GameManager)
			GameManager = new CGameManager();
		return GameManager;
	}

	CGameObject** m_ppGameObject;

	CGameObject* m_pPlayer;
	// 
	CGameObject** m_ppMonster;

	CGameObject* m_potherPlayer;

	UINT m_nMonsterNum;
};
typedef struct Array
{

};

