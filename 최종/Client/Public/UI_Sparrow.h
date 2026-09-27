#pragma once

#include"UI_Base.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Sparrow final : public CUI_Base
{
private:
	CUI_Sparrow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Sparrow(const CUI_Base& rhs);
	virtual ~CUI_Sparrow() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_bool						m_bOnce = { false };
	_float						m_fTime = { 0.f };
	_float4x4					m_vFirstWorldMatrix = {};

	CTexture*					m_pTextureZoomUI0 = { nullptr };
	CTexture*					m_pTextureZoomUI1 = { nullptr };
	CTexture*					m_pTextureZoomUI2 = { nullptr };
	CTexture*					m_pTextureZoomUI3 = { nullptr };
	CTexture*					m_pTextureZoomUI4 = { nullptr };
	CTexture*					m_pTextureZoomUI5 = { nullptr };

private:
	HRESULT						Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame, _uint iRenderState);
	HRESULT						Reset_First_State();

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;
public:
	static	 CUI_Sparrow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END