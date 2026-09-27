#include "pch.h"
#include "Blizzard.h"
#include "GameInstance.h"

CBlizzard::CBlizzard(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CBlizzard::CBlizzard(const CBlizzard& rhs) :
	CLandObject(rhs)
{
}

HRESULT CBlizzard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlizzard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CBullet"));
		return E_FAIL;
	}


	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CBullet"));
		return E_FAIL;
	}


	if (nullptr != pArg)
	{
		BLIZZARD_DESC* pBlizzardDesc = (BLIZZARD_DESC*)pArg;
		m_pTransform->Set_Scale(_float3(pBlizzardDesc->fScale, pBlizzardDesc->fScale, 1.f));
		m_pTransform->Set_State(CTransform::STATE_POSITION, pBlizzardDesc->vPos);
	}


	m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Object(LEVEL_STATIC,TEXT("Layer_Camera_Player"))->Get_Component(g_strTransformTag));
	Safe_AddRef(m_pCameraTransform);
	//m_pTransform->Turn(_float3(0.f, 1.f, 0.f), 2);
	//m_pTransform->Set_Scale(_float3(0.2f, 0.2f, 1.f));

	return S_OK;
}

_uint CBlizzard::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJECT_NOTHING;

	//m_pTransform->Go_Straight(fTimeDelta); //움직인다.
	Move(fTimeDelta);
	
	SetUp_BillBoard();
	return OBJECT_NOTHING;
}

void CBlizzard::Late_Tick(_float fTimeDelta)
{
	Restart();
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CBlizzard::Render()
{

	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Texture(0, 0)))
	{
		MSG_BOX(TEXT("Failed to Bind_Texture : Render"));
		return E_FAIL;
	}
	if (FAILED(Set_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Set_RenderState : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer_Com->Render()))
	{
		MSG_BOX(TEXT("Failed to Render : Render"));
		return E_FAIL;
	}
	if (FAILED(Reset_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Reset_RenderState : Render"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlizzard::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TrailRenderer"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_TrailRenderer : CBlizzard::Add_Components"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlizzard::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	return S_OK;
}

HRESULT CBlizzard::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

void CBlizzard::SetUp_BillBoard()
{
	if (nullptr == m_pCameraTransform)
		return;

	const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();


	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);
}

void CBlizzard::Move(_float fTimeDelta)
{
	_float3 vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	vPos.y -= 2.f * fTimeDelta;
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

void CBlizzard::Restart()
{
	_float3 vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

	if (BLIZZARD_END_Y > vPos.y)
	{
		vPos.y = BLIZZARD_START_Y;
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}
}

CBlizzard* CBlizzard::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlizzard* pInstance = new CBlizzard(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlizzard"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlizzard::Clone(void* pArg)
{
	CBlizzard* pInstance = new CBlizzard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBlizzard"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlizzard::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer_Com);

	Safe_Release(m_pCameraTransform);
	__super::Free();
}
