#include "Collider.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CComponent(pGraphic_Device)
{
}

CCollider::CCollider(const CCollider& rhs) :
	CComponent(rhs),
	m_WorldMatrix(rhs.m_WorldMatrix), 
	m_pVB(rhs.m_pVB),
	m_iVertexStride(rhs.m_iVertexStride),
	m_iNumVertices(rhs.m_iNumVertices),
	m_dwFVF(rhs.m_dwFVF),
	m_dwPrimitive(rhs.m_dwPrimitive),
	m_pIB(rhs.m_pIB),
	m_iIndexStride(rhs.m_iIndexStride),
	m_iNumIndices(rhs.m_iNumIndices),
	m_eIndexFormat(rhs.m_eIndexFormat)
{
}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCollider::Render()
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

	return S_OK;
}


void CCollider::Turn(const _float3& vAxis, _float fTimeDelta)
{
	_float4x4 RotationMatrix;

	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, D3DXToRadian(90.f) * fTimeDelta);

	_float3 vRight = Get_State(STATE_RIGHT);
	_float3 vUp = Get_State(STATE_UP);
	_float3 vLook = Get_State(STATE_LOOK);

	Set_State(STATE_RIGHT, *D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix));
	Set_State(STATE_UP, *D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix));
	Set_State(STATE_LOOK, *D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix));
}

HRESULT CCollider::Create_VertexBuffer()
{
	/* 정점 배열을 할당하고 LPDIRECT3DVERTEXBUFFER라는 컴객체를 생성한다. */
	/* D3DUSAGE_DYNAMIC : 메모리공간을 동적으로 만든다 .*/
	/* 0 : 메모리공간을 정적으로 만든다 .*/

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iVertexStride * m_iNumVertices, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
	{
		MSG_BOX(TEXT("Failed to CreateVertexBuffer : __super,CCollider"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCollider::Create_IndexBuffer()
{

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, 0)))
	{
		MSG_BOX(TEXT("Failed to CreateIndexBuffer : __super,CCollider"));
		return E_FAIL;
	}
	return S_OK;
}





void CCollider::Free()
{
	__super::Free();
}

