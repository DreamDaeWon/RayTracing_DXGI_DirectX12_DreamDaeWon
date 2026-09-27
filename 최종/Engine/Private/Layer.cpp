#include "Layer.h"
#include "GameObject.h"
CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	auto iter = m_GameObjectList.begin();
	for (;
		iter != m_GameObjectList.end();)
	{
		_uint	iResult = (*iter)->Tick(fTimeDelta);
		if (1 == iResult)
		{
			Safe_Release(*iter);
			iter = m_GameObjectList.erase(iter);
		}
		else
			++iter;
	}
		
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_GameObjectList)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CLayer::Add_GameObject_In_Layer(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
	{
		MSG_BOX(TEXT("Failed to Add_GameObject_In_Layer : CLayer"));
		return E_FAIL;
	}

	m_GameObjectList.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Get_Object(CGameObject** ppGameObject, _uint iIndex)
{
	if (m_GameObjectList.size() <= iIndex || m_GameObjectList.empty())
	{
		MSG_BOX(TEXT("Failed to Get_Object"));
		return E_FAIL;
	}
	auto iter = m_GameObjectList.begin();
	for (size_t i = 0; i < iIndex; i++)
		iter++;

	*ppGameObject = (*iter);
	Safe_AddRef(*ppGameObject);
	return S_OK;
}

CGameObject* CLayer::Get_Object(_uint iIndex)
{
	if (m_GameObjectList.size() <= iIndex || m_GameObjectList.empty())
	{
		MSG_BOX(TEXT("Range over Err : Get_Object "));
		return nullptr;
	}

	auto iter = m_GameObjectList.begin();
	for (size_t i = 0; i < iIndex; i++)
		iter++;

	return (*iter);
}

CComponent* CLayer::Get_Component(const wstring& strComTag, _uint iIndex)
{
	if (m_GameObjectList.size() <= iIndex || m_GameObjectList.empty())
	{
		MSG_BOX(TEXT("Range over Err : Get_Component "));
		return nullptr;
	}
	
	auto iter = m_GameObjectList.begin();
	for (size_t i = 0; i < iIndex; i++)
		iter++;

	return (*iter)->Get_Component(strComTag);
}

CLayer* CLayer::Create()
{
	CLayer* pInstance = new CLayer;
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& iter : m_GameObjectList)
		Safe_Release(iter);
	m_GameObjectList.clear();
}
