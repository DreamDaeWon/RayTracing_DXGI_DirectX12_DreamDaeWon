#include "pch.h"
#include "Weapon_Sparrow.h"
#include "GameInstance.h"
#include"Bullet.h"
#include "Effect_Splat_Blue.h"

HRESULT CWeapon_Sparrow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CWeapon_Sparrow"));
		return E_FAIL;
	}
	m_eID = WEAPON_SPARROW;
	m_eBulletID = BULLET_RIFLE;
	m_fShootingTerm = SPARROW_SHOOTING_TERM;
	m_fTerm = SPARROW_SHOOTING_TERM;
	m_eShooting_Way = SHOOTING_SEMIAUTO;

	m_iMaxBulletNum = 5;
	m_iNowBulletNum = 5;
	m_iExtraBulletNum = 20;
	m_fCP = 50.f;
	m_fReboundSize = 3.f;
	m_iQAimSize = 24;
	return S_OK;
}

HRESULT CWeapon_Sparrow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CWeapon_Sparrow"));
		return E_FAIL;
	}

	if (FAILED(Clone_Bullet()))
	{
		MSG_BOX(TEXT("Failed to Clone_Bullet : CWeapon_Spparow"));
		return E_FAIL;
	}

	m_pEffect = dynamic_cast<CEffect_Splat_Blue*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Effect_Splat_Blue")));
	if (nullptr == m_pEffect)
	{
		MSG_BOX(TEXT("Failed to Get Effect : CWeapon_Sparrow"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Sparrow::Clone_Bullet()
{
	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 200.f;//?

	for (_uint i = 0; i < m_iMaxBulletNum; i++)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Sparrow_Bullet"), TEXT("Prototype_GameObject_Bullet"), &GameObjectDesc)))
		{
			MSG_BOX(TEXT("Failed to Ready_Layer_Bullet : CLevel_GamePlay"));
			return E_FAIL;
		}
	}

	m_vecBullet.reserve(m_iMaxBulletNum);

	list<CGameObject*>* pBulletList = m_pGameInstance->Get_List(LEVEL_STATIC, TEXT("Layer_Sparrow_Bullet"));
	if (nullptr == pBulletList)
		return E_FAIL;

	auto iter = (*pBulletList).begin();
	for (_uint i = 0; i < m_iMaxBulletNum; i++)
	{
		if (iter == (*pBulletList).end())
			break;
		m_vecBullet.push_back((CBullet*)(*iter));
		Safe_AddRef((*iter));
		iter++;
	}
	for (auto iter : m_vecBullet)
		iter->Set_Frame(5.f);
	m_iNowBulletNum = m_iMaxBulletNum;

	return S_OK;
}


CWeapon_Sparrow::CWeapon_Sparrow(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CWeapon(pGraphic_Device)
{
}

CWeapon_Sparrow::CWeapon_Sparrow(const CWeapon_Sparrow& rhs) :
	CWeapon(rhs)
{
}

_uint CWeapon_Sparrow::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bEx)
	{
		m_fCP = 150.f;
		m_iMaxBulletNum = 5;
	}
	else
	{
		m_fCP = 50.f;
		m_iMaxBulletNum = 5;
	}
		
	return 0;
}

void CWeapon_Sparrow::Shot_Bullet()
{
	if (0 >= m_iNowBulletNum || m_iMaxBulletNum < m_iNowBulletNum)
		return;

	_float3 vRayDir = {};
	_float3 vRayPos = {};
	m_pGameInstance->Get_World_Mouse_Ray(&vRayDir, &vRayPos);
	m_vecBullet[m_iNowBulletNum - 1]->Set_Pos_Dir(vRayPos, vRayDir);
	//pBullet->Set_Pos_Dir(vWeaponPos, vRayDir);
	m_vecBullet[m_iNowBulletNum - 1]->Set_Life_Time();
	--m_iNowBulletNum;
	if (!m_bEx)
		m_pGameInstance->PlaySoundW(TEXT("Atlas_Shoot.wav"), CSound_Manager::CHANNEL_WEAPON, 0.12f);
	else
		m_pGameInstance->PlaySoundW(TEXT("Atlas_ShootReinforce.wav"), CSound_Manager::CHANNEL_WEAPON, 0.12f);

	m_eCurState = STATE_SHOOTING;
	m_fShootingTerm = SPARROW_SHOOTING_TERM;
	m_pEffect->Set_Life_Time();
}

HRESULT CWeapon_Sparrow::Add_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Sparrow"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Weapon_Pupa : Add_Components"));
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_Sparrow"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom1))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Weapon_Atlas : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

CWeapon_Sparrow* CWeapon_Sparrow::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWeapon_Sparrow* pInstance = new CWeapon_Sparrow(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon_Sparrow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_Sparrow::Clone(void* pArg)
{
	CWeapon_Sparrow* pInstance = new CWeapon_Sparrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWeapon_Sparrow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Sparrow::Free()
{
	__super::Free();
}
