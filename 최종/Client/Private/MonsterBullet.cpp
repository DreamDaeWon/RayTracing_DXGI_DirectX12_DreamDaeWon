#include "pch.h"
#include "GameInstance.h"
#include "MonsterBullet.h"

CMonsterBullet::CMonsterBullet(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CMonster_Bullet_Base(pGraphic_Device)
{
}

CMonsterBullet::CMonsterBullet(const CMonsterBullet& rhs) :
	CMonster_Bullet_Base(rhs)
{
}

HRESULT CMonsterBullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CMonsterBullet"));
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

HRESULT CMonsterBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CMonsterBullet"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CMonsterBullet"));
		return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK));
	m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT) * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * m_fScale);
	m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_vBossPos.x, m_vBossPos.y - 0.2f, m_vBossPos.z));

	//For. Collider
	dynamic_cast<CCollider_Sphere*>(m_pCollider_Com)->Set_Range(0.5f);

	return S_OK;
}

_uint CMonsterBullet::Tick(_float fTimeDelta)
{

	if (m_bDead)
		return OBJECT_DEAD;

	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	m_pTransform->Set_State(CTransform::STATE_POSITION,
		_float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x + (m_vLook.x * m_fSpeed * fTimeDelta),
			m_pTransform->Get_State(CTransform::STATE_POSITION).y + (m_vLook.y * m_fSpeed * fTimeDelta),
			m_pTransform->Get_State(CTransform::STATE_POSITION).z + m_vLook.z * m_fSpeed * fTimeDelta));


	//For. Collider
	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CMonsterBullet::Late_Tick(_float fTimeDelta)
{
	if (0.f > m_fLifeTime)
		Set_Dead();

	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();


	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMonsterBullet::Render()
{
	if (FAILED(__super::Render()))
	{
		MSG_BOX(TEXT("Failed to Render : __super,CPlayer"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonsterBullet::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_Monster_Three_Red_Eyes_Bullet"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Monster_Three_Red_Eyes_Bullet : Add_Components"));
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

CMonsterBullet* CMonsterBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonsterBullet* pInstance = new CMonsterBullet(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CMonsterBullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterBullet::Clone(void* pArg)
{
	CMonsterBullet* pInstance = new CMonsterBullet(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CMonsterBullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBullet::Free()
{
	__super::Free();
}
