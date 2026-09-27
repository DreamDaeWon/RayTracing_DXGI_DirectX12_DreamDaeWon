#pragma once

#include"UI_Base.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Ending final : public CUI_Base
{
public:

private:
	CUI_Ending(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Ending(const CUI_Base& rhs);
	virtual ~CUI_Ending() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
public:
	void Set_Start(_bool bStart) { m_bStart = bStart; }
private:
	_bool								m_bStart = { false };
	_bool								m_bMove = { false };
	_bool								m_bTitle = { false };
	_float							m_fTime = { 0.f };
	_float							m_fTimeMove = { 0.f };
	_float							m_fTimeEnding = { 0.f };
	_float4x4						m_vFirstWorldMatrix = {};
	CTexture*						m_pTextureEnding = { nullptr }; 
	CTexture*						m_pTextureLogo = { nullptr };
private:
	HRESULT							Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame, _uint iRenderState);
	void							Reset_First_State();

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;
public:
	static	 CUI_Ending* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END