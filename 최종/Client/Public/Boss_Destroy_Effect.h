#pragma once
#include "Effect_Base.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CBoss_Destroy_Effect final : public CEffect_Base
{
public:
	typedef struct tagBossDestroyEffect_Desc : public CEffect_Base::EFFECT_BASE_DESC {
		_float3 vPosition = { 0.f,0.f,0.f };
		_uint iWeaponID = { 0 };
		_float fScale = { 0.f };
	}BOSS_DESTROY_EFFECT_DESC;

private:
	CBoss_Destroy_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss_Destroy_Effect(const CBoss_Destroy_Effect& rhs);
	virtual ~CBoss_Destroy_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();

private:
	_float3 m_vPosition = { 0.f, 0.f, 0.f };
	_float m_fFrameSpeed = { 0.f };
	_float m_fScale = { 0.f };

	CTransform* m_pCameraTransform = { nullptr };

public:
	static CBoss_Destroy_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END