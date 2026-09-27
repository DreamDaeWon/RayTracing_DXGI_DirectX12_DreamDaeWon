#pragma once

#include"UI_Base.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Player_Skill final : public CUI_Base
{
private:
	enum UI { UI_BACK, UI_BACK_DOT, UI_SPACE, UI_SHIFT, UI_F, UI_Q, UI_, UI_END };
	enum SKILL_SLOT { SPACE_SKILL, SHIFT_SKILL, Q_SKILL, F_SKILL, SKILL_SLOT_END };
public:
	typedef struct tagUI_Player_Skill_Desc : public CUI_Base::UI_BASE_DESC
	{
		_float* pPlayerSp = { nullptr };
		_float* pPlayerMaxSp = { nullptr };
	}UI_PLAYER_SKILL_DESC;


private:
	CUI_Player_Skill(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Player_Skill(const CUI_Base& rhs);
	virtual ~CUI_Player_Skill() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	void Get_State();
private:
	
	_bool							m_bEnd = { false };
	_bool							m_bMove = { false };

	_float						m_fFrame = { 0.f };
	_float						m_fCurSP = { 0.f };
	_float						m_fPreSP = { 0.f };
	_float						m_fTime = { 0.f };
	_float						m_fGap = { 0.f };

	UI								m_eCurUI = { UI_SHIFT };
	_float*						 m_pPlayerSp = { nullptr };
	_float*						 m_pPlayerMaxSp = { nullptr };


	CTexture*					m_pTextureSkillCom = { nullptr };  // 스킬 - 프레임이 곧 그 스킬의 이미지
	CTexture*					m_pTextureSkillCoolCom = { nullptr };  // 스킬 - 쿨탐
	CTexture*					m_pTextureBackCom = { nullptr };  // background - 
	CTexture*					m_pTextureBackDotCom = { nullptr };  // background_dot
	CTexture*					m_pTextureSkillGaugeCom = { nullptr };  //게이지 프레임

	_float4x4					m_FirstWorld = {};
	_float3						m_vScale = {};

	class CPlayer_Skill*	m_pSkillSlot[SKILL_SLOT_END] = {nullptr};
	_float					m_pCoolTime[SKILL_SLOT_END] = {0.f};
private:
	HRESULT							Render_Again(_float fSizeX, _float fSizeY, _float fX, _float fY, _uint iFrame, _uint iRenderState);
	void							Reset_First_State();

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;
public:
	static	 CUI_Player_Skill* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END