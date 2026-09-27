#include "Collider_Sphere.h"

CCollider_Sphere::CCollider_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CCollider(pGraphic_Device)
{
}

CCollider_Sphere::CCollider_Sphere(const CCollider_Sphere& rhs) :
	CCollider(rhs)
{
}

HRESULT CCollider_Sphere::Initialize_Prototype()
{
	m_iNumVertices = (CCOLLIDER_SPHERE_SLICE * CCOLLIDER_SPHERE_STACK) + 2;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1)*/;
	m_dwPrimitive = (CCOLLIDER_SPHERE_SLICE * 2) + ((CCOLLIDER_SPHERE_STACK - 1) * (CCOLLIDER_SPHERE_SLICE * 2));

	m_iIndexStride = m_iNumVertices >= 65535 ? 4 : 2;
	m_iNumIndices = m_dwPrimitive * 3;
	m_eIndexFormat = m_iIndexStride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;

	_float m_fAngle = (_float)(360.f / CCOLLIDER_SPHERE_SLICE);
	_float m_fOne_Radius = (_float)((CCOLLIDER_SPHERE_RADIUS * 2.f) / (CCOLLIDER_SPHERE_STACK + 1));

	//피킹을 위한 정점의 정보를 담을 공간을 할당한다.
	//m_pVerticesPos = new _float3[m_iNumVertices];

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
	//pVertices[0].vPosition = _float3(0.f, 0.f, -CCOLLIDER_SPHERE_RADIUS);
	pVertices[0].vPosition = _float3(0.f, 0.f, -1.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.f);
	//pVertices[m_iNumVertices -1].vPosition = _float3(0.f, 0.f, CCOLLIDER_SPHERE_RADIUS);
	pVertices[m_iNumVertices - 1].vPosition = _float3(0.f, 0.f, 1.f);
	pVertices[m_iNumVertices - 1].vTexcoord = _float2(1.0f, 1.0f);

	_float VeferX = 0.f;
	_float VeferY = 0.f;
	_float VeferZ = 0.f;
	_uint iVertexNumber = 0;
	_float fGoSlow = 0.f;

	// 홀수일 때

	//m_fOne_Radius = (_float)((CCOLLIDER_SPHERE_RADIUS * 2.f) / (CCOLLIDER_SPHERE_STACK+1));
	m_fOne_Radius = 2.f / (CCOLLIDER_SPHERE_STACK + 1);

	// 맨 끝쪽과 정 가운데를 제외한 나머지 정점을 찍음
	for (_uint i = 1; i <= (CCOLLIDER_SPHERE_STACK / 2); ++i)
	{
		//VeferZ = CCOLLIDER_SPHERE_RADIUS - ((m_fOne_Radius * i)/ CCOLLIDER_SPHERE_RADIUS);
		//VeferZ = CCOLLIDER_SPHERE_RADIUS - ((m_fOne_Radius * i));
		VeferZ = 1 - ((m_fOne_Radius * i));

		for (_uint j = 0; j < CCOLLIDER_SPHERE_SLICE; ++j)
		{
			++iVertexNumber;
			//루프마다 그릴 원의 반지름.
			//_float fX = CCOLLIDER_SPHERE_RADIUS -  m_fOne_Radius * i;
			_float fX = 1.f - m_fOne_Radius * i;
			_float fRadius = (_float)sqrt(1.f - fX * fX);


			VeferX = cosf(D3DXToRadian(m_fAngle * j)) * fRadius;
			VeferY = sinf(D3DXToRadian(m_fAngle * j)) * fRadius;
			//VeferX = sinf(D3DXToRadian(m_fAngle * j)) * (m_fOne_Radius * i);
			//VeferY = cosf(D3DXToRadian(m_fAngle * j)) * (m_fOne_Radius * i);

			// 끝쪽부터 채운다.
			pVertices[iVertexNumber + ((i - 1) * CCOLLIDER_SPHERE_SLICE)].vPosition = _float3(VeferX, VeferY, -VeferZ);
			pVertices[iVertexNumber + ((i - 1) * CCOLLIDER_SPHERE_SLICE)].vTexcoord = _float2((_float)(j * 0.3f), (_float)(i * 0.3f));
			pVertices[(iVertexNumber + (((CCOLLIDER_SPHERE_STACK - i) * CCOLLIDER_SPHERE_SLICE)))].vPosition = _float3(VeferX, VeferY, VeferZ);
			pVertices[(iVertexNumber + (((CCOLLIDER_SPHERE_STACK - i) * CCOLLIDER_SPHERE_SLICE)))].vTexcoord = _float2((_float)(j * 0.3f), (_float)(i * 0.3f));

			/*pVertices[iVertexNumber + ((i - 1) * CCOLLIDER_SPHERE_SLICE)].vPosition = m_pVerticesPos[iVertexNumber + ((i - 1) * CCOLLIDER_SPHERE_SLICE)] = _float3(VeferX, VeferY, -VeferZ);
			pVertices[iVertexNumber + ((i - 1) * CCOLLIDER_SPHERE_SLICE)].vTexcoord = _float2((_float)(j/ CCOLLIDER_SPHERE_SLICE-1), (_float)(i / (CCOLLIDER_SPHERE_STACK / 2)));
			pVertices[(iVertexNumber + (((CCOLLIDER_SPHERE_STACK - i) * CCOLLIDER_SPHERE_SLICE)))].vPosition = m_pVerticesPos[(iVertexNumber + (((CCOLLIDER_SPHERE_STACK - i) * CCOLLIDER_SPHERE_SLICE)))] = _float3(VeferX, VeferY, VeferZ);
			pVertices[(iVertexNumber + (((CCOLLIDER_SPHERE_STACK - i) * CCOLLIDER_SPHERE_SLICE)))].vTexcoord = _float2((_float)(j / CCOLLIDER_SPHERE_SLICE - 1), (_float)(i / (CCOLLIDER_SPHERE_STACK / 2)));*/
		}

		iVertexNumber = 0;
	}

	// 정 가운데의 정점을 찍음
	for (_uint j = 0; j < CCOLLIDER_SPHERE_SLICE; ++j)
	{
		//VeferX = cosf(D3DXToRadian(m_fAngle * j)) * (CCOLLIDER_SPHERE_RADIUS);

		//VeferY = sinf(D3DXToRadian(m_fAngle * j)) * (CCOLLIDER_SPHERE_RADIUS);
		VeferX = cosf(D3DXToRadian(m_fAngle * j));
		VeferY = sinf(D3DXToRadian(m_fAngle * j));
		pVertices[((((CCOLLIDER_SPHERE_STACK / 2)) * CCOLLIDER_SPHERE_SLICE) + (j + 1))].vPosition = _float3(VeferX, VeferY, 0.f);
		pVertices[((((CCOLLIDER_SPHERE_STACK / 2)) * CCOLLIDER_SPHERE_SLICE) + (j + 1))].vTexcoord = _float2(1.0f, 0.f);
	}
	m_pVB->Unlock();

#pragma endregion


#pragma region INDEX_BUFFER

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;


	_ushort* pIndices = { nullptr };

	//인덱스 순서를 저장하기 위한 공간을 할당.
	//_ushort* pIndicesInfo = new _ushort[m_iNumIndices];

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);
	_ushort iIndexNumber = 0;
	// 처음과 끝면 먼저 폴리곤 만들기
	for (_ushort i = 1; i <= CCOLLIDER_SPHERE_SLICE; ++i)
	{

		if (i != CCOLLIDER_SPHERE_SLICE)
		{
			// 앞면
			pIndices[iIndexNumber * 3] = 0;
			pIndices[iIndexNumber * 3 + 1] = i;
			pIndices[iIndexNumber * 3 + 2] = i + 1;

			// 끝면
			pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = (m_iNumVertices - 1);
			pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - i);
			pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - i - 1);
			++iIndexNumber;
		}

		else
		{
			// 앞면
			pIndices[iIndexNumber * 3] = 0;
			pIndices[iIndexNumber * 3 + 1] = i;
			pIndices[iIndexNumber * 3 + 2] = 1;

			// 끝면
			pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = (m_iNumVertices - 1);
			pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - i);
			pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - 1);
			++iIndexNumber;
		}
	}

	// 안쪽 폴리곤 만들기
	for (_ushort i = 0; i < (CCOLLIDER_SPHERE_STACK / 2); ++i) // 스택
	{
		for (_ushort j = 1; j <= CCOLLIDER_SPHERE_SLICE; ++j) // 슬라이스
		{
			if (j != CCOLLIDER_SPHERE_SLICE)
			{
				// 앞면 윗면
				pIndices[iIndexNumber * 3] = ((i * CCOLLIDER_SPHERE_SLICE) + j);
				pIndices[iIndexNumber * 3 + 1] = (((i + 1) * CCOLLIDER_SPHERE_SLICE) + j);
				pIndices[iIndexNumber * 3 + 2] = (((i + 1) * CCOLLIDER_SPHERE_SLICE) + j + 1);
				//++iIndexNumber;

				// 뒷면 윗면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j));
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((((i + 1) * CCOLLIDER_SPHERE_SLICE) + j)));
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((((i + 1) * CCOLLIDER_SPHERE_SLICE) + j + 1)));
				++iIndexNumber;

				// 윗면 아랫면
				pIndices[iIndexNumber * 3] = ((i * CCOLLIDER_SPHERE_SLICE) + j);
				pIndices[iIndexNumber * 3 + 1] = (((i + 1) * CCOLLIDER_SPHERE_SLICE) + j + 1);
				pIndices[iIndexNumber * 3 + 2] = ((i * CCOLLIDER_SPHERE_SLICE) + j + 1);
				//++iIndexNumber;

				// 뒷면 아랫면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j));
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j) - (CCOLLIDER_SPHERE_SLICE + 1));
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j + 1));
				++iIndexNumber;
			}
			else
			{
				// 앞면 윗면
				pIndices[iIndexNumber * 3] = ((i * CCOLLIDER_SPHERE_SLICE) + j);
				pIndices[iIndexNumber * 3 + 1] = (((i + 1) * CCOLLIDER_SPHERE_SLICE) + j);
				pIndices[iIndexNumber * 3 + 2] = (((i * CCOLLIDER_SPHERE_SLICE) + j) + 1);
				//++iIndexNumber;

				// 뒷면 윗면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j));
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - (((i + 1) * CCOLLIDER_SPHERE_SLICE) + j));
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j) - 1);
				++iIndexNumber;

				// 윗면 아랫면
				pIndices[iIndexNumber * 3] = ((i * CCOLLIDER_SPHERE_SLICE) + j);
				pIndices[iIndexNumber * 3 + 1] = (((i * CCOLLIDER_SPHERE_SLICE) + j) + 1);
				pIndices[iIndexNumber * 3 + 2] = (((i * CCOLLIDER_SPHERE_SLICE) + j) - (CCOLLIDER_SPHERE_SLICE - 1));
				//++iIndexNumber;

				// 뒷면 아랫면
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3)] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j));
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 1] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j) - 1);
				pIndices[(m_iNumIndices - 1) - (iIndexNumber * 3) - 2] = ((m_iNumVertices - 1) - ((i * CCOLLIDER_SPHERE_SLICE) + j) + (CCOLLIDER_SPHERE_SLICE - 1));
				++iIndexNumber;
			}
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CCollider_Sphere::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCollider_Sphere::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

	/*_float3 vScale = Get_Scale();
	m_pGraphic_Device->GetTransform(D3DTS_WORLD, &m_WorldMatrix);
	Set_Scale(vScale);*/
	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
	/*m_pGraphic_Device->GetTransform(D3DTS_WORLD, &m_WorldMatrix);
	Update_Collider_Info(m_WorldMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);*/
	__super::Render();
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

void CCollider_Sphere::Update_Collider_Info(_float4x4 vTargetWorldMatrix)
{
	Set_Scale(_float3(m_fRange, m_fRange, m_fRange));
    Set_State(STATE_POSITION, _float3(vTargetWorldMatrix.m[STATE_POSITION][0], vTargetWorldMatrix.m[STATE_POSITION][1], vTargetWorldMatrix.m[STATE_POSITION][2]));
}

CCollider_Sphere* CCollider_Sphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCollider_Sphere* pInstance = new CCollider_Sphere(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider_Sphere"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider_Sphere::Clone(void* pArg)
{
    CCollider_Sphere* pInstance = new CCollider_Sphere(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider_Sphere"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_Sphere::Free()
{
    __super::Free();
}


