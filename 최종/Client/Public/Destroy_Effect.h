#pragma once
#include "Effect_Base.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CDestroy_Effect final : public CEffect_Base
{
public:
	typedef struct tagDestroyEffect_Desc : public CEffect_Base::EFFECT_BASE_DESC {
		_float3 vPosition = { 0.f,0.f,0.f };
		_uint iWeaponID = { 0 };
		_float fScale = { 0.f };
	}DESTROY_EFFECT_DESC;

private:
	CDestroy_Effect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDestroy_Effect(const CDestroy_Effect& rhs);
	virtual ~CDestroy_Effect() = default;

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
	void Second_Explosion(_uint iCount);

private:
	_float3 m_vPosition = { 0.f, 0.f, 0.f };
	_float m_fScale = { 0.f };

	CTransform* m_pCameraTransform = { nullptr };

public:
	static CDestroy_Effect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END