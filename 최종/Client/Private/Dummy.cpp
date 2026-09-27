#include "pch.h"
#include "GameInstance.h"
#include "Dummy.h"
#include "Player.h"
#include <Weapon.h>

CDummy::CDummy(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CDummy::CDummy(const CDummy& rhs) :
	CLandObject(rhs)
{
}

HRESULT CDummy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super, CDummy"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CDummy"));
		return E_FAIL;
	}

	if (pArg != nullptr)
	{
		DUMMY_DESC* DummyDesc = (DUMMY_DESC*)pArg;
		m_vPosition = DummyDesc->vPos;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CDummy"));
		return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE_POSITION, m_vPosition);
	m_pTransform->Set_Scale(_float3(1.f, 2.f, 1.f));
	return S_OK;
}

_uint CDummy::Tick(_float fTimeDelta)
{
	//if (m_bDead)
	//	return OBJECT_DEAD;

	__super::Tick(fTimeDelta);

	m_pUI_Damage->Update_Position(Return_ViewPort_Pos(), Get_ViewZ());
	m_pCollider_Com[COLLIDER_RECT]->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	return OBJECT_NOTHING;
}

void CDummy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Collision_Bullet(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CDummy::Render()
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

HRESULT CDummy::Add_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Dummy"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Dummy : Add_Components"));
		return E_FAIL;
	}

	/* For. Com_Collider*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"), TEXT("Com_Collider_Rect"), reinterpret_cast<CComponent**>(&m_pCollider_Com[COLLIDER_RECT]))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Rect : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummy::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

HRESULT CDummy::Reset_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

void CDummy::Collision_Bullet(_float fTimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	CWeapon* pWeapons[3] = { pPlayer->Get_Weapon(0),pPlayer->Get_Weapon(1) ,pPlayer->Get_Weapon(2) };

	for (size_t i = 0; i < 3; i++)
	{
		if (nullptr == pWeapons[i])
			continue;
		_float fTotalDamage = pWeapons[i]->Collision_Bullet_Rect(dynamic_cast<CCollider_Rect*>(m_pCollider_Com[COLLIDER_RECT]), fTimeDelta);
		if(0.f < fTotalDamage)
			m_pUI_Damage->Hit_Damage(fTotalDamage);
		pWeapons[i]->Plus_GunGauge(fTotalDamage);
	}
}

//void CDummy::SetUp_BillBoard()
//{
//}

CDummy* CDummy::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDummy* pInstance = new CDummy(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CDummy"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
	CDummy* pInstance = new CDummy(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CDummy"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCollider_Com[COLLIDER_RECT]);

	__super::Free();
}
