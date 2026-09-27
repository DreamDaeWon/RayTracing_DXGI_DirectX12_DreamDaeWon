#pragma once
#include "LandObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider_Cube_AABB;
class CCollider;
class CVIBuffer_Cube;
class CTexture;
END


BEGIN(Client)

class CObject_Box final:  public CLandObject
{
public:
	typedef struct tagObjectBoxDesc : public CLandObject::LANDOBJECT_DESC {
		_float3 vPosition = {};
		_uint iFrame = { 0 };
	}OBJ_BOX_DESC;

private:
	CObject_Box(LPDIRECT3DDEVICE9 pGraphic_Device);
	CObject_Box(const CObject_Box& rhs);
	virtual ~CObject_Box() = default;

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

	void Collision_Bullet(_float fTimeDelta);
	void Collider_Billboarding();

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Cube* m_pVIBuffer_Com = { nullptr };
	CCollider* m_pCollider_Com[COLLIDER_END] = { nullptr };
	_uint m_iFrame = { 0 };

public:
	static CObject_Box* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END