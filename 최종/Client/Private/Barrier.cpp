#include "pch.h"
#include "Barrier.h"
#include "GameInstance.h"
#include "Player.h"

_float g_shield = 0.5f;

CBarrier::CBarrier(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CPlayer_Skill(pGraphic_Device)
{
}

CBarrier::CBarrier(const CBarrier& rhs) :
	CPlayer_Skill(rhs)
{
}

HRESULT CBarrier::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CBarrier"));
		return E_FAIL;
	}
	m_eID = SKILL_SHIELD;
	m_fCoolTime = 4.f;
	m_fSP = 20.f;
	m_fKeepSP = 15.f;
	m_fActTime = 1.5f;
	m_fScale = 0.5f;
	return S_OK;
}

HRESULT CBarrier::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CBarrier"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CBarrier"));
		return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(0.f, 5.f, 0.f));

	return S_OK;
}

_uint CBarrier::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

	Key_Input(fTimeDelta);

	// 실드 커지는 효과
	if (m_bActivate == true)
	{
		m_pGameInstance->PlaySoundW(L"shield_start.wav", CSound_Manager::EFFECT, g_shield);

		if (1.f > m_fScale)
		{
			m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
			m_fScale += 0.01f;
		}
	}
	else
		m_fScale = 0.5f;

	return 0;
}

void CBarrier::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CBarrier::Render()
{
	if (!m_bActivate)
		return S_OK;

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

void CBarrier::Key_Input(_float fTimeDelta)
{
	if (m_bActivate == true)
	{
		m_fTime += fTimeDelta;
		dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_InvincibilityTime(m_fActTime);
		dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Invincibility(true);

	}

	
	if (m_bActivate && m_fTime > m_fActTime)
	{
		m_bActivate = false;
		m_fTime = 0.f;
	}


}

HRESULT CBarrier::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Sphere"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Sphere : Add_Components"));
		return E_FAIL;
	}
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Barrier"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Collider : Add_Components"));
		return E_FAIL;
	}
	/* For.Com_VIBuffer */
	/*if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Sphere"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Sphere : Add_Components"));
		return E_FAIL;
	}*/
	/* For.Com_Texture */
	/*if (FAILED(__super::Add_Component(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Effect_Barrier"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Collider : Add_Components"));
		return E_FAIL;
	}*/
	return S_OK;
}

HRESULT CBarrier::Set_RenderState()
{
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_SRCBLEND"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_DESTBLEND"));
		return E_FAIL;
	}
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DBLENDOP_ADD"));
		return E_FAIL;
	}

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}

HRESULT CBarrier::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHATESTENABLE"));
		return E_FAIL;
	}

	if (FAILED(m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE)))
	{
		MSG_BOX(TEXT("Failed to SetRenderState : D3DRS_ALPHABLENDENABLE"));
		return E_FAIL;
	}

	return S_OK;
}

CBarrier* CBarrier::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBarrier* pInstance = new CBarrier(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CBarrier"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBarrier::Clone(void* pArg)
{
	CBarrier* pInstance = new CBarrier(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CBarrier"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBarrier::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pTextureCom);

	__super::Free();
}
