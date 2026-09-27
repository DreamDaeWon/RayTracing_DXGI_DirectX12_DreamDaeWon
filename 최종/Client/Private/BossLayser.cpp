#include "pch.h"
#include "BossLayser.h"
#include "GameInstance.h"

CBossLayser::CBossLayser(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CBossLayser::CBossLayser(const CBossLayser& rhs) :
	CLandObject(rhs)
{
}

HRESULT CBossLayser::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CMonster"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossLayser::Initialize(void* pArg)
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
		BOSS_LAYSER_DESC* pObjectBoxDesc = (BOSS_LAYSER_DESC*)pArg;
		m_pTransform->Set_State(CTransform::STATE_POSITION, pObjectBoxDesc->vPos);
		m_iFrame = pObjectBoxDesc->iFrame;
		m_iDir = pObjectBoxDesc->iDir;
	}

	// 크기를 설정해줌
	m_pTransform->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

	m_pCollider_Com->Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

	return S_OK;
}

_uint CBossLayser::Tick(_float fTimeDelta)
{

	if (m_bDead)
	{
		return OBJECT_DEAD;
	}

	__super::Tick(fTimeDelta);

	_float3 m_vScale = m_pTransform->Get_Scale();

	// 엄청 빠른 속도로 돌기
	m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT),D3DXToRadian(300.f));

	m_pTransform->Set_Scale(_float3(m_vScale.x + (m_fLongX * fTimeDelta), m_vScale.y, m_vScale.z));
	m_pCollider_Com->Set_Scale(_float3(m_vScale.x + (m_fLongX * fTimeDelta), m_vScale.y, m_vScale.z));

	if (m_iDir == 1) // 오른쪽
	{
		m_pTransform->Go_Right((m_fLongX * fTimeDelta) * 0.5f);
	}
	else // 왼쪽
	{
		m_pTransform->Go_Left((m_fLongX * fTimeDelta) * 0.5f);
	}

	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	return OBJECT_NOTHING;
}

void CBossLayser::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_fLifeTime < 0.f)
	{
		Set_Dead();
	}
	else
	{
		m_fLifeTime -= fTimeDelta;
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBossLayser::Render()
{

	//m_pTransform->Set_State(CTransform::STATE_LOOK, m_pCameraTransform->Get_State(CTransform::STATE_LOOK) * m_fScale);
	//m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pCameraTransform->Get_State(CTransform::STATE_RIGHT) * m_fScale);
	//m_pTransform->Set_State(CTransform::STATE_UP, m_pCameraTransform->Get_State(CTransform::STATE_UP) * m_fScale);
	//CTransform* m_pPlayerTransform = { nullptr };

	//m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_1945, TEXT("Layer_AirPlane"), TEXT("Com_Transform")));

	//m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).x,
	//	m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).y + 0.2f,
	//	m_pPlayerTransform->Get_State(CTransform::STATE_POSITION).z));

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

HRESULT CBossLayser::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Cube : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_1945, TEXT("Prototype_Component_Texture_BossBattleCruiserLayser"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_BossBattleCruiserLayser : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube_AABB"), TEXT("Com_Collider_Cube_AABB"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Cube_AABB : Add_Components"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBossLayser::Set_RenderState()
{
	return S_OK;
}

HRESULT CBossLayser::Reset_RenderState()
{
	return S_OK;
}

CBossLayser* CBossLayser::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBossLayser* pInstance = new CBossLayser(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CBossLayser"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossLayser::Clone(void* pArg)
{
	CBossLayser* pInstance = new CBossLayser(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CBossLayser"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossLayser::Free()
{
	Safe_Release(m_pVIBuffer_Com);
	Safe_Release(m_pCollider_Com);
	Safe_Release(m_pTextureCom);
	__super::Free();
}
