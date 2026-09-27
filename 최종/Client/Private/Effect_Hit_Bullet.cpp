#include "pch.h"
#include "Effect_Hit_Bullet.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Explosion.h"

CEffect_Hit_Bullet::CEffect_Hit_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CGameObject(pGraphic_Device)
{
}

CEffect_Hit_Bullet::CEffect_Hit_Bullet(const CEffect_Hit_Bullet& rhs) :
	CGameObject(rhs)
{
}

HRESULT CEffect_Hit_Bullet::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CPlayer"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Hit_Bullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CEffect_Hit_Bullet"));
		return E_FAIL;
	}

	m_iMaxEffectNum = 30;
	m_fShowTerm = 0.f;

	if (FAILED(Clone_Explosion()))
	{
		MSG_BOX(TEXT("Failed to Clone_Bullet : CEffect_Hit_Bullet"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Hit_Bullet::Clone_Explosion()
{
	for (_uint i = 0; i < m_iMaxEffectNum; i++)
	{
		CEffect_Explosion::EFFECT_EXPLOSION_DESC tEffectExplosionDesc = {};
		tEffectExplosionDesc.fFrame = 0.f;
		tEffectExplosionDesc.fMaxFrame = 9.f;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Explosion"), TEXT("Prototype_GameObject_Effect_Explosion"),&tEffectExplosionDesc)))
		{
			MSG_BOX(TEXT("Failed to Clone_Explosion : CEffect_Hit_Bullet"));
			return E_FAIL;
		}
	}

	list<CGameObject*>* pExplosionList = m_pGameInstance->Get_List(LEVEL_STATIC, TEXT("Layer_Explosion"));
	if (nullptr == pExplosionList)
		return E_FAIL;

	m_vecEffectExplosion.reserve(m_iMaxEffectNum);

	auto iter = (*pExplosionList).begin();
	for (_uint i = 0; i < m_iMaxEffectNum; i++)
	{
		if (iter == (*pExplosionList).end())
			break;
		m_vecEffectExplosion.push_back((CEffect_Explosion*)(*iter));
		Safe_AddRef((*iter));
		iter++;
	}

	return S_OK;
}

void CEffect_Hit_Bullet::Show_Explosion(_float3 vPos, _float fTimeDelta)
{
	/*if (0 >= m_iNowBulletNum || m_iMaxBulletNum < m_iNowBulletNum)
		return;*/

	/*m_fShowTerm += fTimeDelta;
	if (0.01f > m_fShowTerm)
		return;*/

	m_fShowTerm = 0.f;
	m_vecEffectExplosion[m_iVectorCursor]->Set_Life_Time();
	m_vecEffectExplosion[m_iVectorCursor]->Set_Position(vPos);
	++m_iVectorCursor;
	if (m_iMaxEffectNum <= m_iVectorCursor)
		m_iVectorCursor = 0;
}

CEffect_Hit_Bullet* CEffect_Hit_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Hit_Bullet* pInstance = new CEffect_Hit_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Hit_Bullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Hit_Bullet::Clone(void* pArg)
{
	CEffect_Hit_Bullet* pInstance = new CEffect_Hit_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Hit_Bullet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Hit_Bullet::Free()
{
	for (auto& iter : m_vecEffectExplosion)
		Safe_Release(iter);
	m_vecEffectExplosion.clear();
	__super::Free();
}
