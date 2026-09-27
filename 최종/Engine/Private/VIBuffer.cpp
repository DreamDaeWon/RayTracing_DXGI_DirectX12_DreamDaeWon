#include "VIBuffer.h"
#include "GameInstance.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs) :
	CComponent(rhs),
	m_pVB(rhs.m_pVB),
	m_iVertexStride(rhs.m_iVertexStride),
	m_iNumVertices(rhs.m_iNumVertices),
	m_dwFVF(rhs.m_dwFVF),
	m_dwPrimitive(rhs.m_dwPrimitive),
	m_pIB(rhs.m_pIB),
	m_iIndexStride(rhs.m_iIndexStride),
	m_iNumIndices(rhs.m_iNumIndices),
	m_eIndexFormat(rhs.m_eIndexFormat),
	m_pVerticesPos(rhs.m_pVerticesPos), //터레인에 있던거 옮겨옴.
	m_pIndicesInfo(rhs.m_pIndicesInfo) //새로 만듬. 
{ 
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (FAILED(m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride))) // 인자 : 0번째부터 바인딩 시작, 버텍스 버퍼, 어디서부터 읽을지, 정점 개수
	{
		MSG_BOX(TEXT("Failed to SetStreamSource : __super,CVIBuffer"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetIndices(m_pIB))) 
	{
		MSG_BOX(TEXT("Failed to SetIndices : __super,CVIBuffer"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetFVF(m_dwFVF)))
	{
		MSG_BOX(TEXT("Failed to SetFVF : __super,CVIBuffer"));
		return E_FAIL;
	}
	return m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_dwPrimitive);
}

_float3 CVIBuffer::Compute_Picking(CTransform* pTransform)
{
	_float3 vRayPos, vRayDir;
	_float3 vOut = { 0.f,0.f,0.f };

	//레이포즈와 레이디렉션을 들어온 트랜스폼의 로컬스페이스로 옮긴다.
	m_pGameInstance->Transform_PickingToLocalSpace(pTransform, &vRayDir, &vRayPos);

	if (nullptr == m_pVerticesPos)
		return _float3(0.f,0.f,0.f);

	if (nullptr == m_pIndicesInfo)
		return _float3(0.f, 0.f, 0.f);

	_uint iIndex = 0;

	//TODO : 수정 요함. 내생각엔 인덱스 정보도 저장하는 공간이 필요함.
	if (2 == m_iIndexStride)
	{

		/*_ushort* pIndices = { nullptr };
		m_pIB->Lock(0, 0, (void**)&pIndices, 0);

		for (size_t i = 0; i < m_dwPrimitive; i++)
		{
			_float fU(0.f), fV(0.f), fDist(0.f);

			if (TRUE == D3DXIntersectTri(&m_pVerticesPos[pIndices[iIndex]], &m_pVerticesPos[pIndices[iIndex + 1]], &m_pVerticesPos[pIndices[iIndex + 2]],
				&vRayPos, D3DXVec3Normalize(&vRayDir, &vRayDir), &fU, &fV, &fDist))
			{
				m_pIB->Unlock();
				vOut = vRayPos + vRayDir * fDist;
				vOut = *D3DXVec3TransformCoord(&vOut, &vOut, pTransform->Get_WorldMatrix());
				return vOut;
			}
			iIndex += 3;
		}
		m_pIB->Unlock();*/
		for (size_t i = 0; i < m_dwPrimitive; i++)
		{
			_float fU(0.f), fV(0.f), fDist(0.f);

			if (TRUE == D3DXIntersectTri(&m_pVerticesPos[((_ushort*)m_pIndicesInfo)[iIndex]], &m_pVerticesPos[((_ushort*)m_pIndicesInfo)[iIndex + 1]], &m_pVerticesPos[((_ushort*)m_pIndicesInfo)[iIndex + 2]],
				&vRayPos, D3DXVec3Normalize(&vRayDir, &vRayDir), &fU, &fV, &fDist))
			{
				vOut = vRayPos + vRayDir * fDist;
				vOut = *D3DXVec3TransformCoord(&vOut, &vOut, pTransform->Get_WorldMatrix());
				return vOut;
			}
			iIndex += 3;
		}
	}
	else if (4 == m_iIndexStride)
	{
		for (size_t i = 0; i < m_dwPrimitive; i++)
		{
			_float fU(0.f), fV(0.f), fDist(0.f);

			if (TRUE == D3DXIntersectTri(&m_pVerticesPos[((_uint*)m_pIndicesInfo)[iIndex]], &m_pVerticesPos[((_uint*)m_pIndicesInfo)[iIndex + 1]], &m_pVerticesPos[((_uint*)m_pIndicesInfo)[iIndex + 2]],
				&vRayPos, D3DXVec3Normalize(&vRayDir, &vRayDir), &fU, &fV, &fDist))
			{
				vOut = vRayPos + vRayDir * fDist;
				vOut = *D3DXVec3TransformCoord(&vOut, &vOut, pTransform->Get_WorldMatrix());
				return vOut;
			}
			iIndex += 3;
		}
	}
	return vOut;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	/* 정점 배열을 할당하고 LPDIRECT3DVERTEXBUFFER라는 컴객체를 생성한다. */
	/* D3DUSAGE_DYNAMIC : 메모리공간을 동적으로 만든다 .*/
	/* 0 : 메모리공간을 정적으로 만든다 .*/

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
	{
		MSG_BOX(TEXT("Failed to CreateVertexBuffer : __super,CVIBuffer"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer::Create_IndexBuffer()
{

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, 0)))
	{
		MSG_BOX(TEXT("Failed to CreateIndexBuffer : __super,CVIBuffer"));
		return E_FAIL;
	}
	return S_OK;
}



void CVIBuffer::Free()
{
	if (false == m_bIsCloned)
		Safe_Delete_Array(m_pVerticesPos);

	if (false == m_bIsCloned)
		Safe_Delete_Array(m_pIndicesInfo);

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	__super::Free();
}
