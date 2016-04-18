#include "HeightMap.h"


CHeightMap::CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	BYTE *pHeightMapImage = new BYTE[m_nWidth * m_nLength];

	//파일을 열고 읽는다. 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다.
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapImage, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	/*이미지의 y-축과 지형의 z-축이 방향이 반대이므로 이미지를 상하대칭 시켜 저장한다. 
	그러면 <그림 7>과 같이 이미지의 좌표축과 지형의 좌표축의 방향이 일치하게 된다.*/
	m_pHeightMapImage = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapImage[x + ((m_nLength - 1 - y)*m_nWidth)] = pHeightMapImage[x + (y*m_nWidth)];
		}
	}

	if (pHeightMapImage) 
		delete[] pHeightMapImage;
}
CHeightMap::~CHeightMap()
{
	if (m_pHeightMapImage) delete[] m_pHeightMapImage;
	m_pHeightMapImage = NULL;
}

D3DXVECTOR3 CHeightMap::GetHeightMapNormal(int x, int z)
{
	//지형의 x-좌표와 z-좌표가 지형(높이 맵)의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터이다.
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) 
		return(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	/*높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한 픽셀 값을 
	   사용하여 법선 벡터를 계산한다.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -(signed)m_nWidth;
	//(x, z), (x+1, z), (z, z+1)의 지형의 높이 값을 구한다.
	float y1 = (float)m_pHeightMapImage[nHeightMapIndex] * m_d3dxvScale.y;
	float y2 = (float)m_pHeightMapImage[nHeightMapIndex + xHeightMapAdd] * m_d3dxvScale.y;
	float y3 = (float)m_pHeightMapImage[nHeightMapIndex + zHeightMapAdd] * m_d3dxvScale.y;

	//vEdge1은 (0, y3, m_vScale.z) - (0, y1, 0) 벡터이다.
	D3DXVECTOR3 vEdge1 = D3DXVECTOR3(0.0f, y3 - y1, m_d3dxvScale.z);
	//vEdge2는 (m_vScale.x, y2, 0) - (0, y1, 0) 벡터이다.
	D3DXVECTOR3 vEdge2 = D3DXVECTOR3(m_d3dxvScale.x, y2 - y1, 0.0f);
	//법선 벡터는 vEdge1과 vEdge2의 외적을 정규화하면 된다.
	D3DXVECTOR3 vNormal;
	D3DXVec3Cross(&vNormal, &vEdge1, &vEdge2);
	D3DXVec3Normalize(&vNormal, &vNormal);
	return(vNormal);
}

float CHeightMap::GetHeight(float fx, float fz, bool bReverseQuad)
{
	//지형의 좌표 (fx, fz)에서 높이 맵의 좌표를 계산한다.
	fx = fx / m_d3dxvScale.x;
	fz = fz / m_d3dxvScale.z;
	//높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 높이는 0이다.
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.
	int x = (int)fx, z = (int)fz;
	float fxPercent = fx - x, fzPercent = fz - z;

	float fTopLeft = m_pHeightMapImage[x + (z*m_nWidth)];
	float fTopRight = m_pHeightMapImage[(x + 1) + (z*m_nWidth)];
	float fBottomLeft = m_pHeightMapImage[x + ((z + 1)*m_nWidth)];
	float fBottomRight = m_pHeightMapImage[(x + 1) + ((z + 1)*m_nWidth)];

	if (bReverseQuad)
	{
		/*지형의 삼각형들이 오른쪽에서 왼쪽 방향으로 나열되는 경우이다. <그림 12>의 오른쪽은 (fzPercent < fxPercent)인 경우이다. 이 경우 TopLeft의 픽셀 값은 (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))로 근사한다. <그림 12>의 왼쪽은 (fzPercent ≥ fxPercent)인 경우이다. 이 경우 BottomRight의 픽셀 값은 (fBottomRight = fBottomLeft + (fTopRight - fTopLeft))로 근사한다.*/
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		/*지형의 삼각형들이 왼쪽에서 오른쪽 방향으로 나열되는 경우이다. <그림 13>의 왼쪽은 (fzPercent < (1.0f - fxPercent))인 경우이다. 
		이 경우 TopRight의 픽셀 값은 (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))로 
		근사한다. <그림 13>의 오른쪽은 (fzPercent ≥ (1.0f - fxPercent))인 경우이다. 
		이 경우 BottomLeft의 픽셀 값은 (fBottomLeft = fTopLeft + (fBottomRight - fTopRight))로 
		근사한다.*/

		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);
}