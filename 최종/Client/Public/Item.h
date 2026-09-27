#pragma once
#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CItem abstract : public CLandObject
{
public:
	typedef struct tagItem_Desc : public CLandObject::LANDOBJECT_DESC {
		_float3 vPos;
		_float3 vLook = { 0.f, 0.f, 1.f };
	}ITEM_DESC;

protected:
	CItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:

	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	CCollider*		m_pCollider_Com[COLLIDER_END] = {nullptr};
	
	_float3			m_vPosition = { 0.f, 0.f, 0.f };
	_float3			m_vLook = {0.f, 1.f, 0.f};

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END

