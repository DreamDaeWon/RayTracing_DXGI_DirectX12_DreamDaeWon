#pragma once
#include "Effect_Base.h"

#define EFFECT_SPLAT_ORANGE_CIRCLE_LIFE_TIME 0.5f

#define RIHGT_VIEW_RIGHT 0.06f
#define RIHGT_VIEW_UP 0.02f
#define RIHGT_VIEW_BACK 0.1f

BEGIN(Client)

class CEffect_Orange_Circle final : public CEffect_Base
{
public:
	typedef struct tagEffectSplatWhiteCircleDesc : public CEffect_Base::EFFECT_BASE_DESC {

	}EFFECT_SPLAT_WHITE_CIRCLE_DESC;

private:
	CEffect_Orange_Circle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Orange_Circle(const CEffect_Orange_Circle& rhs);
	virtual ~CEffect_Orange_Circle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Life_Time() {
		m_fLifeTime = EFFECT_SPLAT_ORANGE_CIRCLE_LIFE_TIME;
		m_fFrame = 0.f;
	}

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();


private:
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	_float m_fScale = { 1.f }; // 최종크기는 얼마인지? 

public:
	static CEffect_Orange_Circle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END