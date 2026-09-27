#pragma once
#include "Item.h"

#define SPRITE_SPEED 5.f
#define ITEM_TIME 20.f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CItem_1945 final : public CItem
{
private:
	CItem_1945(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem_1945(const CItem_1945& rhs);
	virtual ~CItem_1945() = default;

public:
	void Set_Life_Time() {
		m_fLifeTime = ITEM_TIME;
		m_bDead = false;
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

	void Do_State(_float fTimeDelta);

private:
	CTransform* m_pCameraTransform = { nullptr };

	_float			m_fFrame = { 0.f };
	_float			m_fMaxFrame = { 0.f };
	_float			m_fLifeTime = { 0.f };
	_float			m_fX = { 0.f };
	_float			m_fZ = { 0.f };

public:
	static CItem_1945* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

