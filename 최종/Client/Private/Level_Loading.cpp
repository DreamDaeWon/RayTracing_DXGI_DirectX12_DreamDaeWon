#include "pch.h"
#include "GameInstance.h"
#include "Level.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Normal1.h"
#include "Level_Normal2.h"
#include "Level_1945.h"
#include "Level_SnowBoss.h"
#include "Loader.h"
#include"Loading.h"

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLevel(pGraphic_Device) 
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	if (FAILED(__super::Initialize()))
	{
		MSG_BOX(TEXT("Failed to Initialize : __super,CLevel_Loading"));
		return E_FAIL;
	}
	
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pGraphic_Device, eNextLevelID);
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_Logo"));
		return E_FAIL;
	}
    return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if(nullptr == m_pLoader)
	{
		MSG_BOX(TEXT("nullptr : m_pLoader"));
		return;
	}
	else if(nullptr == m_pGameInstance)
	{
		MSG_BOX(TEXT("nullptr : m_pGameInstance"));
		return;
	}
	
	__super::Tick(fTimeDelta);

	if (m_pLoader->isFinished())
	{
		CLevel* pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case Client::LEVEL_STATIC:
			break;
		case Client::LEVEL_LOADING:
			break;
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pGraphic_Device);
			break;
		case LEVEL_TUTORIAL:
			pLevel = CLevel_GamePlay::Create(m_pGraphic_Device);
			break;
		case Client::LEVEL_NORMAL1:
			pLevel = CLevel_Normal1::Create(m_pGraphic_Device);
			break;
		case Client::LEVEL_1945:
			pLevel = CLevel_1945::Create(m_pGraphic_Device);
			break;
		case Client::LEVEL_NORMAL2:
			pLevel = CLevel_Normal2::Create(m_pGraphic_Device);
			break;
		case LEVEL_SNOWBOSS:
			pLevel = CLevel_SnowBoss::Create(m_pGraphic_Device);
			break;
		case Client::LEVEL_END:
			break;
		default:
			break;
		}

		m_pGameInstance->Open_Level(m_eNextLevelID, pLevel);
	}

}

HRESULT CLevel_Loading::Render()
{

	if(FAILED(__super::Render()))
	{
		MSG_BOX(TEXT("Faild to __super::Render : CLevel_Loading"));
		return E_FAIL;
	}

	if(nullptr == m_pLoader)
	{
		MSG_BOX(TEXT("nullptr == m_pLoader : CLevel_Loading"));
		return E_FAIL;
	}
	m_pLoader->Output();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	CLoading::LOADING_DESC  Loading_desc = {};
	Loading_desc.pLoader = m_pLoader;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LOADING, strLayerTag, TEXT("Prototype_GameObject_Loading"),&Loading_desc)))
	{
		MSG_BOX(TEXT("Failed to Ready_Layer_BackGround : CLevel_Logo"));
		return E_FAIL;
	}


	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pGraphic_Device);
	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	Safe_Release(m_pLoader);
	__super::Free(); 

}
