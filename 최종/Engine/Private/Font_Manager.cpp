#include "..\Public\Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CFont_Manager::Initialize()
{
	return S_OK;
}

HRESULT CFont_Manager::Add_Font(const wstring & strFontTag, const wstring& strFontType, const _uint & iWidth, const _uint & iHeight, const _uint & iWeight)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;	

	CCustomFont*	pFont = CCustomFont::Create(m_pGraphic_Device, strFontType, iWidth, iHeight, iWeight);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

void CFont_Manager::Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 * pPos, D3DXCOLOR Color)
{
	CCustomFont*		pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return;

	pFont->Render_Font(strText, pPos, Color);	
}



CCustomFont * CFont_Manager::Find_Font(const wstring & strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);
	if (iter == m_Fonts.end())
		return nullptr;
	return iter->second;
}

CFont_Manager * CFont_Manager::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFont_Manager*		pInstance = new CFont_Manager(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CFont_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFont_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);
	m_Fonts.clear();

	Safe_Release(m_pGraphic_Device);
}
