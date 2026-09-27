#include "pch.h"
#include "AirPlaneTurretBody.h"
#include "GameInstance.h"

CAirPlaneTurretBody::CAirPlaneTurretBody(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CAirPlaneTurretBody::CAirPlaneTurretBody(const CAirPlaneTurretBody& rhs) :
	CLandObject(rhs)
{
}

HRESULT CAirPlaneTurretBody::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CMonster"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAirPlaneTurretBody::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = 1.f;
	}

	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CMonster"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CMonster"));
		return E_FAIL;
	}

	if (pArg != nullptr)
	{
		TURRET_BODY_DESC* pObjectBoxDesc = (TURRET_BODY_DESC*)pArg;
		m_pTransform->Set_State(CTransform::STATE_POSITION, pObjectBoxDesc->vPosition);
		m_iFrame = pObjectBoxDesc->iFrame;
	}

	// 크기를 설정해줌
	m_pTransform->Set_Scale(_float3(m_fScale, m_fScale * 1.f, m_fScale));


	return S_OK;
}

_uint CAirPlaneTurretBody::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CAirPlaneTurretBody::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CAirPlaneTurretBody::Render()
{

	//m_pTransform->Set_State(CTransform::STATE_LOOK, m_pCameraTransform->Get_State(CTransform::STATE_LOOK) * m_fScale);
	//m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pCameraTransform->Get_State(CTransform::STATE_RIGHT) * m_fScale);
	//m_pTransform->Set_State(CTransform::STATE_UP, m_pCameraTransform->Get_State(CTransform::STATE_UP) * m_fScale);
	CTransform* m_pPlayerTransform = { nullptr };

	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_1945, TEXT("Layer_AirPlane"), TEXT("Com_Transform")));

	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).x,
		m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).y + 0.2f,
		m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).z));

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
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
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

HRESULT CAirPlaneTurretBody::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_1945, TEXT("Prototype_Component_Texture_TurretBody"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_TurretBody : Add_Components"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAirPlaneTurretBody::Set_RenderState()
{
	return S_OK;
}

HRESULT CAirPlaneTurretBody::Reset_RenderState()
{
	return S_OK;
}

CAirPlaneTurretBody* CAirPlaneTurretBody::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAirPlaneTurretBody* pInstance = new CAirPlaneTurretBody(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CAirPlaneTurretBody"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAirPlaneTurretBody::Clone(void* pArg)
{
	CAirPlaneTurretBody* pInstance = new CAirPlaneTurretBody(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CAirPlaneTurretBody"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAirPlaneTurretBody::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);
	__super::Free();
}
