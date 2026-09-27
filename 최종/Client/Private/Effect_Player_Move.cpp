#include "pch.h"
#include "Effect_Player_Move.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "Effect_Cloud.h"

CEffect_Player_Move::CEffect_Player_Move(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CGameObject(pGraphic_Device)
{
}

CEffect_Player_Move::CEffect_Player_Move(const CEffect_Player_Move& rhs) :
	CGameObject(rhs)
{
}

HRESULT CEffect_Player_Move::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Initialize_Prototype : __super,CPlayer"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Player_Move::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CEffect_Player_Move"));
		return E_FAIL;
	}

	m_iMaxEffectNum = 10;

	if (FAILED(Clone_Explosion()))
	{
		MSG_BOX(TEXT("Failed to Clone_Bullet : CEffect_Player_Move"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Player_Move::Clone_Explosion()
{
	//_uint iLevelIndex = m_pGameInstance->Get_Level();
	for (_uint i = 0; i < m_iMaxEffectNum; i++)
	{
		CEffect_Cloud::EFFECT_CLOUD_DESC tEffectCloudDesc = {};
		tEffectCloudDesc.fFrame = 0.f;
		tEffectCloudDesc.fMaxFrame = 0.f;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Cloud"), TEXT("Prototype_GameObject_Effect_Cloud"),&tEffectCloudDesc)))
		{
			MSG_BOX(TEXT("Failed to Ready_Layer_Bullet : CLevel_GamePlay"));
			return E_FAIL;
		}
	}

	list<CGameObject*>* pCloudList = m_pGameInstance->Get_List(LEVEL_STATIC, TEXT("Layer_Cloud"));
	if (nullptr == pCloudList)
		return E_FAIL;

	m_vecEffectCloud.reserve(m_iMaxEffectNum);

	auto iter = (*pCloudList).begin();
	for (_uint i = 0; i < m_iMaxEffectNum; i++)
	{
		if (iter == (*pCloudList).end())
			break;
		m_vecEffectCloud.push_back((CEffect_Cloud*)(*iter));
		Safe_AddRef((*iter));
		iter++;
	}
	/*for (_uint i = 0; i < m_iMaxEffectNum; i++)
	{
		CEffect_Cloud::EFFECT_CLOUD_DESC tEffectCloudDesc = {};
		tEffectCloudDesc.fFrame = 0.f;
		tEffectCloudDesc.fMaxFrame = 0.f;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TUTORIAL, TEXT("Layer_Cloud"), TEXT("Prototype_GameObject_Effect_Cloud"),&tEffectCloudDesc)))
		{
			MSG_BOX(TEXT("Failed to Ready_Layer_Bullet : CLevel_GamePlay"));
			return E_FAIL;
		}
	}

	list<CGameObject*>* pCloudList = m_pGameInstance->Get_List(LEVEL_TUTORIAL, TEXT("Layer_Cloud"));
	if (nullptr == pCloudList)
		return E_FAIL;

	m_vecEffectCloud.reserve(m_iMaxEffectNum);

	auto iter = (*pCloudList).begin();
	for (_uint i = 0; i < m_iMaxEffectNum; i++)
	{
		if (iter == (*pCloudList).end())
			break;
		m_vecEffectCloud.push_back((CEffect_Cloud*)(*iter));
		Safe_AddRef((*iter));
		iter++;
	}*/

	return S_OK;
}

void CEffect_Player_Move::Show_Explosion(_float3 vPos, _float fTimeDelta)
{
	/*if (0 >= m_iNowBulletNum || m_iMaxBulletNum < m_iNowBulletNum)
		return;*/

	m_fShowTerm += fTimeDelta;
	if (0.1f > m_fShowTerm)
		return;

	m_fShowTerm = 0.f;
	m_vecEffectCloud[m_iVectorCursor]->Set_Life_Time();
	m_vecEffectCloud[m_iVectorCursor]->Set_Position(vPos);
	++m_iVectorCursor;
	if (m_iMaxEffectNum <= m_iVectorCursor)
		m_iVectorCursor = 0;
}

CEffect_Player_Move* CEffect_Player_Move::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Player_Move* pInstance = new CEffect_Player_Move(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Player_Move"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Player_Move::Clone(void* pArg)
{
	CEffect_Player_Move* pInstance = new CEffect_Player_Move(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect_Player_Move"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Player_Move::Free()
{
	for (auto& iter : m_vecEffectCloud)
		Safe_Release(iter);
	m_vecEffectCloud.clear();
	__super::Free();
}
