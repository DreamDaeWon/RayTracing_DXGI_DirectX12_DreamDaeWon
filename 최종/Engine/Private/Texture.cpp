#include "Texture.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CComponent(pGraphic_Device)
{
}

CTexture::CTexture(const CTexture& rhs)
    :CComponent(rhs),
    m_iNumTextures(rhs.m_iNumTextures),
    m_Textures(rhs.m_Textures)
{
    for (auto& pTexture : m_Textures)
        Safe_AddRef(pTexture);
}



HRESULT CTexture::Initialize_Prototype(TYPE eType, const wstring& strFullPath, _uint iNumTextures)
{
    for (size_t i = 0; iNumTextures > i; ++i)
    {
        LPDIRECT3DBASETEXTURE9  pTexture = { nullptr };

        _tchar szFullPath[MAX_PATH] = TEXT("");

        wsprintf(szFullPath, strFullPath.c_str(), i);

        /*HRESULT hr = TYPE_TEX2D == eType ? D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture) : D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);

        if (FAILED(hr))
            return E_FAIL;*/
        switch (eType)
        {
        case Engine::CTexture::TYPE_TEX2D:
            if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture)))
            {
                MSG_BOX(TEXT("Failed to D3DXCreateTextureFromFile : CTexture"));
                return E_FAIL;
            }
            break;
        case Engine::CTexture::TYPE_TEXCUBE:
            if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
            {
                MSG_BOX(TEXT("Failed to D3DXCreateCubeTextureFromFile : CTexture"));
                return E_FAIL;
            }
            break;
        case Engine::CTexture::TYPE_END:
            return E_FAIL;
        default:
            return E_FAIL;
        }
        m_Textures.push_back(pTexture);
    }
    m_iNumTextures = iNumTextures;

    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CTexture::Bind_Texture(_ulong dwStage, _uint iTextureIndex)
{
    if (iTextureIndex >= m_iNumTextures)
    {
        MSG_BOX(TEXT("Failed to iTextureIndex >= m_iNumTextures : CTexture"));
        return E_FAIL;
    }

    return m_pGraphic_Device->SetTexture(dwStage, m_Textures[iTextureIndex]);
}

CTexture* CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eType, const wstring& strFullPath, _uint iNumTextures)
{
    CTexture* pInstance = new CTexture(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype(eType, strFullPath, iNumTextures)))
    {
        MSG_BOX(TEXT("Failed to Created : CTexture"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CTexture"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTexture::Free()
{
    for (auto& pTexture : m_Textures)
        Safe_Release(pTexture);

    m_Textures.clear();

    __super::Free();
}
