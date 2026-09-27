#pragma once
#include "Effect_Base.h"

#define EFFECT_EXPLOSION_LIFE_TIME 0.5f

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CEffect_Explosion final: public CEffect_Base
{
public:
	typedef struct tagEffect_Desc : public CEffect_Base::EFFECT_BASE_DESC {

	}EFFECT_EXPLOSION_DESC;

private:
	CEffect_Explosion(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Explosion(const CEffect_Explosion& rhs);
	virtual ~CEffect_Explosion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Position(_float3 vPos) { m_pTransform->Set_State(CTransform::STATE_POSITION, vPos); }
	void Set_Life_Time() {
		m_fLifeTime = EFFECT_EXPLOSION_LIFE_TIME; m_fFrame = 0.f;
		_float fRandomScale = (rand() % 7 + 1) / 10.f + 0.3f;
		m_pTransform->Set_Scale(_float3(fRandomScale, fRandomScale, 0.f));
	}


private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();

private:
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	CTransform* m_pCameraTransform = { nullptr };
public:
	static CEffect_Explosion* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END