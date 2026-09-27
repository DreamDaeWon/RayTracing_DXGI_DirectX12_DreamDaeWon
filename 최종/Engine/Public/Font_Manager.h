#pragma once

#include "Base.h"


BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();

public:
	HRESULT		Add_Font( const wstring& strFontTag,
							const wstring& strFontType,
							const _uint& iWidth, 
							const _uint& iHeight, 
							const _uint& iWeight);

	void		Render_Font(const wstring& strFontTag,
							const wstring& strText,
							const _float2* pPos,
							D3DXCOLOR Color);

private:
	LPDIRECT3DDEVICE9								m_pGraphic_Device = { nullptr };

private:
	map<const wstring, class CCustomFont*>			m_Fonts;

private:
	class CCustomFont*		Find_Font(const wstring& strFontTag);

public:
	static CFont_Manager* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual	void Free() override;
};

END
