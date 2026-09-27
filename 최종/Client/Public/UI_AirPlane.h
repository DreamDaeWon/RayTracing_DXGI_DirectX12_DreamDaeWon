#pragma once

#include"UI_Base.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_AirPlane final : public CUI_Base
{
public:
	typedef struct UI_AirPlane_Desc : public CUI_Base::UI_BASE_DESC
	{
		class CState*		pPlaneState = { nullptr };
		class CAirPlane* pPlane = { nullptr };
	}UI_AIRPLANE_DESC;
private:
	CUI_AirPlane(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_AirPlane(const CUI_Base& rhs);
	virtual ~CUI_AirPlane() = default;
	enum SKILL_SLOT { SPACE_SKILL, SHIFT_SKILL, SKILL_SLOT_END };

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	void SetScene(_bool _Scene) { m_bScene = _Scene; } // Scene 설정해주는 곳

private:
	_bool								m_bOnce = { false };
	_bool								m_bMove = { false };
	_bool								m_bCome = { false };
	_bool								m_bGo = { false };
	_bool							m_bScene = { false }; // 대원 시네마틱 상황일 때 UI 끄기
	_uint							m_iCurMode = { 0 };
	_uint							m_iPreMode = { 0 };
	_float							m_fTime = { 0.f };
	_float							m_fTimeCome = { 0.f };
	_float							m_fTimeGo = { 0.f };
	_float							m_fMaxBullet = { 0 };
	_float							m_fPreBullet = { 0 };
	_float							m_fCurBullet = { 0 };
	_float							m_fGap = { 0 };
	_float4x4						m_vFirstWorldMatrix = {};

	CTexture*					m_pTextureHp = { nullptr };
	CTexture*					m_pTextureGauge = { nullptr };
	CTexture*					m_pTextureFrame = { nullptr };
	CTexture*					m_pTextureSideBack = { nullptr };
	CTexture*					m_pTextureSideFront= { nullptr };
	CTexture*					m_pTextureMonitor = { nullptr };
	CTexture*					m_pTextureSkill = { nullptr };
	class CState*					m_pPlaneState = { nullptr };
	class CAirPlane*					m_pPlane = { nullptr };

private:
	HRESULT						Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame, _uint iRenderState);
	HRESULT						Reset_First_State();

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;
public:
	static	 CUI_AirPlane* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END