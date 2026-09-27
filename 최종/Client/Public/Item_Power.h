#pragma once
#include "Item.h"

#define POWER_SPRITE_SPEED 5.f
#define RESPAWN_TIME 3.f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CItem_Power final : public CItem
{
private:
	CItem_Power(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem_Power(const CItem_Power& rhs);
	virtual ~CItem_Power() = default;

public:
	void Set_Life_Time() {
		m_fLifeTime = RESPAWN_TIME;
		m_bDead = true;
	}

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

private:
	void Item_Collision();
	void SetUp_BillBoard();

private:
	CTransform* m_pCameraTransform = { nullptr };

	_float			m_fFrame = { 0.f };
	_float			m_fMaxFrame = { 0.f };
	_float			m_fLifeTime = { 0.f };

public:
	static CItem_Power* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

