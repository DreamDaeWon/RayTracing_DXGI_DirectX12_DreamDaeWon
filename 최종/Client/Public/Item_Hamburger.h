#pragma once
#include "Item.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CItem_Hamburger final : public CItem
{
private:
	CItem_Hamburger(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem_Hamburger(const CItem_Hamburger& rhs);
	virtual ~CItem_Hamburger() = default;

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
	void Item_Collision();

private:
	_uint			m_iFrame = { 0 };
	CTransform* m_pCameraTransform = { nullptr };

public:
	static CItem_Hamburger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

