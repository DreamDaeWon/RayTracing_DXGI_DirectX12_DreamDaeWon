#pragma once
#include "Effect_Base.h"

#define EFFECT_SPLAT_RAMBO_LIFE_TIME 0.1f

#define RIHGT_VIEW_RIGHT 0.06f
#define RIHGT_VIEW_UP 0.02f
#define RIHGT_VIEW_BACK 0.1f

BEGIN(Client)

class CEffect_Splat_Rambo final : public CEffect_Base
{
public:
	typedef struct tagEffectSplatWhiteCircleDesc : public CEffect_Base::EFFECT_BASE_DESC {

	}EFFECT_SPLAT_WHITE_CIRCLE_DESC;

private:
	CEffect_Splat_Rambo(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Splat_Rambo(const CEffect_Splat_Rambo& rhs);
	virtual ~CEffect_Splat_Rambo() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Life_Time(_int _iScale) {
		m_fLifeTime = EFFECT_SPLAT_RAMBO_LIFE_TIME;
		m_fFrame = 0.f;
		m_fScaleCircle = (_iScale * 0.1f);
	}

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();


private:
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	_float m_fScaleCircle = { 0.2f }; // 한번에 5개를 돌릴 때 원끼리 얼마나 떨어뜨릴건지?
	_float m_fScale = { 0.5f }; // 최종크기는 얼마인지? 

public:
	static CEffect_Splat_Rambo* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END