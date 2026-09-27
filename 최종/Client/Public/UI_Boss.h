#pragma once

#include"UI_Base.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Boss final : public CUI_Base
{
public:
	typedef struct tagUI_Boss_Desc : CUI_Base::UI_BASE_DESC
	{
		class CState* pBossStateCom = { nullptr };
	}UI_BOSS_DESC;

private:
	CUI_Boss(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Boss(const CUI_Base& rhs);
	virtual ~CUI_Boss() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
private:
	_bool							m_bMove = { false };
	_bool							m_bHit = { false };
	_float							m_fMaxHp = { 0.f };
	_float							m_fCurHp = { 0.f };
	_float							m_fPreHp = { 0.f };
	_float							m_fPreHpForY = { 0.f };
	_float							m_fGap = { 0.f };

	_float							m_fYellowTime = { 0.f };
	_float							m_fTime = { 0.f };
	_float							m_fTime1 = { 0.f };
	_float4x4						m_vFirstWorldMatrix = {};
	CTexture*						m_pTextureHp = { nullptr }; 
	CTexture*						m_pTextureGroggy = { nullptr };
	class CState*					m_pBossStateCom = { nullptr };
private:
	HRESULT							Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame, _uint iRenderState);
	void							Reset_First_State();

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;
public:
	static	 CUI_Boss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END