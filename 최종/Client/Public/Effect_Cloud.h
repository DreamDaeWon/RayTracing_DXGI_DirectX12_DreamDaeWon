#pragma once
#include "Effect_Base.h"

#define EFFECT_PLAYER_MOVE_LIFE_TIME 0.5f

BEGIN(Client)

class CEffect_Cloud final: public CEffect_Base
{
public:
	typedef struct tagEffectCloudDesc : public CEffect_Base::EFFECT_BASE_DESC {

	}EFFECT_CLOUD_DESC;

private:
	CEffect_Cloud(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Cloud(const CEffect_Cloud& rhs);
	virtual ~CEffect_Cloud() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	//플레이어가 움직이는 상태일때 살아남.
	void Set_Position(_float3 vPos) { m_pTransform->Set_State(CTransform::STATE_POSITION, vPos); }
	void Set_Life_Time() { m_fLifeTime = EFFECT_PLAYER_MOVE_LIFE_TIME; }

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();


private:
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	CTransform* m_pCameraTransformCom = { nullptr };

public:
	static CEffect_Cloud* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END