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
	for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i] = NULL;
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
{
	CWaveMesh *pWaveMesh = new  CWaveMesh(pd3dDevice, 80.0, 80.0, 300.0f, 50.0f, 300.0f);
	SetMesh(pWaveMesh, 0);
	m_pStonesTexture = NULL;
	m_pWavesNormalMap1 = NULL;
	m_pWavesNormalMap2 = NULL;
	m_pMaterial = NULL;
	CreateTextures(pd3dDevice);
	CreateWaveConsBuffer(pd3dDevice);
	CreateBlendingState(pd3dDevice);
	m_pMaterial = new CMaterial();

	m_pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f);
	m_pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.4f, 0.5f, 0.7f, 1.0f);
	m_pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 4.0f);
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
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/waves1.dds"), NULL, NULL, &pd3dsrvTexture, NULL);
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
	m_WavesDispOffset1.x += 0.01f*fTimeElapsed;
	m_WavesDispOffset1.y += 0.03f*fTimeElapsed;

	m_WavesDispOffset2.x += 0.01f*fTimeElapsed;
	m_WavesDispOffset2.y += 0.03f*fTimeElapsed;

	D3DXMATRIX d3dxmtxWaveScale1; 
	D3DXMatrixScaling(&d3dxmtxWaveScale1, 2.0f, 2.0f, 1.0f);
	D3DXMATRIX d3dxmtxWaveOffset1;
	D3DXMatrixTranslation(&d3dxmtxWaveOffset1, m_WavesDispOffset1.x, m_WavesDispOffset1.y, 0.0f);
	

	// S * R * T 
	D3DXMatrixMultiply(&m_d3dxmtxWavesDispTexTransform1, &d3dxmtxWaveScale1, &d3dxmtxWaveOffset1);

	D3DXMATRIX d3dxmtxWaveScale2;
	D3DXMatrixScaling(&d3dxmtxWaveScale2, 2.0f, 2.0f, 1.0f);
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
	D3DXMatrixTranslation(&d3dxmtxWaveOffset1, m_WavesDispOffset1.x, m_WavesDispOffset1.y, 0.0f);
	D3DXMatrixMultiply(&m_d3dxmtxWavesNormalTexTransform1, &d3dxmtxWaveScale1, &d3dxmtxWaveOffset1);


	D3DXMatrixScaling(&d3dxmtxWaveScale2, 16.0f, 16.0f, 1.0f);
	D3DXMatrixTranslation(&d3dxmtxWaveOffset2, m_WavesDispOffset2.x, m_WavesDispOffset2.y, 0.0f);
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


CWizardObject::CWizardObject(ID3D11Device *pd3dDevice, string strFileName) : CGameObject(1)
{
	//string strFileName = "Data/walk02Box001";


	CCharacterMesh *pHumanMesh = new CCharacterMesh(pd3dDevice, strFileName);
	//pHumanMesh->CreateChileMesh(pd3dDevice);
	SetMesh(pHumanMesh, 0);
	// Rotate(-90.0, 0.0, 0.0);
	// Scale(D3DXVECTOR3(0.8, 0.8, 0.8));
}
CWizardObject::~CWizardObject()
{
}

void CWizardObject::Animate(float fTimeElapsed)
{
	//CGameTimer* GameTimer = CGameTimer::GetCGameTimer();
	//MoveStrafe(GameTimer->GetTimeElapsed() * 10.0f);
	//MoveForward(GameTimer->GetTimeElapsed() * 10.0f); 
}

void  CWizardObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
	if (m_ppMeshes && m_ppMeshes[0]) m_ppMeshes[0]->Render(pd3dDeviceContext);
}


CTreeObject::CTreeObject(ID3D11Device *pd3dDevice) : CGameObject(1)
{
	string strFileName = "Data/Cliff1_vertex.txt";


	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	//텍스쳐 리소스를 생성한다.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL; //Stone Brick
	m_pTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Image/Cliff-Diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	m_pTexture->SetTexture(0, pd3dsrvTexture);
	m_pTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();

	pd3dSamplerState->Release();


	CFixedMesh *pFixedMesh = new CFixedMesh(pd3dDevice, strFileName);
	//pHumanMesh->CreateChileMesh(pd3dDevice);
	SetMesh(pFixedMesh, 0);

	//Scale(D3DXVECTOR3(7.2, 7.2, 7.2));

	Rotate(-90.0, 90.0, 0.0);
	
}
CTreeObject::~CTreeObject()
{
}

