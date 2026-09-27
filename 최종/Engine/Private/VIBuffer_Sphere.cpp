#include "VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device, _uint _Sphere_Slices, _uint _Sphere_Stacks)
	: CVIBuffer(pGraphic_Device),
	m_Sphere_Slices(_Sphere_Slices),
	m_Sphere_Stacks(_Sphere_Stacks)
{

}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs)
	: CVIBuffer(rhs),
	m_Sphere_Slices(rhs.m_Sphere_Slices),
	m_Sphere_Stacks(rhs.m_Sphere_Stacks)
{

}

HRESULT CVIBuffer_Sphere::Initialize_Prototype()
{
	
	m_iNumVertices = (m_Sphere_Slices * m_Sphere_Stacks) + 2;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1)*/;
	m_dwPrimitive = (m_Sphere_Slices * 2) + ((m_Sphere_Stacks - 1) * (m_Sphere_Slices * 2));

	m_iIndexStride = m_iNumVertices >= 65535 ? 4 : 2;
	m_iNumIndices = m_dwPrimitive * 3;
	m_eIndexFormat = m_iIndexStride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;

	m_Angle = (_float)(360.f / m_Sphere_Slices);
	m_One_Radius = (_float)((m_Sphere_Radius * 2.f) / (m_Sphere_Stacks + 1));

	//피킹을 위한 정점의 정보를 담을 공간을 할당한다.
	m_pVerticesPos = new _float3[m_iNumVertices];

#pragma region VERTEX_BUFFER
	/* 정점 여섯개의 배열 공간을 할당한다. */
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	/* 할당된 공간의 가장 앞 주소를 얻어온다. */
	VTXPOSTEX* pVertices = { nullptr };

	/* 할당된 정점 배열공간의 가장 앞 주소를 얻어올 수 있따. */
	/* 이 공간을 걸어 잠근다. */
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	// 처음 정점과 끝 정점
	//pVertices[0].vPosition = _float3(0.f, 0.f, -m_Sphere_Radius);
	pVertices[0].vPosition = m_pVerticesPos[0] = _float3(0.f, 0.f, -1.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.f);
	//pVertices[m_iNumVertices -1].vPosition = _float3(0.f, 0.f, m_Sphere_Radius);
	pVertices[m_iNumVertices -1].vPosition = m_pVerticesPos[m_iNumVertices - 1] = _float3(0.f, 0.f, 1.f);
	pVertices[m_iNumVertices -1].vTexcoord = _float2(1.0f, 1.0f);

	_float VeferX = 0.f;
	_float VeferY = 0.f;
	_float VeferZ = 0.f;
	_uint iVertexNumber = 0;
	_float fGoSlow = 0.f;

	// 홀수일 때
	if(m_Sphere_Stacks % 2 !=0)
	{
		//m_One_Radius = (_float)((m_Sphere_Radius * 2.f) / (m_Sphere_Stacks+1));
		m_One_Radius = 2.f / (m_Sphere_Stacks+1);
		
		// 맨 끝쪽과 정 가운데를 제외한 나머지 정점을 찍음
		for (_uint i = 1; i <= (m_Sphere_Stacks / 2); ++i)
		{
 			//VeferZ = m_Sphere_Radius - ((m_One_Radius * i)/ m_Sphere_Radius);
			//VeferZ = m_Sphere_Radius - ((m_One_Radius * i));
			VeferZ = 1 - ((m_One_Radius * i));
			
			for (_uint j = 0; j < m_Sphere_Slices; ++j)
			{
				++iVertexNumber;
				//루프마다 그릴 원의 반지름.
				//_float fX = m_Sphere_Radius -  m_One_Radius * i;
				_float fX = 1.f -  m_One_Radius * i;
				_float fRadius = (_float)sqrt(1.f - fX * fX);


				VeferX = cosf(D3DXToRadian(m_Angle * j)) * fRadius;
				VeferY = sinf(D3DXToRadian(m_Angle * j)) * fRadius;
				//VeferX = sinf(D3DXToRadian(m_Angle * j)) * (m_One_Radius * i);
				//VeferY = cosf(D3DXToRadian(m_Angle * j)) * (m_One_Radius * i);

				// 끝쪽부터 채운다.
				pVertices[iVertexNumber + ((i - 1) * m_Sphere_Slices)].vPosition = m_pVerticesPos[iVertexNumber + ((i - 1) * m_Sphere_Slices)] = _float3(VeferX, VeferY, -VeferZ);
				pVertices[iVertexNumber + ((i - 1) * m_Sphere_Slices)].vTexcoord = _float2((_float)(j * 0.3f), (_float)(i  * 0.3f));
				pVertices[(iVertexNumber + (((m_Sphere_Stacks - i) * m_Sphere_Slices)))].vPosition = m_pVerticesPos[(iVertexNumber + (((m_Sphere_Stacks - i) * m_Sphere_Slices)))] = _float3(VeferX, VeferY, VeferZ);
				pVertices[(iVertexNumber + (((m_Sphere_Stacks - i) * m_Sphere_Slices)))].vTexcoord = _float2((_float)(j *0.3f), (_float)(i * 0.3f));

				/*pVertices[iVertexNumber + ((i - 1) * m_Sphere_Slices)].vPosition = m_pVerticesPos[iVertexNumber + ((i - 1) * m_Sphere_Slices)] = _float3(VeferX, VeferY, -VeferZ);
				pVertices[iVertexNumber + ((i - 1) * m_Sphere_Slices)].vTexcoord = _float2((_float)(j/ m_Sphere_Slices-1), (_float)(i / (m_Sphere_Stacks / 2)));
				pVertices[(iVertexNumber + (((m_Sphere_Stacks - i) * m_Sphere_Slices)))].vPosition = m_pVerticesPos[(iVertexNumber + (((m_Sphere_Stacks - i) * m_Sphere_Slices)))] = _float3(VeferX, VeferY, VeferZ);
				pVertices[(iVertexNumber + (((m_Sphere_Stacks - i) * m_Sphere_Slices)))].vTexcoord = _float2((_float)(j / m_Sphere_Slices - 1), (_float)(i / (m_Sphere_Stacks / 2)));*/
			}
			
			iVertexNumber = 0;
		}

		// 정 가운데의 정점을 찍음
		for (_uint j = 0; j < m_Sphere_Slices; ++j)
		{
			//VeferX = cosf(D3DXToRadian(m_Angle * j)) * (m_Sphere_Radius);

			//VeferY = sinf(D3DXToRadian(m_Angle * j)) * (m_Sphere_Radius);
			VeferX = cosf(D3DXToRadian(m_Angle * j));
			VeferY = sinf(D3DXToRadian(m_Angle * j));
			pVertices[((((m_Sphere_Stacks / 2)) * m_Sphere_Slices) + (j + 1))].vPosition = m_pVerticesPos[((((m_Sphere_Stacks / 2)) * m_Sphere_Slices) + (j + 1))] = _float3(VeferX, VeferY, 0.f);
			pVertices[((((m_Sphere_Stacks / 2)) * m_Sphere_Slices) + (j + 1))].vTexcoord = _float2(j, 0.5f);
		}


	}
		// 짝수일 때
	else
	{
		m_One_Radius = (_float)((m_Sphere_Radius * 2.f) / (m_Sphere_Stacks));
		// 맨 끝쪽과 정 가운데를 제외한 나머지 정점을 찍음
		for (_uint i = 1; i <= (m_Sphere_Stacks / 2); ++i)
		{
			VeferZ = m_Sphere_Radius - (m_One_Radius * i); 
			for (_uint j = 0; j < m_Sphere_Slices; ++j)
			{
				++iVertexNumber;
				VeferX = cosf(D3DXToRadian(m_Angle * j)) * (m_One_Radius * i);
				VeferY = sinf(D3DXToRadian(m_Angle * j)) * (m_One_Radius * i);

				// 끝쪽부터 채운다.
				pVertices[iVertexNumber].vPosition = _float3(VeferX, VeferY, -VeferZ);
				pVertices[iVertexNumber].vTexcoord = _float2(1.0f, 0.f);
				pVertices[(iVertexNumber + ((m_Sphere_Stacks - i) * m_Sphere_Slices))].vPosition = _float3(VeferX, VeferY, VeferZ);
				pVertices[(iVertexNumber + ((m_Sphere_Stacks - i) * m_Sphere_Slices))].vTexcoord = _float2(1.0f, 0.f);
			}

		}
	}

	//pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	//pVertices[1].vTexcoord = _float2(1.0f, 0.f);

	//pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	//pVertices[2].vTexcoord = _float2(1.0f, 1.f);

	//pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	//pVertices[3].vTexcoord = _float2(0.0f, 1.f);

	m_pVB->Unlock();

#pragma endregion


#pragma region INDEX_BUFFER

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	if (2 == m_iIndexStride)
	{
		_ushort* pIndices = { nullptr };

		//인덱스 순서를 저장하기 위한 공간을 할당.
		_ushort* pIndicesInfo = new _ushort[m_iNumIndices];

		m_pIB->Lock(0, 0, (void**)&pIndices, 0);
		_ushort iIndexNumber = 0;
		// 처음과 끝면 먼저 폴리곤 만들기
		for (_ushort i = 1; i <= m_Sphere_Slices; ++i)
		{

			if (i != m_Sphere_Slices)
			{
				// 앞면
				pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = 0;
				pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = i;
				pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = i + 1;

				// 끝면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = (m_iNumVertices - 1);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - i);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - i - 1);
				++iIndexNumber;
			}

			else
			{
				// 앞면
				pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = 0;
				pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = i;
				pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = 1;

				// 끝면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = (m_iNumVertices - 1);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - i);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - 1);
				++iIndexNumber;
			}
		}

		// 안쪽 폴리곤 만들기
		for (_ushort i = 0; i < (m_Sphere_Stacks / 2); ++i) // 스택
		{
			for (_ushort j = 1; j <= m_Sphere_Slices; ++j) // 슬라이스
			{
				if (j != m_Sphere_Slices)
				{
					// 앞면 윗면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i + 1) * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = (((i + 1) * m_Sphere_Slices) + j + 1);
					//++iIndexNumber;

					// 뒷면 윗면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((((i + 1) * m_Sphere_Slices) + j)));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((((i + 1) * m_Sphere_Slices) + j + 1)));
					++iIndexNumber;

					// 윗면 아랫면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i + 1) * m_Sphere_Slices) + j + 1);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = ((i * m_Sphere_Slices) + j + 1);
					//++iIndexNumber;

					// 뒷면 아랫면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) - (m_Sphere_Slices + 1));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j + 1));
					++iIndexNumber;
				}
				else
				{
					// 앞면 윗면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i + 1) * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = (((i * m_Sphere_Slices) + j) + 1);
					//++iIndexNumber;

					// 뒷면 윗면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - (((i + 1) * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) - 1);
					++iIndexNumber;

					// 윗면 아랫면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i * m_Sphere_Slices) + j) + 1);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = (((i * m_Sphere_Slices) + j) - (m_Sphere_Slices - 1));
					//++iIndexNumber;

					// 뒷면 아랫면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) - 1);
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) + (m_Sphere_Slices - 1));
					++iIndexNumber;
				}
			}
		}
		m_pIndicesInfo = pIndicesInfo;
		m_pIB->Unlock();
	}
	else if(4 == m_iIndexStride)
	{
		_uint* pIndices = { nullptr };

		m_pIB->Lock(0, 0, (void**)&pIndices, 0);
		_uint iIndexNumber = 0;
		_uint* pIndicesInfo = new _uint[m_iNumIndices];

		// 처음과 끝면 먼저 폴리곤 만들기
		for (_uint i = 1; i <= m_Sphere_Slices; ++i)
		{

			if (i != m_Sphere_Slices)
			{
				// 앞면
				pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = 0;
				pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = i;
				pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = i + 1;

				// 끝면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = (m_iNumVertices - 1);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - i);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - i - 1);
				++iIndexNumber;
			}

			else
			{
				// 앞면
				pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = 0;
				pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = i;
				pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = 1;

				// 끝면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = (m_iNumVertices - 1);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - i);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - 1);
				++iIndexNumber;
			}
		}

		// 안쪽 폴리곤 만들기
		for (_uint i = 0; i < (m_Sphere_Stacks / 2); ++i) // 스택
		{
			for (_uint j = 1; j <= m_Sphere_Slices; ++j) // 슬라이스
			{
				if (j != m_Sphere_Slices)
				{
					// 앞면 윗면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i + 1) * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = (((i + 1) * m_Sphere_Slices) + j + 1);
					//++iIndexNumber;

					// 뒷면 윗면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((((i + 1) * m_Sphere_Slices) + j)));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((((i + 1) * m_Sphere_Slices) + j + 1)));
					++iIndexNumber;

					// 윗면 아랫면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i + 1) * m_Sphere_Slices) + j + 1);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = ((i * m_Sphere_Slices) + j + 1);
					//++iIndexNumber;

					// 뒷면 아랫면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) - (m_Sphere_Slices + 1));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j + 1));
					++iIndexNumber;
				}
				else
				{
					// 앞면 윗면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i + 1) * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = (((i * m_Sphere_Slices) + j) + 1);
					//++iIndexNumber;

					// 뒷면 윗면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - (((i + 1) * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) - 1);
					++iIndexNumber;

					// 윗면 아랫면
					pIndices[iIndexNumber * 3] = pIndicesInfo[iIndexNumber * 3] = ((i * m_Sphere_Slices) + j);
					pIndices[iIndexNumber * 3 + 1] = pIndicesInfo[iIndexNumber * 3 + 1] = (((i * m_Sphere_Slices) + j) + 1);
					pIndices[iIndexNumber * 3 + 2] = pIndicesInfo[iIndexNumber * 3 + 2] = (((i * m_Sphere_Slices) + j) - (m_Sphere_Slices - 1));
					//++iIndexNumber;

					// 뒷면 아랫면
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j));
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) - 1);
					pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = pIndicesInfo[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * m_Sphere_Slices) + j) + (m_Sphere_Slices - 1));
					++iIndexNumber;
				}
			}
		}
		m_pIB->Unlock();
	}
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Sphere* CVIBuffer_Sphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint _Sphere_Slices, _uint _Sphere_Stacks)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(pGraphic_Device,  _Sphere_Slices,  _Sphere_Stacks);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVIBuffer_Sphere"));

		Safe_Release(pInstance);
	}

	return pInstance;
}
/* 사본객체를 생성하기위한 함수에요. */
CComponent* CVIBuffer_Sphere::Clone(void* pArg)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVIBuffer_Sphere"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();

}
