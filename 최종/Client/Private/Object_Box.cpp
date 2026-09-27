#include "pch.h"
#include "Object_Box.h"
#include "GameInstance.h"
#include "Player.h"
#include "Weapon.h"

CObject_Box::CObject_Box(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CObject_Box::CObject_Box(const CObject_Box& rhs) :
	CLandObject(rhs)
{
}

HRESULT CObject_Box::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CMonster"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CObject_Box::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		LANDOBJECT_DESC* pLandObjectDesc = (LANDOBJECT_DESC*)pArg;
		pLandObjectDesc->fSpeedPerSec = 1.f;
		pLandObjectDesc->fRotationPerSec = D3DXToRadian(90.f);
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
		OBJ_BOX_DESC* pObjectBoxDesc = (OBJ_BOX_DESC*)pArg;
		m_pTransform->Set_State(CTransform::STATE_POSITION, pObjectBoxDesc->vPosition);
		m_iFrame = pObjectBoxDesc->iFrame;
	}

	m_pCollider_Com[COLLIDER_RECT]->Set_Scale(_float3(1.f, 1.f, 1.f));

	return S_OK;
}

_uint CObject_Box::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pCollider_Com[COLLIDER_CUBE_AABB]->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());
	m_pCollider_Com[COLLIDER_RECT]->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());
	Collider_Billboarding();
	return OBJECT_NOTHING;
}

void CObject_Box::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Collision_Bullet(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CObject_Box::Render()
{
	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Texture(0, m_iFrame)))
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

HRESULT CObject_Box::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Object_Box"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Object_Box : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Collider_Cube_AABB */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube_AABB"), TEXT("Com_Collider_Cube_AABB"), reinterpret_cast<CComponent**>(&m_pCollider_Com[COLLIDER_CUBE_AABB]))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Cube_AABB : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Collider_Rect */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), TEXT("Com_Collider_Rect"), reinterpret_cast<CComponent**>(&m_pCollider_Com[COLLIDER_RECT]))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CObject_Box::Set_RenderState()
{
	return S_OK;
}

HRESULT CObject_Box::Reset_RenderState()
{
	return S_OK;
}

void CObject_Box::Collision_Bullet(_float fTimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	CWeapon* pWeapons[3] = { pPlayer->Get_Weapon(0),pPlayer->Get_Weapon(1) ,pPlayer->Get_Weapon(2) };

	for (size_t i = 0; i < 3; i++)
	{
		if (nullptr == pWeapons[i])
			continue;
		pWeapons[i]->Collision_Bullet_Rect_For_Box(dynamic_cast<CCollider_Rect*>(m_pCollider_Com[COLLIDER_RECT]), fTimeDelta);
	}
}

void CObject_Box::Collider_Billboarding()
{
	CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera_Player"), g_strTransformTag));

	if (nullptr == pCameraTransformCom)
	{
		MSG_BOX(TEXT("nullptr == pCameraTransformCom : Collider_Billboarding()"));
		return;
	}

	const _float4x4 pCameraWorldMatrix = *pCameraTransformCom->Get_WorldMatrix();


	_float3 vScale = m_pCollider_Com[COLLIDER_RECT]->Get_Scale();

	m_pCollider_Com[COLLIDER_RECT]->Set_State(CCollider::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	//m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pCollider_Com[COLLIDER_RECT]->Set_State(CCollider::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	
}

CObject_Box* CObject_Box::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CObject_Box* pInstance = new CObject_Box(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CObject_Box"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CObject_Box::Clone(void* pArg)
{
	CObject_Box* pInstance = new CObject_Box(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CObject_Box"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Box::Free()
{
	Safe_Release(m_pCollider_Com[COLLIDER_CUBE_AABB]);
	Safe_Release(m_pCollider_Com[COLLIDER_RECT]);
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);
	__super::Free();
}
