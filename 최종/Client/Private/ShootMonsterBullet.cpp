#include "pch.h"
#include "GameInstance.h"
#include "ShootMonsterBullet.h"

CShootMonsterBullet::CShootMonsterBullet(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CMonster_Bullet_Base(pGraphic_Device)
{
}

CShootMonsterBullet::CShootMonsterBullet(const CShootMonsterBullet& rhs) :
	CMonster_Bullet_Base(rhs)
{
}

HRESULT CShootMonsterBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CShootMonsterBullet"));
		return E_FAIL;
	}

	//¸â¹ö ÃÊ±âÈ­
	m_fSpeed = { 1.f };
	m_vBossPos = {};
	m_fScale = { 0.5f };
	m_fLifeTime = { 0.f };
	m_iTexNum = { 0 };
	m_vLook = { 0.f, 1.f, 0.f };

	return S_OK;
}

HRESULT CShootMonsterBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CShootMonsterBullet"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CShootMonsterBullet"));
		return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT) * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_vBossPos.x, 0.f, m_vBossPos.z));

	//For. Collider
	dynamic_cast<CCollider_Sphere*>(m_pCollider_Com)->Set_Range(0.5f);

	return S_OK;
}

_uint CShootMonsterBullet::Tick(_float fTimeDelta)
{

	if (m_bDead)
	{
		m_pUI_Damage->Set_RealDead();
		return OBJECT_DEAD;
	}

	//Set_Render_Texture(fTimeDelta);
	//Do_State(fTimeDelta);



	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	if (!m_bDeadShow && m_eState != STATE_DEATH)
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION,
			_float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x + (m_vLook.x * m_fSpeed * fTimeDelta),
				0.f,
				m_pTransform->Get_State(CTransform::STATE_POSITION).z + m_vLook.z * m_fSpeed * fTimeDelta));
	}

	//For. Collider
	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CShootMonsterBullet::Late_Tick(_float fTimeDelta)
{
	if (0.f > m_fLifeTime && !m_bDead)
	{
		m_eState = STATE_DEATH;
		m_fTime = 0.f;
		Set_Dead();
		m_bDeadShow = true;
	}

	//Set_Render_Texture(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CShootMonsterBullet::Render()
{
	//if (FAILED(__super::Render()))
	//{
	//	MSG_BOX(TEXT("Failed to Render : __super,CPlayer"));
	//	return E_FAIL;
	//}

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

void CShootMonsterBullet::Do_State(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	switch (m_eState)
	{
	case STATE_IDLE:


		break;

	case STATE_DEATH:
	
			m_bDeadShow = true;

		break;

	default:
		break;
	}

}

HRESULT CShootMonsterBullet::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_ShootMonster_Bullet"), TEXT("Com_TextureIdle"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_ShootMonster_Bullet : Add_Components"));
		return E_FAIL;
	}


	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pCollider_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Collider_Sphere : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

CShootMonsterBullet* CShootMonsterBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShootMonsterBullet* pInstance = new CShootMonsterBullet(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CShootMonsterBullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShootMonsterBullet::Clone(void* pArg)
{
	CShootMonsterBullet* pInstance = new CShootMonsterBullet(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CShootMonsterBullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShootMonsterBullet::Free()
{
	//m_pTextureCom = nullptr;
	//Safe_Release(m_pTextureCom);
	__super::Free();
}
