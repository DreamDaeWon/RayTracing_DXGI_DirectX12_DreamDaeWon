#include "pch.h"
#include "Trigger_Camera.h"
#include "GameInstance.h"
#include "Player.h"
#include"Level_Loading.h"
#include"Camera_Player_DW.h"

CTrigger_Camera::CTrigger_Camera(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CTrigger_Camera::CTrigger_Camera(const CTrigger_Camera& rhs) :
	CLandObject(rhs)
{
}

HRESULT CTrigger_Camera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CTrigger_Camera"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrigger_Camera::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		TRIGGER_DESC* pSumonObjectDesc = (TRIGGER_DESC*)pArg;
		m_eCurCameraMode = pSumonObjectDesc->eCameraMode;
		m_vPos = pSumonObjectDesc->vPos;
		m_fScale = pSumonObjectDesc->fScale;
		m_fGoSpeed = pSumonObjectDesc->fSpeed;
		m_iTexNum = pSumonObjectDesc->iTexNum;


		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = D3DXToRadian(1.f);

	}

	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CTrigger_Camera"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CTrigger_Camera"));
		return E_FAIL;
	}


	m_pTransform->Set_Scale(_float3(1.f,1.f,1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, m_vPos);
	m_pCollider_Com->Set_Range(1.5f);
	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix()); 
	
	return S_OK;
}

_uint CTrigger_Camera::Tick(_float fTimeDelta)
{


	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	__super::Tick(fTimeDelta);

	return OBJECT_NOTHING;
}

void CTrigger_Camera::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CTrigger_Camera::Render()
{
	return S_OK;
}


HRESULT CTrigger_Camera::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
		return E_FAIL;
	}/* For.Prototype_Component_Texture_Red_Circle */
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Red_Circle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Trigger_Door : Add_Components"));
		return E_FAIL;
	}

	//콜라이더 실험 BEGIN.
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Sphere : Add_Components"));
		return E_FAIL;
	}
	//콜라이더 실험 END.
	return S_OK;
}





CTrigger_Camera* CTrigger_Camera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTrigger_Camera* pInstance = new CTrigger_Camera(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CTrigger_Camera"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger_Camera::Clone(void* pArg)
{
	CTrigger_Camera* pInstance = new CTrigger_Camera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CTrigger_Camera"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger_Camera::Free()
{

	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);

	//콜라이더 실험 BEGIN
	Safe_Release(m_pCollider_Com);
	//콜라이더 실험 END.

	__super::Free();
}
