#include "GameObject.h"
#include "GameInstance.h"


CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device) :
    m_pGraphic_Device(pGraphic_Device),
    m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pGraphic_Device);
}

CGameObject::CGameObject(const CGameObject& rhs) :
    m_pGraphic_Device(rhs.m_pGraphic_Device),
    m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pGraphic_Device);
}

HRESULT CGameObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        GAMEOBJECT_DESC* pGameObjectDesc = (GAMEOBJECT_DESC*)pArg;
    }

    m_pTransform = CTransform::Create(m_pGraphic_Device);

    if (nullptr == m_pTransform)
    {
        MSG_BOX(TEXT("nullptr == m_pTransform : CGameObject::Initialize"));
        return E_FAIL;
    }
    if (FAILED(m_pTransform->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Initialize : m_pTransform"));
        return E_FAIL;
    }
    
    m_Components.emplace(g_strTransformTag, m_pTransform);
    Safe_AddRef(m_pTransform);

    return S_OK;
}

_uint CGameObject::Tick(_float fTimedelta)
{
    return 0;
}

void CGameObject::Late_Tick(_float fTimedelta)
{
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

CComponent* CGameObject::Get_Component(const wstring& strComTag)
{
    auto iter = m_Components.find(strComTag);
    if (m_Components.end() == iter)
        return nullptr;
    return iter->second;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
    CComponent* pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);

    if (nullptr == pComponent)
    {
        MSG_BOX(TEXT("Failed to Add_Component : CGameObject"));
        return E_FAIL;
    }

    auto iter = m_Components.find(strComponentTag);
    if (iter != m_Components.end())
    {
        MSG_BOX(TEXT("Failed to aready exist : strComponentTag"));
        return E_FAIL;
    }

    m_Components.emplace(strComponentTag, pComponent);

    *ppOut = pComponent;

    Safe_AddRef(pComponent);

    return S_OK;
}



void CGameObject::Free()
{
    for (auto& iter : m_Components)
        Safe_Release(iter.second);
    m_Components.clear();

    Safe_Release(m_pTransform);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pGraphic_Device);

    __super::Free();
}
