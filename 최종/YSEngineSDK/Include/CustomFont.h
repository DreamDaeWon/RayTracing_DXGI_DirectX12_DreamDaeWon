#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCustomFont() = default;

public:
	HRESULT		Ready_Font(const wstring& strFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight);
	void		Render_Font(const wstring&  strText, const _float2* pPos, D3DXCOLOR Color);

private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };
	LPD3DXSPRITE				m_pSprite = { nullptr };
	LPD3DXFONT					m_pFont = { nullptr };

public:
	static CCustomFont*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight);

private:
	virtual void	Free();
};

END