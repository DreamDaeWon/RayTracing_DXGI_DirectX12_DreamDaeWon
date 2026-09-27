#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphic_Device(pGraphicDev)
	, m_pSprite(nullptr)
	, m_pFont(nullptr)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CCustomFont::Ready_Font(const wstring& strFontType, const _uint & iWidth, const _uint & iHeight, const _uint & iWeight)
{
	D3DXFONT_DESC			Font_Desc;
	ZeroMemory(&Font_Desc, sizeof(D3DXFONT_DESC));

	Font_Desc.CharSet = HANGEUL_CHARSET;
	lstrcpy(Font_Desc.FaceName, strFontType.c_str());

	Font_Desc.Width = iWidth;
	Font_Desc.Height = iHeight;
	Font_Desc.Weight = iWeight;

	if (FAILED(D3DXCreateFontIndirect(m_pGraphic_Device, &Font_Desc, &m_pFont)))
		return E_FAIL;
	if (FAILED(D3DXCreateSprite(m_pGraphic_Device, &m_pSprite)))
		return E_FAIL;
	
	return S_OK;
}

void CCustomFont::Render_Font(const wstring& strText, const _float2 * pPos, D3DXCOLOR Color)
{
	RECT	rc = { (long)pPos->x, (long)pPos->y };

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, strText.c_str(), strText.length(), &rc, DT_NOCLIP, Color);

	m_pSprite->End();
}

CCustomFont * CCustomFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strFontType, const _uint & iWidth, const _uint & iHeight, const _uint & iWeight)
{
	CCustomFont *	pInstance = new CCustomFont(pGraphicDev);

	if (FAILED(pInstance->Ready_Font(strFontType, iWidth, iHeight, iWeight)))
	{
		MSG_BOX(TEXT("MyFont Create Failed"));

		Safe_Release(pInstance);	
	}

	return pInstance;
}



void CCustomFont::Free()
{
	Safe_Release(m_pFont);
	Safe_Release(m_pSprite);

	Safe_Release(m_pGraphic_Device);
}
