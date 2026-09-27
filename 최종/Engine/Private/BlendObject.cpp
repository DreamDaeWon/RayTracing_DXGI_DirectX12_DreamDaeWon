#include "BlendObject.h"


CBlendObject::CBlendObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBlendObject::CBlendObject(const CBlendObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CBlendObject::Tick(_float fTimeDelta)
{
	return 0;
}

void CBlendObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}

HRESULT CBlendObject::Compute_ViewZ()
{
	_float3			vPosition = m_pTransform->Get_State(CTransform::STATE_POSITION);

	_float4x4		ViewMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXVec3TransformCoord(&vPosition, &vPosition, &ViewMatrix);

	m_fViewZ = vPosition.z;

	return S_OK;
}

void CBlendObject::Free()
{
	__super::Free();

}
