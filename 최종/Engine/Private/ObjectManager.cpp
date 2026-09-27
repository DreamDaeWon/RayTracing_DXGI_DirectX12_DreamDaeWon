#include "ObjectManager.h"
#include "GameObject.h"
#include "Layer.h"

CObjectManager::CObjectManager()
{
}

HRESULT CObjectManager::Initialize(_uint iNumLevels)
{
	m_iLevels = iNumLevels;
	
	m_pObjectListMap = new map<const wstring, CLayer*>[iNumLevels]; // 인자로 받은 레벨개수로 맵컨테이너를 생성해주는 것이 포인트.
	//m_pObjectListMap = new map<const wstring, list<class CGameObject*>>[iNumLevels]; // 인자로 받은 레벨개수로 맵컨테이너를 생성해주는 것이 포인트.

	return S_OK;
}

HRESULT CObjectManager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))  //프로토 타입태그를 찾았을 때 >> 이미 있다?
	{
		MSG_BOX(TEXT("nullptr != Find_Prototype : CObjectManager::Add_Prototype"));
		return E_FAIL;
	}

	if (nullptr == pPrototype)
	{
		MSG_BOX(TEXT("nullptr == pPrototype : CObjectManager::Add_Prototype"));
		return E_FAIL;
	}

	m_PrototypeMap.emplace(strPrototypeTag, pPrototype);
	return S_OK;
}

HRESULT CObjectManager::Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (m_iLevels <= iLevelIndex)
	{
		MSG_BOX(TEXT("ERR Index Range,  iLevelIndex: CObjectManager::Add_Clone"));
		return E_FAIL;
	}
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);

	if (nullptr == pPrototype)
	{
		MSG_BOX(TEXT("nullptr != Find_Prototype : CObjectManager::Add_Clone"));
		return E_FAIL;
	}
	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
	{
		MSG_BOX(TEXT("nullptr != pGameObject : CObjectManager::Add_Clone"));
		return E_FAIL;
	}

	CLayer* pLayer = Find_Layers(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
		{
			MSG_BOX(TEXT("Failed to Create CLayer : CObjectManager::Add_Clone"));
			return E_FAIL;
		}
		pLayer->Add_GameObject_In_Layer(pGameObject);

		m_pObjectListMap[iLevelIndex].emplace(strLayerTag, pLayer); //레이어를 생성
	}
	else
		pLayer->Add_GameObject_In_Layer(pGameObject);


	return S_OK;
}

HRESULT CObjectManager::Add_Object(CGameObject* pGameObject, _uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layers(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
		{
			MSG_BOX(TEXT("Failed to Create CLayer : CObjectManager::Add_Clone"));
			return E_FAIL;
		}
		pLayer->Add_GameObject_In_Layer(pGameObject);
		Safe_AddRef(pGameObject);
		m_pObjectListMap[iLevelIndex].emplace(strLayerTag, pLayer); //레이어를 생성
	}
	else
	{
		pLayer->Add_GameObject_In_Layer(pGameObject);
		Safe_AddRef(pGameObject);
	}

	return S_OK;
}

CComponent* CObjectManager::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex)
{
	if (m_iLevels <= iLevelIndex)
	{
		MSG_BOX(TEXT("Range over Err : CObjectManager::Get_Component, iLevelIndex"));
		return nullptr;
	}

	CLayer* pLayer = Find_Layers(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComTag,iIndex);
}

HRESULT CObjectManager::Get_Object(_uint iLevelIndex, const wstring& strLayerTag, CGameObject** ppObject, _uint iIndex)
{
	if (m_iLevels <= iLevelIndex)
	{
		MSG_BOX(TEXT("ERR Index Range,  iLevelIndex: CObjectManager::Get_Object"));
		return E_FAIL;
	}
	CLayer* pLayer = Find_Layers(iLevelIndex, strLayerTag);
	if (nullptr != pLayer)
	{
		if (FAILED(pLayer->Get_Object(ppObject, iIndex)))
		{
			MSG_BOX(TEXT("Failed to Get_Object,   CObjectManager::Get_Object"));
			return E_FAIL;
		}
	}
	else
	{
		MSG_BOX(TEXT("nullptr == pLayer,   CObjectManager::Get_Object"));
		return E_FAIL;
	}
	return S_OK;
		
}

CGameObject* CObjectManager::Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	if (m_iLevels <= iLevelIndex)
	{
		MSG_BOX(TEXT("ERR Index Range,  iLevelIndex: CObjectManager::Get_Object"));
		return nullptr;
	}
	CLayer* pLayer = Find_Layers(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		MSG_BOX(TEXT("nullptr == pLayer,   CObjectManager::Get_Object"));
		return nullptr;
	}
	return pLayer->Get_Object(iIndex);
}

list<class CGameObject*>* CObjectManager::Get_List(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (m_iLevels <= iLevelIndex)
	{
		//MSG_BOX(TEXT("ERR Index Range,  iLevelIndex: CObjectManager::Get_Object"));
		return nullptr;
	}
	CLayer* pLayer = Find_Layers(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		//MSG_BOX(TEXT("nullptr == pLayer,  Find_Layers: CObjectManager::Get_List"));
		return nullptr;
	}
	return pLayer->Get_List();
}


void CObjectManager::Tick(_float fTimeDelta)
{
	for (size_t i = 0; m_iLevels > i; ++i)
	{
		for (auto& iter : m_pObjectListMap[i])
		{
			if (m_pObjectListMap[i].empty())
				break;
			iter.second->Tick(fTimeDelta);
		}
		/*for (auto& iter : m_pObjectListMap[m_iLevels])
		{
			for (auto& ObjIter : iter.second)
				ObjIter->Tick(fTimeDelta);
		}*/
	}
	

}

void CObjectManager::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; m_iLevels > i; ++i)
	{
		for (auto& iter : m_pObjectListMap[i]) //iLevels
		{
			iter.second->Late_Tick(fTimeDelta);
		}
		/*for (auto& iter : m_pObjectListMap[i])
		{
			for (auto& ObjIter : iter.second)
				ObjIter->Late_Tick(fTimeDelta);
		}*/
	}

}

void CObjectManager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pObjectListMap[iLevelIndex])
		Safe_Release(Pair.second);
	m_pObjectListMap[iLevelIndex].clear();

	/*for (auto& iter : m_PrototypeMap)
		Safe_Release(iter.second);
	m_PrototypeMap.clear();*/
}


CGameObject* CObjectManager::Find_Prototype(const wstring& strPrototypeTag)
{
	auto iter = m_PrototypeMap.find(strPrototypeTag);

	if (iter == m_PrototypeMap.end())
	{
		//MSG_BOX(TEXT("Failed to find : strPrototypeTag"));
		return nullptr;
	}

	return iter->second;
}

CLayer* CObjectManager::Find_Layers(_uint iLevelIndex, const wstring& strLayerTag)
{
	auto iter = m_pObjectListMap[iLevelIndex].find(strLayerTag);

	if (iter == m_pObjectListMap[iLevelIndex].end())
	{
		//MSG_BOX(TEXT("Failed to find : strLayerTag"));
		return nullptr;
	}

	return iter->second;
}

CObjectManager* CObjectManager::Create(_uint iNumLevels)
{
	CObjectManager* pInstance = new CObjectManager;
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObjectManager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObjectManager::Free()
{
	for (size_t i = 0; m_iLevels > i; ++i)
	{
		for (auto& iter : m_pObjectListMap[i])
		{
			Safe_Release(iter.second);
		/*	for (auto& ObjIter : iter.second)
				Safe_Release(ObjIter);
			iter.second.clear();*/
		}
		m_pObjectListMap[i].clear();
	}

	for (auto& iter : m_PrototypeMap)
		Safe_Release(iter.second);
	m_PrototypeMap.clear();

	Safe_Delete_Array(m_pObjectListMap);
}
