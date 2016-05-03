#include "Object.h"
#include "Shader.h"
#include "GameFramework.h"
using namespace std;

CMaterial::CMaterial()
{
	m_nReferences = 0;
}

CMaterial::~CMaterial()
{
}
CGameObject::CGameObject(int nMeshes)
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0) m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++
		)m_ppMeshes[i] = NULL;
		m_bcMeshBoundingCube = AABB();

	m_bActive = true;

	m_nReferences = 0;
	m_pMaterial = NULL;
	m_pTexture = NULL;
}

CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}
	if (m_pMaterial) m_pMaterial->Release();
	if (m_pTexture) m_pTexture->Release();
}

void CGameObject::AddRef()
{
	m_nReferences++;
}
void CGameObject::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	if (m_nReferences <= 0) delete this;
}
void CGameObject::SetObjectType(UINT nObjectType)
{
	m_nObjectType = nObjectType;
	//m_nPlayerObject;
}
void CGameObject::SetPlayerType()
{

}
void CGameObject::SetMonsterType()
{

}

void CGameObject::Animate(float fTimeElapsed)
{
}


void CGameObject::SetMesh(CMesh *pMesh, int nIndex)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex])
			m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh)
			pMesh->AddRef();
	}

	if (pMesh)
	{
		AABB bcBoundingCube = pMesh->GetBoundingCube();
		m_bcMeshBoundingCube.Union(&bcBoundingCube);
	}
}
void CGameObject::SetPosition(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x;
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
}

void CGameObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_d3dxmtxWorld._41 = d3dxvPosition.x;
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;
}


void CGameObject::SetRight(D3DXVECTOR3 d3dxvRight)
{
	m_d3dxmtxWorld._11 = d3dxvRight.x;
	m_d3dxmtxWorld._12 = d3dxvRight.y;
	m_d3dxmtxWorld._13 = d3dxvRight.z;
}

void CGameObject::SetUp(D3DXVECTOR3 d3dxvUp)
{
	m_d3dxmtxWorld._21 = d3dxvUp.x;
	m_d3dxmtxWorld._22 = d3dxvUp.y;
	m_d3dxmtxWorld._23 = d3dxvUp.z;
}
void CGameObject::SetDirection(D3DXVECTOR3 d3dxvDirection)
{
	m_d3dxmtxWorld._31 = d3dxvDirection.x;
	m_d3dxmtxWorld._32 = d3dxvDirection.y;
	m_d3dxmtxWorld._33 = d3dxvDirection.z;
}


void CGameObject::RenewWorldMatrix()
{
	D3DXVECTOR3 d3dxvRight = D3DXVECTOR3(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVECTOR3 d3dxvUp = D3DXVECTOR3(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVECTOR3 d3dxvLook = D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);

	D3DXVec3Normalize(&d3dxvLook, &d3dxvLook);
	//카메라의 z-축과 y-축에 수직인 벡터를 x-축으로 설정한다.
	D3DXVec3Cross(&d3dxvRight, &d3dxvUp, &d3dxvLook);
	//카메라의 x-축 벡터를 정규화한다.
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	//카메라의 z-축과 x-축에 수직인 벡터를 y-축으로 설정한다.
	D3DXVec3Cross(&d3dxvUp, &d3dxvLook, &d3dxvRight);
	//카메라의 y-축 벡터를 정규화한다.
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);

	m_d3dxmtxWorld._11 = d3dxvRight.x;
	m_d3dxmtxWorld._12 = d3dxvRight.y;
	m_d3dxmtxWorld._13 = d3dxvRight.z;

	m_d3dxmtxWorld._21 = d3dxvUp.x;
	m_d3dxmtxWorld._22 = d3dxvUp.y;
	m_d3dxmtxWorld._23 = d3dxvUp.z;

	m_d3dxmtxWorld._31 = d3dxvLook.x;
	m_d3dxmtxWorld._32 = d3dxvLook.y;
	m_d3dxmtxWorld._33 = d3dxvLook.z;
}


D3DXVECTOR3 CGameObject::GetPosition()
{
	return(D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43));
}
///////////
D3DXVECTOR3 CGameObject::GetLookAt()
{
	//게임 객체를 로컬 z-축 벡터를 반환한다.
	D3DXVECTOR3 d3dxvLookAt(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

D3DXVECTOR3 CGameObject::GetUp()
{
	//게임 객체를 로컬 y-축 벡터를 반환한다.
	D3DXVECTOR3 d3dxvUp(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

D3DXVECTOR3 CGameObject::GetRight()
{
	//게임 객체를 로컬 x-축 벡터를 반환한다.
	D3DXVECTOR3 d3dxvRight(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

void CGameObject::MoveStrafe(float fDistance)
{
	//게임 객체를 로컬 x-축 방향으로 이동한다.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveUp(float fDistance)
{
	//게임 객체를 로컬 y-축 방향으로 이동한다.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveForward(float fDistance)
{
	//게임 객체를 로컬 z-축 방향으로 이동한다.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLookAt();
	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	//게임 객체를 주어진 각도로 회전한다.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

void CGameObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	//게임 객체를 주어진 회전축을 중심으로 회전한다.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}
void  CGameObject::Scale(D3DXVECTOR3 pd3dxvAxis)
{
	D3DXMATRIX mtxRotate;

	D3DXMatrixScaling(&mtxRotate, pd3dxvAxis.x, pd3dxvAxis.y, pd3dxvAxis.z);
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}


void CGameObject::GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection)
{
	//pd3dxvPickPosition: 카메라 좌표계의 점(화면 좌표계에서 마우스를 클릭한 점을 역변환한 점)
	//pd3dxmtxWorld: 월드 변환 행렬, pd3dxmtxView: 카메라 변환 행렬
	//pd3dxvPickRayPosition: 픽킹 광선의 시작점, pd3dxvPickRayDirection: 픽킹 광선 벡터
	/*객체의 월드 변환 행렬이 주어지면 객체의 월드 변환 행렬과 카메라 변환 행렬을 곱하고 역행렬을 구한다. 이것은 카메라 변환 행렬의 역행렬과 객체의 월드 변환 행렬의 역행렬의 곱과 같다. 객체의 월드 변환 행렬이 주어지지 않으면 카메라 변환 행렬의 역행렬을 구한다. 객체의 월드 변환 행렬이 주어지면 모델 좌표계의 픽킹 광선을 구하고 그렇지 않으면 월드 좌표계의 픽킹 광선을 구한다.*/
	D3DXMATRIX d3dxmtxInverse;
	D3DXMATRIX d3dxmtxWorldView = *pd3dxmtxView;
	if (pd3dxmtxWorld) D3DXMatrixMultiply(&d3dxmtxWorldView, pd3dxmtxWorld, pd3dxmtxView);
	D3DXMatrixInverse(&d3dxmtxInverse, NULL, &d3dxmtxWorldView);
	D3DXVECTOR3 d3dxvCameraOrigin(0.0f, 0.0f, 0.0f);
	/*카메라 좌표계의 원점 (0, 0, 0)을 위에서 구한 역행렬로 변환한다. 변환의 결과는 카메라 좌표계의 원점에 대응되는 모델 좌표계의 점 또는 월드 좌표계의 점이다.*/
	D3DXVec3TransformCoord(pd3dxvPickRayPosition, &d3dxvCameraOrigin, &d3dxmtxInverse);
	/*카메라 좌표계의 점을 위에서 구한 역행렬로 변환한다. 변환의 결과는 마우스를 클릭한 점에 대응되는 모델 좌표계의 점 또는 월드 좌표계의 점이다.*/
	D3DXVec3TransformCoord(pd3dxvPickRayDirection, pd3dxvPickPosition, &d3dxmtxInverse);
	//픽킹 광선의 방향 벡터를 구한다.
	*pd3dxvPickRayDirection = *pd3dxvPickRayDirection - *pd3dxvPickRayPosition;
}

int CGameObject::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;
	if (m_bActive && m_ppMeshes)
	{
		GenerateRayForPicking(pd3dxvPickPosition, &m_d3dxmtxWorld, pd3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);
		for (int i = 0; i < m_nMeshes; i++)
		{
			nIntersected = m_ppMeshes[i]->CheckRayIntersection(&d3dxvPickRayPosition, &d3dxvPickRayDirection, pd3dxIntersectInfo);
			if (nIntersected > 0) break;
		}
	}
	return(nIntersected);
}


bool CGameObject::IsVisible(CCamera *pCamera)
{
	// 플레이어의 정보를 갱신하는 마법의 함수
	// 보일 때만 갱신한다?
	OnPrepareRender();

	bool bIsVisible = false;
	if (m_bActive)
	{
		AABB bcBoundingCube = m_bcMeshBoundingCube;
		bcBoundingCube.Update(&m_d3dxmtxWorld);
		if (pCamera) bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
	}
	return(bIsVisible);
}


void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);

	// 플레이어의 m_d3dPosition과 월드 행렬이 같아야하는거 아님?

	if (m_pMaterial)
		CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);
	//객체의 텍스쳐를 쉐이더 변수에 설정(연결)한다.
	if (m_pTexture) m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
			{
				bool bIsVisible = true;
				if (pCamera)
				{
					AABB bcBoundingCube = m_ppMeshes[i]->GetBoundingCube();
					bcBoundingCube.Update(&m_d3dxmtxWorld);
					bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
				}
				if (bIsVisible) m_ppMeshes[i]->Render(pd3dDeviceContext);
				//m_ppMeshes[i]->Render(pd3dDeviceContext);
			}
		}
	}
}
void CGameObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

//새로 추가된
void CGameObject::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}



CRevolvingObject::CRevolvingObject(int nMeshes) : CGameObject(nMeshes)
{
	m_d3dxvRevolutionAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 0.0f;
}

CRevolvingObject::~CRevolvingObject()
{
}

void CRevolvingObject::Animate(float fTimeElapsed)
{
	//공전을 나타내기 위해 회전 행렬을 오른쪽에 곱한다.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRevolutionAxis, (float)D3DXToRadian(m_fRevolutionSpeed * fTimeElapsed));
	m_d3dxmtxWorld = m_d3dxmtxWorld * mtxRotate;
}

CHeightMapTerrain::CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale) : CGameObject(0)
{

	m_nWidth = nWidth; //257
	m_nLength = nLength; //257

	int cxQuadsPerBlock = nBlockWidth - 1; //  257 - 1
	int czQuadsPerBlock = nBlockLength - 1;// 257 - 1
	// 67 
	//1024   1024

	// d3dxvScale(8.0f, 2.0f, 8.0f);
	m_d3dxvScale = d3dxvScale;

	m_pHeightMap = new CHeightMap(pFileName, nWidth, nLength, d3dxvScale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;

	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, xStart, zStart, nBlockWidth, nBlockLength, d3dxvScale, m_pHeightMap);
			SetMesh(pHeightMapGridMesh, x + (z*cxBlocks));
		}
	}
}

CHeightMapTerrain::~CHeightMapTerrain()
{
	if (m_pHeightMap) delete m_pHeightMap;
}


CTexture::CTexture(int nTextures, int nSamplers, int nTextureStartSlot, int nSamplerStartSlot)
{
	m_nReferences = 0;

	m_nTextures = nTextures;
	m_ppd3dsrvTextures = new ID3D11ShaderResourceView*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) 
		m_ppd3dsrvTextures[i] = NULL;
	m_nTextureStartSlot = nTextureStartSlot;
	m_nSamplers = nSamplers;
	m_ppd3dSamplerStates = new ID3D11SamplerState*[m_nSamplers];
	for (int i = 0; i < m_nSamplers; i++) m_ppd3dSamplerStates[i] = NULL;
	m_nSamplerStartSlot = nSamplerStartSlot;
}

CTexture::~CTexture()
{
	for (int i = 0; i < m_nTextures; i++) if (m_ppd3dsrvTextures[i]) m_ppd3dsrvTextures[i]->Release();
	for (int i = 0; i < m_nSamplers; i++) if (m_ppd3dSamplerStates[i]) m_ppd3dSamplerStates[i]->Release();
	if (m_ppd3dsrvTextures) delete[] m_ppd3dsrvTextures;
	if (m_ppd3dSamplerStates) delete[] m_ppd3dSamplerStates;
}

void CTexture::SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture)
{
	if (m_ppd3dsrvTextures[nIndex])
		m_ppd3dsrvTextures[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
	if (pd3dsrvTexture)
		pd3dsrvTexture->AddRef();
}

void CTexture::SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState)
{
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
	if (pd3dSamplerState) pd3dSamplerState->AddRef();
}

void CTexture::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->PSSetShaderResources(m_nTextureStartSlot, m_nTextures, m_ppd3dsrvTextures);
	pd3dDeviceContext->PSSetSamplers(m_nSamplerStartSlot, m_nSamplers, m_ppd3dSamplerStates);
}
//
void CTexture::UpdateShaderVariableDS(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->DSSetShaderResources(m_nTextureStartSlot, m_nTextures, m_ppd3dsrvTextures);
	pd3dDeviceContext->DSSetSamplers(m_nSamplerStartSlot, m_nSamplers, m_ppd3dSamplerStates);
}
void CTexture::UpdateShaderVariableVS(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->VSSetShaderResources(m_nTextureStartSlot, m_nTextures, m_ppd3dsrvTextures);
	pd3dDeviceContext->VSSetSamplers(m_nSamplerStartSlot, m_nSamplers, m_ppd3dSamplerStates);
}

void CTexture::UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetShaderResources(nSlot, 1, &m_ppd3dsrvTextures[nIndex]);
}

void CTexture::UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetSamplers(nSlot, 1, &m_ppd3dSamplerStates[nIndex]);
}


CSkyBox::CSkyBox(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	CSkyBoxMesh *pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, 20.0f, 20.0f, 20.0f);
	SetMesh(pSkyBoxMesh, 0);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	//스카이 박스 객체의 위치를 카메라의 위치로 변경한다.
	D3DXVECTOR3 d3dxvCameraPos = pCamera->GetPosition();
	SetPosition(d3dxvCameraPos.x, d3dxvCameraPos.y, d3dxvCameraPos.z);
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);

	//스카이 박스 메쉬(6개의 사각형)를 렌더링한다.
	if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);
}

//====================================================
CPlaneObject::CPlaneObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	CPlaneMesh *pPlaneMesh = new CPlaneMesh(pd3dDevice, 60.0f, 60.0f, 0.0f);
	SetMesh(pPlaneMesh, 0);
}

CPlaneObject::~CPlaneObject()
{
}

void CPlaneObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);

	if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);
}


CPlaneNormalMappingObject::CPlaneNormalMappingObject(ID3D11Device *pd3dDevice) : CPlaneObject(pd3dDevice)
{
	CNormalMappingPlaneMesh *pNormalMappingPlaneMesh = new CNormalMappingPlaneMesh(pd3dDevice, 60.0f, 60.0f, 0.0f);
	SetMesh(pNormalMappingPlaneMesh, 0);


	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	m_pNoramlMap = new CTexture(1, 1, 1, 1);
	m_pNoramlMap->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();
	m_pNoramlMap->AddRef();

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/stones_nmap.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pNoramlMap->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
}

CPlaneNormalMappingObject::~CPlaneNormalMappingObject()
{
}

void CPlaneNormalMappingObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
	m_pNoramlMap->UpdateShaderVariable(pd3dDeviceContext);
	if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);
}


CTessellationObject::CTessellationObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	CTessellationMesh *pTessellationMesh = new  CTessellationMesh(pd3dDevice, 100.0f, 100.0f, 00.0f);
	SetMesh(pTessellationMesh, 0);
}

CTessellationObject::~CTessellationObject()
{
}

void CTessellationObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateWorldMatrixForTess(pd3dDeviceContext, &m_d3dxmtxWorld);

	if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);
}



CDisplacementMappingObject::CDisplacementMappingObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	CDisplacementMappingMesh *pTessellationMesh = new  CDisplacementMappingMesh(pd3dDevice, 80.0f, 50.0f, 0.0f);
	SetMesh(pTessellationMesh, 0);

	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	m_pNoramlMap = new CTexture(1, 1, 1, 1);
	m_pNoramlMap->SetSampler(0, pd3dSamplerState);

	// m_pNoramlMap->AddRef();
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/stones_nmap.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pNoramlMap->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();
}

CDisplacementMappingObject::~CDisplacementMappingObject()
{
}


void CDisplacementMappingObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
	//노말맵을 픽셀, 도메인 셰이더에 연결
	m_pNoramlMap->UpdateShaderVariable(pd3dDeviceContext);
	m_pNoramlMap->UpdateShaderVariableDS(pd3dDeviceContext);

	if (m_ppMeshes && m_ppMeshes[0])
		m_ppMeshes[0]->Render(pd3dDeviceContext);

	m_pNoramlMap->ClearShaderVariableDS(pd3dDeviceContext);
}

CWaveObject::CWaveObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{//990
	CWaveMesh *pWaveMesh = new  CWaveMesh(pd3dDevice, 80.0, 80.0, 1300.0f, 50.0f, 1080.0f); //800 1900
	//80.0, 80.0, 800.0f, 50.0f, 1560.0f);
	SetMesh(pWaveMesh, 0);
	m_pStonesTexture = NULL;
	m_pWavesNormalMap1 = NULL;
	m_pWavesNormalMap2 = NULL;
	m_pMaterial = NULL;
	CreateTextures(pd3dDevice);
	CreateWaveConsBuffer(pd3dDevice);
	CreateBlendingState(pd3dDevice);
	m_pMaterial = new CMaterial();

	m_pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.4f, 0.4f, 0.7f, 1.0f); //0.2f, 0.1f, 1.0f, 1.0f);
	m_pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.4f, 1.0f);
	m_pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 128.0f);
	m_pMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
}

void CWaveObject::CreateTextures(ID3D11Device *pd3dDevice)
{
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	//텍스쳐 리소스를 생성한다.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	m_pStonesTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/water2.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pStonesTexture->SetTexture(0, pd3dsrvTexture);
	m_pStonesTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();

	m_pWavesNormalMap1 = new CTexture(1, 1, 1, 1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/waves0.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pWavesNormalMap1->SetTexture(0, pd3dsrvTexture);
	m_pWavesNormalMap1->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();

	m_pWavesNormalMap2 = new CTexture(1, 1, 2, 1);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/water1.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pWavesNormalMap2->SetTexture(0, pd3dsrvTexture);
	m_pWavesNormalMap2->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();


	pd3dSamplerState->Release();

}
CWaveObject::~CWaveObject()
{
}

void CWaveObject::CreateWaveConsBuffer(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WAVE_MATRICES);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWavesTranformMatrices);
}
void CWaveObject::CreateBlendingState(ID3D11Device *pd3dDevice)
{
	D3D11_BLEND_DESC d3dBlendDesc;
	ZeroMemory(&d3dBlendDesc, sizeof(d3dBlendDesc));
	d3dBlendDesc.AlphaToCoverageEnable = false;
	d3dBlendDesc.IndependentBlendEnable = false;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	D3D11_COLOR_WRITE_ENABLE_ALPHA;
	D3D11_COLOR_WRITE_ENABLE_ALL;

	pd3dDevice->CreateBlendState(&d3dBlendDesc, &m_pd3dBlendingState);
}
void CWaveObject::UpdateShaderVariable(ID3D11DeviceContext* pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWavesTranformMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WAVE_MATRICES *pcbWaveMatrices = (VS_CB_WAVE_MATRICES*)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWaveMatrices->m_d3dxmtxWavesDispTexTransform1, &m_d3dxmtxWavesDispTexTransform1);
	D3DXMatrixTranspose(&pcbWaveMatrices->m_d3dxmtxWavesDispTexTransform2, &m_d3dxmtxWavesDispTexTransform2);
	D3DXMatrixTranspose(&pcbWaveMatrices->m_d3dxmtxWavesNormalTexTransform1, &m_d3dxmtxWavesNormalTexTransform1);
	D3DXMatrixTranspose(&pcbWaveMatrices->m_d3dxmtxWavesNormalTexTransform2, &m_d3dxmtxWavesNormalTexTransform2);

	pd3dDeviceContext->Unmap(m_pd3dcbWavesTranformMatrices, 0);

	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WAVE_MATRICES, 1, &m_pd3dcbWavesTranformMatrices);
}
void CWaveObject::Animate(float fTimeElapsed)
{
	m_WavesDispOffset1.x += 0.01f*fTimeElapsed;  //0.01
	m_WavesDispOffset1.y += 0.03f*fTimeElapsed;  //0.03

	m_WavesDispOffset2.x += 0.01f*fTimeElapsed;
	m_WavesDispOffset2.y += 0.03f*fTimeElapsed;

	D3DXMATRIX d3dxmtxWaveScale1; 
	D3DXMatrixScaling(&d3dxmtxWaveScale1, 2.0f, 2.0f, 1.0f);
	D3DXMATRIX d3dxmtxWaveOffset1;
	D3DXMatrixTranslation(&d3dxmtxWaveOffset1, m_WavesDispOffset1.x, m_WavesDispOffset1.y, 0.0f);
	

	// S * R * T 
	D3DXMatrixMultiply(&m_d3dxmtxWavesDispTexTransform1, &d3dxmtxWaveScale1, &d3dxmtxWaveOffset1);

	D3DXMATRIX d3dxmtxWaveScale2;
	D3DXMatrixScaling(&d3dxmtxWaveScale2, 1.0f, 1.0f, 1.0f);
	D3DXMATRIX d3dxmtxWaveOffset2;
	D3DXMatrixTranslation(&d3dxmtxWaveOffset2, m_WavesDispOffset2.x, m_WavesDispOffset2.y, 0.0f);
	

	// S * R * T 
	D3DXMatrixMultiply(&m_d3dxmtxWavesDispTexTransform2, &d3dxmtxWaveScale2, &d3dxmtxWaveOffset2);

	// 노말 조정.
	m_WavesNormalOffset1.x += 0.05f * fTimeElapsed;
	m_WavesNormalOffset1.y += 0.2f * fTimeElapsed;

	m_WavesNormalOffset2.x -= 0.02f * fTimeElapsed;
	m_WavesNormalOffset2.y += 0.05f * fTimeElapsed;

	D3DXMatrixScaling(&d3dxmtxWaveScale1, 22.0f, 22.0f, 1.0f);
	D3DXMatrixTranslation(&d3dxmtxWaveOffset1, m_WavesNormalOffset1.x, m_WavesNormalOffset1.y, 0.0f);
	D3DXMatrixMultiply(&m_d3dxmtxWavesNormalTexTransform1, &d3dxmtxWaveScale1, &d3dxmtxWaveOffset1);


	D3DXMatrixScaling(&d3dxmtxWaveScale2, 16.0f, 16.0f, 1.0f);
	D3DXMatrixTranslation(&d3dxmtxWaveOffset2, m_WavesNormalOffset2.x, m_WavesNormalOffset2.y, 0.0f);
	D3DXMatrixMultiply(&m_d3dxmtxWavesNormalTexTransform2, &d3dxmtxWaveScale2, &d3dxmtxWaveOffset2);


}

void CWaveObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	UpdateShaderVariable(pd3dDeviceContext);
	m_pStonesTexture->UpdateShaderVariable(pd3dDeviceContext);
	m_pWavesNormalMap1->UpdateShaderVariableDS(pd3dDeviceContext);
	m_pWavesNormalMap2->UpdateShaderVariableDS(pd3dDeviceContext);

	m_pWavesNormalMap1->UpdateShaderVariable(pd3dDeviceContext);
	m_pWavesNormalMap2->UpdateShaderVariable(pd3dDeviceContext);
	
	float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pd3dDeviceContext->OMSetBlendState(m_pd3dBlendingState, blendFactors, 0xfffffff);
	CGameObject::Render(pd3dDeviceContext, pCamera);
	pd3dDeviceContext->OMSetBlendState(0, NULL, 0xffffffff);

	//CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
	//노말맵을 픽셀, 도메인 셰이더에 연결
	//m_pNoramlMap->UpdateShaderVariable(pd3dDeviceContext);
	//m_pNoramlMap->UpdateShaderVariableDS(pd3dDeviceContext);

	//if (m_ppMeshes && m_ppMeshes[0])
	//	m_ppMeshes[0]->Render(pd3dDeviceContext);


	//m_pNoramlMap->ClearShaderVariableDS(pd3dDeviceContext);
}


CTestDisplacementMappingObject::CTestDisplacementMappingObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	CTestDisplacementMappingMesh *pTessellationMesh = new  CTestDisplacementMappingMesh(pd3dDevice, 80.0f, 50.0f, 0.0f);
	SetMesh(pTessellationMesh, 0);

	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	m_pNoramlMap = new CTexture(1, 1, 1, 1);
	m_pNoramlMap->SetSampler(0, pd3dSamplerState);

	// m_pNoramlMap->AddRef();
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/stones_nmap.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pNoramlMap->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();

}

CTestDisplacementMappingObject ::~CTestDisplacementMappingObject()
{
}

void CTestDisplacementMappingObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);


	//노말맵을 픽셀, 도메인 셰이더에 연결
	m_pNoramlMap->UpdateShaderVariable(pd3dDeviceContext);
	m_pNoramlMap->UpdateShaderVariableDS(pd3dDeviceContext);

	if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);

}





CTessellationHeightMapTerrain::CTessellationHeightMapTerrain(ID3D11Device *pd3dDevice, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale) : CGameObject(0)
{
	m_nWidth = nWidth; //257
	m_nLength = nLength; //257

	m_d3dxvScale = d3dxvScale;


	m_nMeshes = 1;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;

	CTessellationHeightMapGridMesh  *pHeightMapGridMesh =
		new CTessellationHeightMapGridMesh(pd3dDevice, m_nWidth, m_nLength, d3dxvScale);

	SetMesh(pHeightMapGridMesh, 0);

}

CTessellationHeightMapTerrain::~CTessellationHeightMapTerrain()
{
}


CHumanObject::CHumanObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	CHumanMesh *pHumanMesh = new CHumanMesh(pd3dDevice);
	SetMesh(pHumanMesh, 0);
}

CHumanObject::~CHumanObject()
{
}

void  CHumanObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);

	if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);
}


CAnimatedObject::CAnimatedObject(ID3D11Device *pd3dDevice, string strFileName) : CGameObject(1)
{
	//string strFileName = "Data/walk02Box001";

	/*CCharacterMesh *pHumanMesh = new CCharacterMesh(pd3dDevice, strFileName);
	SetMesh(pHumanMesh, 0);*/

	//ID3D11SamplerState *pd3dSamplerState = NULL;
	//D3D11_SAMPLER_DESC d3dSamplerDesc;
	//ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	//d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//d3dSamplerDesc.MinLOD = 0;
	//d3dSamplerDesc.MaxLOD = 0;
	//pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	////텍스쳐 리소스를 생성한다.
	//ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	//m_pTexture = new CTexture(1, 1, 0, 0);
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/StoneKing.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	//m_pTexture->SetTexture(0, pd3dsrvTexture);
	//m_pTexture->SetSampler(0, pd3dSamplerState);

	//pd3dsrvTexture->Release();
	//pd3dSamplerState->Release();

	//CreateShaderVariables(pd3dDevice);
	//AnimationClip* animationClip = NULL;
	//m_uiAnimationClipNum = 5;
	//m_AnimationClip = CreateAnimation(animationClip);


}
CAnimatedObject::~CAnimatedObject()
{
	//if (m_AnimationClip) delete m_AnimationClip;

}


void CAnimatedObject::Animate(float fTimeElapsed)
{
	//CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
	//MoveStrafe(GameTimer->GetTimeElapsed() * 10.0f);
	//MoveForward(GameTimer->GetTimeElapsed() * 10.0f); 
}

void CAnimatedObject::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_RESULT_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbAnimation) == S_OK)
		std::cout << "success" << std::endl;

}
void CAnimatedObject::CreateAnimation()
{
	//UINT uiAnimationClipNums = 5;
	m_AnimationClip = new AnimationClip[m_uiAnimationClipNum];
	m_AnimationClip[0].m_nAnimationState = ANIMATAION_CLIP_IDLE;
	m_AnimationClip[1].m_nAnimationState = ANIMATAION_CLIP_ATTACK1;
	m_AnimationClip[2].m_nAnimationState = ANIMATAION_CLIP_ATTACK2;
	m_AnimationClip[3].m_nAnimationState = ANIMATAION_CLIP_RUN;
	m_AnimationClip[4].m_nAnimationState = ANIMATAION_CLIP_DEATH;

	m_AnimationClip[0].m_strFileName = "StoneKing_AttackReady.txt";
	m_AnimationClip[1].m_strFileName = "StoneKing_Attack1.txt";
	m_AnimationClip[2].m_strFileName = "StoneKing_Attack2.txt";
	m_AnimationClip[3].m_strFileName = "StoneKing_Run.txt";
	m_AnimationClip[4].m_strFileName = "StoneKing_Death.txt";

	for (int i = 0; i < m_uiAnimationClipNum; i++)
	{
		LoadAnimation( i);
		//cout << i << "번째 로딩 성공" << endl;
	}
	//return animationClip;
}
void CAnimatedObject::LoadAnimation( UINT k)
{
	FILE *pFile = NULL;
	long long llAniTime = 0;
	string strFileName = "Data/";
	strFileName += m_AnimationClip[k].m_strFileName;
	//_wfopen_s(&pFile, L"Data/firstStepBox001_matrix.txt", L"rt");
	fopen_s(&pFile, strFileName.c_str(), "rt");
	fscanf_s(pFile, "%d \n", &llAniTime);
	fscanf_s(pFile, "%d \n", &m_AnimationClip[k].m_uiBoneIndexCount);
	m_AnimationClip[k].m_llAniTime = llAniTime;

	m_AnimationClip[k].m_ppResultMatrix = new D3DXMATRIX*[m_AnimationClip[k].m_llAniTime / 10];
	m_AnimationClip[k].m_fTimePos = 0.0f;
	m_AnimationClip[k].llNowTime = 0;


	for (long long i = 0; i < m_AnimationClip[k].m_llAniTime / 10; ++i)
	{
		// 최종 행렬 -> 매 초마다 해당하는    (포인터 배열을 가지고 있는)
		m_AnimationClip[k].m_ppResultMatrix[i] = new D3DXMATRIX[m_AnimationClip[k].m_uiBoneIndexCount];
	}
	for (long long i = 0; i <m_AnimationClip[k].m_llAniTime / 10; ++i)
	{
		for (unsigned int j = 0; j <m_AnimationClip[k].m_uiBoneIndexCount; ++j)
		{
			//cout << i << "  " << j << endl;
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_AnimationClip[k].m_ppResultMatrix[i][j]._11, &m_AnimationClip[k].m_ppResultMatrix[i][j]._12, &m_AnimationClip[k].m_ppResultMatrix[i][j]._13, &m_AnimationClip[k].m_ppResultMatrix[i][j]._14);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_AnimationClip[k].m_ppResultMatrix[i][j]._21, &m_AnimationClip[k].m_ppResultMatrix[i][j]._22, &m_AnimationClip[k].m_ppResultMatrix[i][j]._23, &m_AnimationClip[k].m_ppResultMatrix[i][j]._24);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_AnimationClip[k].m_ppResultMatrix[i][j]._31, &m_AnimationClip[k].m_ppResultMatrix[i][j]._32, &m_AnimationClip[k].m_ppResultMatrix[i][j]._33, &m_AnimationClip[k].m_ppResultMatrix[i][j]._34);
			fscanf_s(pFile, "%f %f %f %f  \n",
				&m_AnimationClip[k].m_ppResultMatrix[i][j]._41, &m_AnimationClip[k].m_ppResultMatrix[i][j]._42, &m_AnimationClip[k].m_ppResultMatrix[i][j]._43, &m_AnimationClip[k].m_ppResultMatrix[i][j]._44);
		}
	}
	::fclose(pFile);
}


void  CAnimatedObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	//CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
	//서버 애니메이션 상태 업데이트
	//플레이어의 상태값은 struct에 PlayerState자료형이다.
	ClientServer *s = ClientServer::getInstangce();
	//**********************아래거 주석할것.
	//for (auto i = 1; i < ROOM_MAX_PLAYER;++i)
	for (int i = 0; i < m_uiAnimationClipNum; i++)
	{
		if (m_AnimationClip[i].m_nAnimationState == m_nAnimationState)
		{
			CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
			m_AnimationClip[i].m_fTimePos += GameTimer->GetTimeElapsed();


			m_AnimationClip[i].llNowTime = m_AnimationClip[i].m_fTimePos * 1000;
			//애니메이션 재생시간이 총 재생 시간을 초과하면.
			if (m_AnimationClip[i].llNowTime >= m_AnimationClip[i].m_llAniTime)
			{
				m_AnimationClip[i].llNowTime -= m_AnimationClip[i].m_llAniTime;
				m_AnimationClip[i].m_fTimePos = 0;

				// 
				if (m_nAnimationState == ANIMATAION_CLIP_ATTACK1
					|| m_nAnimationState == ANIMATAION_CLIP_ATTACK2
					|| m_nAnimationState == ANIMATAION_CLIP_DEATH)
					m_nAnimationState = ANIMATAION_CLIP_IDLE;
			}

			D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
			pd3dDeviceContext->Map(m_pd3dcbAnimation, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
			m_cbMapData = (VS_CB_RESULT_MATRIX *)d3dMappedResource.pData;

			for (int j = 0; j < MaxBone; j++) //왼쪽
				m_cbMapData->m_d3dxmtxResult[j] = m_AnimationClip[i].m_ppResultMatrix[m_AnimationClip[i].llNowTime / 10][j];
			pd3dDeviceContext->Unmap(m_pd3dcbAnimation, 0);


			if (m_pd3dcbAnimation != NULL)
				pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_RESULT_MATRIX, 1, &m_pd3dcbAnimation);
		}
	}

	CGameObject::Render(pd3dDeviceContext, pCamera);
	//if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);
}

COtherPlayerObject::COtherPlayerObject(ID3D11Device *pd3dDevice, string strFileName) : CAnimatedObject(pd3dDevice,  strFileName)
{
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	m_pTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/MovingObject/StoneKing.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	m_pTexture->SetSampler(0, pd3dSamplerState);

	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();

	CreateShaderVariables(pd3dDevice);
	//AnimationClip* animationClip = NULL;
	m_uiAnimationClipNum = 5;
	 CreateAnimation();
}

COtherPlayerObject::~COtherPlayerObject()
{
	if (m_pd3dcbAnimation)m_pd3dcbAnimation->Release();

	m_AnimationClip->ReleaseObjects();
	if (m_AnimationClip)
		delete m_AnimationClip;
}
void  COtherPlayerObject::CreateAnimation()
{
	//UINT uiAnimationClipNums = 5;
	m_AnimationClip = new AnimationClip[m_uiAnimationClipNum];
	m_AnimationClip[0].m_nAnimationState = ANIMATAION_CLIP_IDLE;
	m_AnimationClip[1].m_nAnimationState = ANIMATAION_CLIP_ATTACK1;
	m_AnimationClip[2].m_nAnimationState = ANIMATAION_CLIP_ATTACK2;
	m_AnimationClip[3].m_nAnimationState = ANIMATAION_CLIP_RUN;
	m_AnimationClip[4].m_nAnimationState = ANIMATAION_CLIP_DEATH;

	m_AnimationClip[0].m_strFileName = "StoneKing_AttackReady.txt";
	m_AnimationClip[1].m_strFileName = "StoneKing_Attack1.txt";
	m_AnimationClip[2].m_strFileName = "StoneKing_Attack2.txt";
	m_AnimationClip[3].m_strFileName = "StoneKing_Run.txt";
	m_AnimationClip[4].m_strFileName = "StoneKing_Death.txt";

	for (int i = 0; i < m_uiAnimationClipNum; i++)
	{
		LoadAnimation( i);
		//cout << i << "번째 로딩 성공" << endl;
	}
	//return animationClip;
}

CMonsterObject::CMonsterObject(ID3D11Device *pd3dDevice, string strFileName) : CAnimatedObject(pd3dDevice, strFileName)
{
	//string strFileName = "Data/Golem_Vertex.txt";


	/*CCharacterMesh*pCCharacterMesh = new CCharacterMesh(pd3dDevice, strFileName);
	SetMesh(pCCharacterMesh, 0);*/
	//CBoundingSphere* pHumanMesh = new CBoundingSphere(pd3dDevice, 20.0f, 10.0f, 10.0f);
	//SetMesh(pHumanMesh, 1);
	//Scale(D3DXVECTOR3(0.01, 0.01, 0.01));

	/*m_fAttackRadius = 140.0f;
	m_fBeAttackedRadius = 60.0f;
	m_nAnimationState = 2;
	CreateShaderVariables(pd3dDevice);
	m_uiAnimationClipNum = 4;
	m_bMonsterState = true;
	CreateAnimation();*/
}



void  CMonsterObject::CreateAnimation()
{
	////UINT uiAnimationClipNums = 5;
	//m_AnimationClip = new AnimationClip[m_uiAnimationClipNum];
	//m_AnimationClip[0].m_nAnimationState = ANIMATAION_CLIP_IDLE;
	//m_AnimationClip[1].m_nAnimationState = ANIMATAION_CLIP_RUN;
	//m_AnimationClip[2].m_nAnimationState = ANIMATAION_CLIP_ATTACK1;
	//m_AnimationClip[3].m_nAnimationState = 3;  // 매칭이 안맞는다.

	////animationClip[4].m_nAnimationState = ANIMATAION_CLIP_DEATH;

	//m_AnimationClip[0].m_strFileName = "GolemIdle.txt";
	//m_AnimationClip[1].m_strFileName = "GolemWalk_matrix.txt";
	//m_AnimationClip[2].m_strFileName = "GolemAttack.txt";
	//m_AnimationClip[3].m_strFileName = "GolemDeath.txt";
	//

	//m_AnimationClip[2].m_fAttackStartTime = 628;
	//m_AnimationClip[2].m_fAttackEndTime = 861;

	////animationClip[4].m_strFileName = "StoneKing_Death.txt";

	//for (int i = 0; i < m_uiAnimationClipNum; i++)
	//{
	//	LoadAnimation( i);
	//	
	//}

}
CMonsterObject ::~CMonsterObject()
{
	
}



void CMonsterObject::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_RESULT_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//D3D11_SUBRESOURCE_DATA d3dBufferData;
	//ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));

	//VS_CB_MONSTER_RESULT_MATRIX monsterMatrice;

	//m_AnimationClip[0].m_ppResultMatrix;

	//d3dBufferData.pSysMem = 

	if (pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbAnimation) == S_OK)
		std::cout << "success" << std::endl;

}

// 누적된 시간에 대해 애니메이션이 재생된다.
// 즉, 현재 상태에 대해서만 시간을 누적시켜 상수버퍼에 보내고
// 재생되지 않는 애니메이션은 시간을 누적시키지 않고, 상수 버퍼에 보낸다.

//*** 해당 애니메이션이 아닌데 이걸 왜 갱신 시킴????

// 1번 애니네이션(걷는)만 재생이된다
// 하지만, 상태값들은 모두 1이 아니다.
// 1번만 재생됬다는것은 1번만 갱신이 되었다는 뜻이다.

void  CMonsterObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	if (m_bMonsterState == true)
	{
		for (int i = 0; i < m_uiAnimationClipNum; i++)
		{
			if (m_AnimationClip[i].m_nAnimationState == m_nAnimationState)
			{
				CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
				m_AnimationClip[i].m_fTimePos += GameTimer->GetTimeElapsed();
				m_AnimationClip[i].llNowTime = m_AnimationClip[i].m_fTimePos * 1000;

				if (m_AnimationClip[i].llNowTime >= m_AnimationClip[i].m_llAniTime)
				{
					m_AnimationClip[i].llNowTime -= m_AnimationClip[i].m_llAniTime;
					m_AnimationClip[i].m_fTimePos = 0;
					if (m_nAnimationState == 3)
						m_bMonsterState = false;
					// 
					/*if (m_nAnimationState == ANIMATAION_CLIP_ATTACK1
					|| m_nAnimationState == ANIMATAION_CLIP_ATTACK2
					|| m_nAnimationState == ANIMATAION_CLIP_DEATH)
					m_nAnimationState = ANIMATAION_CLIP_IDLE;*/
				}
				D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
				pd3dDeviceContext->Map(m_pd3dcbAnimation, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
				m_cbMonsterMatrice = (VS_CB_RESULT_MATRIX *)d3dMappedResource.pData;
				for (int j = 0; j < MaxBone; j++) //왼쪽
				{
					m_cbMonsterMatrice->m_d3dxmtxResult[j] = m_AnimationClip[i].m_ppResultMatrix[m_AnimationClip[i].llNowTime / 10][j];
					/*	m_cbMonsterMatrice->m_d3dxmtxWalk[j] =   m_AnimationClip[1].m_ppResultMatrix[m_AnimationClip[1].llNowTime / 10][j];
					m_cbMonsterMatrice->m_d3dxmtxAttack[j] = m_AnimationClip[2].m_ppResultMatrix[m_AnimationClip[2].llNowTime / 10][j];
					m_cbMonsterMatrice->m_d3dxmtxDeath[j] =  m_AnimationClip[3].m_ppResultMatrix[m_AnimationClip[3].llNowTime / 10][j];*/
				}
				pd3dDeviceContext->Unmap(m_pd3dcbAnimation, 0);

				if (m_pd3dcbAnimation != NULL)
					pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_RESULT_MATRIX, 1, &m_pd3dcbAnimation);
			}
		}
		CGameObject::Render(pd3dDeviceContext, pCamera);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
		pd3dDeviceContext->Map(m_pd3dcbAnimation, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		m_cbMonsterMatrice = (VS_CB_RESULT_MATRIX *)d3dMappedResource.pData;

		for (int j = 0; j < MaxBone; j++) //왼쪽
			m_cbMonsterMatrice->m_d3dxmtxResult[j] = m_AnimationClip[3].m_ppResultMatrix[165][j];
		pd3dDeviceContext->Unmap(m_pd3dcbAnimation, 0);



		if (m_pd3dcbAnimation != NULL)
			pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_RESULT_MATRIX, 1, &m_pd3dcbAnimation);
		CGameObject::Render(pd3dDeviceContext, pCamera);	
	}
}
//140 30  (골렘, 플레이어 공격 충돌 반경)
float CMonsterObject::CalculateDistance(D3DXVECTOR3 d3dxvPosition1, D3DXVECTOR3 d3dxvPosition2)
{
	float fDistnace = ((d3dxvPosition1.x - d3dxvPosition2.x) * (d3dxvPosition1.x - d3dxvPosition2.x)
		+ (d3dxvPosition1.z - d3dxvPosition2.z) * (d3dxvPosition1.z - d3dxvPosition2.z));
	return fDistnace;
}
float CMonsterObject::CalculateAttackRange(float fAttackRadius, float fBeAttackedRadius)
{
	return (fAttackRadius + fBeAttackedRadius) *  (fAttackRadius + fBeAttackedRadius);
}
bool CMonsterObject :: CalculateCollisionRange(D3DXVECTOR3 d3dxvPlayerPosition)
{
	// 몬스터 - 플레이어 = 이런 벡터임
	D3DXVECTOR3 d3dxvLookPlayer = d3dxvPlayerPosition - GetPosition();
	D3DXVec3Normalize(&d3dxvLookPlayer, &d3dxvLookPlayer);
	
	float fAngle = D3DXVec3Dot(&GetLookAt(), &d3dxvLookPlayer);
	float a = (float)acos((double)fAngle);
	float b = D3DXToDegree(a);
	if (b <= 10.0)
		return true;
	else
		return false;
}
// m_AnimationClip[2].llNowTime >=628
void CMonsterObject::CollisionCheck(CGameObject** ppGameObject)
{
	// 28900.0
	// 공격 모션일때 충돌 처리가 일어난다 해. 22~26프레임일때
	// 플레이어를 공격했을 때.
	if (m_nAnimationState == ANIMATAION_CLIP_ATTACK1
		&& CalculateDistance(ppGameObject[0]->GetPosition(), GetPosition()) <= CalculateAttackRange(GetAttackRadius(),ppGameObject[0]->GetBeAttackedRadius())
				&&CalculateCollisionRange(ppGameObject[0]->GetPosition()))
	{
		if (m_AnimationClip[2].m_fAttackStartTime <= m_AnimationClip[2].llNowTime && 
			m_AnimationClip[2].llNowTime <= m_AnimationClip[2].m_fAttackEndTime)
			cout << "아픔" << endl;
	}
	//if (628 <= m_AnimationClip[2].llNowTime && m_AnimationClip[2].llNowTime <= 861)
}
// 1160
// 628일 때 충돌 처리.
void CMonsterObject::Animate(float fTimeElapsed)
{
	CGameManager* pGameManager = CGameManager::GetCGameManager();
	CollisionCheck(pGameManager->m_pPlayers);
}




CGolemObject::CGolemObject(ID3D11Device *pd3dDevice, string strFileName) :CMonsterObject(pd3dDevice, strFileName)
{
	m_fAttackRadius = 140.0f;
	m_fBeAttackedRadius = 60.0f;
	m_nAnimationState = 2;
	CreateShaderVariables(pd3dDevice);
	m_uiAnimationClipNum = 4;
	m_bMonsterState = true;
	CreateAnimation();
}



void  CGolemObject::CreateAnimation()
{
	//UINT uiAnimationClipNums = 5;
	m_AnimationClip = new AnimationClip[m_uiAnimationClipNum];
	m_AnimationClip[0].m_nAnimationState = ANIMATAION_CLIP_IDLE;
	m_AnimationClip[1].m_nAnimationState = ANIMATAION_CLIP_RUN;
	m_AnimationClip[2].m_nAnimationState = ANIMATAION_CLIP_ATTACK1;
	m_AnimationClip[3].m_nAnimationState = 3;  // 매칭이 안맞는다.

	//animationClip[4].m_nAnimationState = ANIMATAION_CLIP_DEATH;

	m_AnimationClip[0].m_strFileName = "GolemIdle.txt";
	m_AnimationClip[1].m_strFileName = "GolemWalk_matrix.txt";
	m_AnimationClip[2].m_strFileName = "GolemAttack.txt";
	m_AnimationClip[3].m_strFileName = "GolemDeath.txt";


	m_AnimationClip[2].m_fAttackStartTime = 628;
	m_AnimationClip[2].m_fAttackEndTime = 861;

	//animationClip[4].m_strFileName = "StoneKing_Death.txt";

	for (int i = 0; i < m_uiAnimationClipNum; i++)
	{
		LoadAnimation(i);
		//cout << i << "번째 로딩 성공" << endl;
	}
	//	return animationClip;
}
CGolemObject ::~CGolemObject()
{

}




CLeavesObject::CLeavesObject(ID3D11Device *pd3dDevice, string strFileName) : CGameObject(1)
{
}
CLeavesObject::~CLeavesObject()
{
}



CWoodObject::CWoodObject(ID3D11Device *pd3dDevice, string strFileName) : CGameObject(1)
{
	//string strFileName = "Data/NewTree02_Vertex.txt";

	//ID3D11SamplerState *pd3dSamplerState = NULL;
	//D3D11_SAMPLER_DESC d3dSamplerDesc;
	//ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	//d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	//d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	//d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	//d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//d3dSamplerDesc.MinLOD = 0;
	//d3dSamplerDesc.MaxLOD = 0;
	//pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	////텍스쳐 리소스를 생성한다.
	//ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	//m_pTexture = new CTexture(1, 1, 0, 0);
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Tree 3-4 Diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	//m_pTexture->SetTexture(0, pd3dsrvTexture);
	//m_pTexture->SetSampler(0, pd3dSamplerState);
	//pd3dsrvTexture->Release();

	//pd3dSamplerState->Release();


	//CFixedMesh *pFixedMesh = new CFixedMesh(pd3dDevice, strFileName);
	//SetMesh(pFixedMesh, 0);
	//Scale(D3DXVECTOR3(0.03, 0.03, 0.03));


}
CWoodObject ::~CWoodObject()
{
}

CBoundingBoxObject::CBoundingBoxObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	// 나무01 53.8551f, 73.0353f, 51.4314f
	// 물 490.0f, 73.0353f, 500.0f
	// 캐릭어 14.0f, 60.0f, 14.0f);

	//CBoundingMesh* pHumanMesh = new CBoundingMesh(pd3dDevice, 140.0f, 14.0f, 30.0f);


	/*CBoundingCircle* pHumanMesh = new CBoundingCircle(pd3dDevice, 140.0f, 20.0f, 1.0f);
	SetMesh(pHumanMesh, 0);*/
}

CBoundingBoxObject::~CBoundingBoxObject()
{
}

