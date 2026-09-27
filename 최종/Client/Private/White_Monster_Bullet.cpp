#include "pch.h"
#include "GameInstance.h"
#include "White_Monster_Bullet.h"

CWhite_Monster_Bullet::CWhite_Monster_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CMonster_Bullet_Base(pGraphic_Device)
{
}

CWhite_Monster_Bullet::CWhite_Monster_Bullet(const CWhite_Monster_Bullet& rhs) :
	CMonster_Bullet_Base(rhs)
{
}

HRESULT CWhite_Monster_Bullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CWhite_Monster_Bullet"));
		return E_FAIL;
	}

	//멤버 초기화
	m_fSpeed = { 1.f };
	m_vBossPos = {};
	m_fScale = { 0.5f };
	m_fLifeTime = { 0.f };
	m_iTexNum = { 0 };
	m_vLook = { 0.f, 1.f, 0.f };

	return S_OK;
}

HRESULT CWhite_Monster_Bullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{

		MSG_BOX(TEXT("Failed to Initialize : __super,CWhite_Monster_Bullet"));
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components : __super,CWhite_Monster_Bullet"));
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

_uint CWhite_Monster_Bullet::Tick(_float fTimeDelta)
{
	
	if (m_bDead && m_bDeadShow)
	{
		return OBJECT_DEAD;
	}

	Set_Render_Texture(fTimeDelta);
	Do_State(fTimeDelta);


	if (m_bDead)
		return OBJECT_NOTHING;


	if (0.f <= m_fLifeTime)
		m_fLifeTime -= fTimeDelta;

	if (!m_bDeadShow && m_eState != STATE_DEATH)
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION,
			_float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x + (m_vLook.x * m_fSpeed * fTimeDelta),
				m_pTransform->Get_State(CTransform::STATE_POSITION).y + (m_vLook.y * m_fSpeed * fTimeDelta),
				m_pTransform->Get_State(CTransform::STATE_POSITION).z + m_vLook.z * m_fSpeed * fTimeDelta));
	}

	//For. Collider
	m_pCollider_Com->Update_Collider_Info(*m_pTransform->Get_WorldMatrix());

	__super::Tick(fTimeDelta);
	return OBJECT_NOTHING;
}

void CWhite_Monster_Bullet::Late_Tick(_float fTimeDelta)
{
	if (0.f > m_fLifeTime && !m_bDead)
	{
		m_fTexture = 0.f;
		m_eState = STATE_DEATH;
		m_fTime = 0.f;
		Set_Dead();
	}
	else if (m_bDead && m_eState != STATE_DEATH)
	{
		m_fTexture = 0.f;
		m_eState = STATE_DEATH;
		m_fTime = 0.f;
	}

	Set_Render_Texture(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

	SetUp_BillBoard();

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWhite_Monster_Bullet::Render()
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
	if (FAILED(m_pTextureCom->Bind_Texture(0, (_uint)m_fTextureTotal)))
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

void CWhite_Monster_Bullet::Do_State(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	switch (m_eState)
	{
	case STATE_IDLE:


		break;

	case STATE_DEATH:
		if (m_fTime <= 0.5f)
		{

		}
		else
		{
			m_bDeadShow = true;
		}

		break;
	
	default:
		break;
	}

}

void CWhite_Monster_Bullet::Set_Render_Texture(_float fTimeDelta)
{
	// 상태에 따라 텍스쳐를 설정해준다.

	if (m_eState == STATE_IDLE)
	{
		m_pTextureCom = m_pTextureIdle;
		m_fTextureMaxPicture = 5.f;
		m_fTextureSpeed = m_fTextureSpeed_Idle;

	}

	if (m_eState == STATE_DEATH)
	{
		m_pTextureCom = m_pTextureDeath;
		m_fTextureMaxPicture = 6.f;
		m_fTextureSpeed = m_fTextureSpeed_Death;

	}

	// 시간에 따라 텍스쳐를 설정해 준다.
	m_fTexture += m_fTextureMaxPicture * fTimeDelta * m_fTextureSpeed;
	if ((_int)m_fTexture > (_int)m_fTextureMaxPicture)
	{
		m_fTexture = 0;
	}

	// 방향에 따라 텍스쳐를 설정해준다.
	m_fTextureTotal = m_fTexture;
}

HRESULT CWhite_Monster_Bullet::Add_Components()
{	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_White_Fly_Monster_Bullet_Idle"), TEXT("Com_TextureIdle"), reinterpret_cast<CComponent**>(&m_pTextureIdle))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_White_Fly_Monster_Bullet_Idle : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_eLevel, TEXT("Prototype_Component_Texture_White_Fly_Monster_Bullet_Death"), TEXT("Com_TextureDeath"), reinterpret_cast<CComponent**>(&m_pTextureDeath))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_White_Fly_Monster_Bullet_Death : Add_Components"));
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

CWhite_Monster_Bullet* CWhite_Monster_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWhite_Monster_Bullet* pInstance = new CWhite_Monster_Bullet(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Faild to Created : CWhite_Monster_Bullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWhite_Monster_Bullet::Clone(void* pArg)
{
	CWhite_Monster_Bullet* pInstance = new CWhite_Monster_Bullet(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Faild to Cloned : CWhite_Monster_Bullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWhite_Monster_Bullet::Free()
{
	m_pTextureCom = nullptr;
	Safe_Release(m_pTextureIdle);
	Safe_Release(m_pTextureDeath);
	__super::Free();
}
