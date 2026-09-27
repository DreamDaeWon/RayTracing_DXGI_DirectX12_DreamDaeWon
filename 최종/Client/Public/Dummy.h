#pragma once
#include "LandObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CDummy final : public CLandObject
{
public:
	typedef struct tagDummy_Desc : public CLandObject::LANDOBJECT_DESC {
		_float3 vPos;
	}DUMMY_DESC;
private:
	CDummy(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

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
	//void SetUp_BillBoard(); // 빌보드 함수 카메라 보도록 고정

private:
	void Collision_Bullet(_float fTimeDelta);

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	//콜라이더 실험 BEGIN.
	CCollider_Rect* m_pCollider_Com[COLLIDER_END] = {nullptr};
	//콜라이더 실험 END.

	_float3 m_vPosition = { 0.f, 0.f, 0.f };


public:
	static CDummy* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END