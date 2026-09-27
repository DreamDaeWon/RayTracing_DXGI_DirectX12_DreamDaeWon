#include "pch.h"
#include "Weapon_SharkBlood.h"
#include "Effect_Splat_White_Circle.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Effect_Hit_Bullet.h"


CWeapon_SharkBlood::CWeapon_SharkBlood(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CWeapon(pGraphic_Device)
{
}

CWeapon_SharkBlood::CWeapon_SharkBlood(const CWeapon_SharkBlood& rhs) :
	CWeapon(rhs)
{
}
HRESULT CWeapon_SharkBlood::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CPlayer"));
		return E_FAIL;
	}
	m_eID = WEAPON_SHARKBLOOD;
	m_eBulletID = BULLET_SHARKBLOOD;
	m_fShootingTerm = SHARKBLOOD_SHOOTING_TERM;
	m_fTerm = SHARKBLOOD_SHOOTING_TERM;
	m_eShooting_Way = SHOOTING_AUTO;
	m_iMaxBulletNum = 25;
	m_iNowBulletNum = 25;
	m_iExtraBulletNum = 60;
	m_fReboundSize = 3.f;
	m_fCP = 7.f;
	m_iQAimSize = 52;
	
	return S_OK;
}

HRESULT CWeapon_SharkBlood::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CWeapon_Shark"));
		return E_FAIL;
	}

	if (FAILED(Clone_Bullet()))
	{
		MSG_BOX(TEXT("Failed to Clone_Bullet : CWeapon_Shark"));
		return E_FAIL;
	}


	for (int i = 0; i < 5; ++i)
	{
		m_pEffect[i] = dynamic_cast<CEffect_Splat_White_Circle*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Effect_Splat_White_Circle"), i));
	}

	return S_OK;
}

HRESULT CWeapon_SharkBlood::Clone_Bullet()
{
	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 200.f;//?

	for (_uint i = 0; i < m_iMaxBulletNum; i++)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_SharkBlood_Bullet"), TEXT("Prototype_GameObject_Bullet"), &GameObjectDesc)))
		{
			MSG_BOX(TEXT("Failed to Ready_Layer_Bullet : CLevel_GamePlay"));
			return E_FAIL;
		}
	}

	m_vecBullet.reserve(m_iMaxBulletNum);

	list<CGameObject*>* pBulletList = m_pGameInstance->Get_List(LEVEL_STATIC, TEXT("Layer_SharkBlood_Bullet"));
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
		iter->Set_Frame(4.f);
	m_iNowBulletNum = m_iMaxBulletNum;

	return S_OK;
}


void CWeapon_SharkBlood::Shot_Bullet()
{
	if (0 >= m_iNowBulletNum || m_iMaxBulletNum < m_iNowBulletNum)
		return;

	_float3 vRayDir = {};
	_float3 vRayPos = {};
	//m_pGameInstance->Get_World_Mouse_Ray(&vRayDir, &vRayPos);
	Get_World_Mouse_Ray_Shot_Grouping(&vRayDir, &vRayPos);
	m_vecBullet[m_iNowBulletNum - 1]->Set_Pos_Dir(vRayPos, vRayDir);
	//pBullet->Set_Pos_Dir(vWeaponPos, vRayDir);
	m_vecBullet[m_iNowBulletNum - 1]->Set_Life_Time();
	--m_iNowBulletNum;

	m_eCurState = STATE_SHOOTING;
	m_fShootingTerm = SHARKBLOOD_SHOOTING_TERM;
	if (!m_bEx)
	{
		if (rand() % 2 == 0)
			m_pGameInstance->PlaySoundW(TEXT("SharkBlood_Shoot.wav"), CSound_Manager::CHANNEL_WEAPON, 0.12f);
		else
			m_pGameInstance->PlaySoundW(TEXT("SharkBlood_Hit_00.wav"), CSound_Manager::CHANNEL_WEAPON, 0.12f);
	}
	else
		m_pGameInstance->PlaySoundW(TEXT("SharkBlood_Hit_01.wav"), CSound_Manager::CHANNEL_WEAPON, 0.12f);

	for(_int i = 0; i < 5; ++i)
	{
		m_pEffect[i]->Set_Life_Time(i);
	}
	
}

_float CWeapon_SharkBlood::Collision_Bullet_Rect(CCollider_Rect* pCollider_Rect, _float fTimeDelta)
{
	_float fResult(0.f);
	CEffect_Hit_Bullet* pEffectHitBullet = dynamic_cast<CEffect_Hit_Bullet*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Effect_Hit_Bullet")));

	for (size_t i = 0; i < m_iMaxBulletNum; i++)
	{
		if (m_vecBullet[i]->Get_Dead())
			continue;

		_float fLength(0.f), fDistance(0.f);
		_float3 vRayDir = { 0.f,0.f,0.f };
		_float3 vRayPos = { 0.f,0.f,0.f };

		m_vecBullet[i]->Get_RayPos_RayDir_Length(fTimeDelta, &vRayPos, &vRayDir, &fLength);
		if (m_pGameInstance->Collision_Rect_Ray_Same_Space(pCollider_Rect, vRayDir, vRayPos, fLength, &fDistance))
		{
			if (m_bEx)
				Chase_Next_Monster(vRayPos + vRayDir * fDistance, m_vecBullet[i]);
			else
			{
				m_vecBullet[i]->Set_Dead();
			}
			pEffectHitBullet->Show_Explosion(vRayPos + vRayDir * (fDistance - 0.01f), fTimeDelta);
			fResult += m_fCP;
		}
	}
	return fResult;
}

HRESULT CWeapon_SharkBlood::Add_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_SharkBlood"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Weapon_SharkBlood : Add_Components"));
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponEx_SharkBlood"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom1))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_Weapon_Atlas : Add_Components"));
		return E_FAIL;
	}
	return S_OK;
}

void CWeapon_SharkBlood::Chase_Next_Monster(_float3 vPos, CBullet* pBullet)
{
	if (pBullet->Get_Chase())
	{
		pBullet->Set_Chase(false);
		pBullet->Set_Dead();
		return;
	}
		

	CGameObject* pNextMonster = { nullptr };
	list<CGameObject*>* pMonsterList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Monster"));
	if (nullptr == pMonsterList)
		return;

	_float fDistance = { 1000.f };
	_float fLength = { 0.f };
	_float3 vRayDir = { 0.f,1.f,0.f };
	for (auto iter : *pMonsterList)
	{
		if (iter->Get_Dead())
			continue;
		CTransform* pMosnterTranform = dynamic_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
		if (nullptr == pMosnterTranform)
			continue;
		_float3 vMonsterPosition = pMosnterTranform->Get_State(CTransform::STATE_POSITION);
		_float3 vDir = vMonsterPosition - vPos;
		fLength = D3DXVec3Length(&vDir);
		if (fDistance > fLength && fLength > 1.f)
		{
			fDistance = fLength;
			pNextMonster = iter;
			D3DXVec3Normalize(&vRayDir, &vDir);
		}
	}

	if (1000.f == fDistance)
		return;
	_float3 vResultDir = dynamic_cast<CTransform*>(pNextMonster->Get_Component(g_strTransformTag))->Get_State(CTransform::STATE_POSITION) - vPos;
	D3DXVec3Normalize(&vResultDir, &vResultDir);
	pBullet->Set_Pos_Dir(vPos, vResultDir);
	pBullet->Set_Life_Time();
	pBullet->Set_Chase(true);
}

CWeapon_SharkBlood* CWeapon_SharkBlood::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWeapon_SharkBlood* pInstance = new CWeapon_SharkBlood(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWeapon_SharkBlood"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon_SharkBlood::Clone(void* pArg)
{
	CWeapon_SharkBlood* pInstance = new CWeapon_SharkBlood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWeapon_SharkBlood"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_SharkBlood::Free()
{
	__super::Free();
}
