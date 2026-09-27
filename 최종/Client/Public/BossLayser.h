#pragma once
#include "LandObject.h"
#include "Client_Defines.h"

#define LIFE_TIME_LAYSER 7.f

BEGIN(Engine)
class CVIBuffer_Cube;
class CCollider_Cube_AABB;
class CTexture;
END


BEGIN(Client)

class CBossLayser final : public CLandObject
{
public:
	typedef struct tagBossLayserDesc : public CLandObject::LANDOBJECT_DESC {
		_float3 vPosition = {};
		_uint iFrame = { 0 };
		_uint iDir = {};
	}BOSS_LAYSER_DESC;

private:
	CBossLayser(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossLayser(const CBossLayser& rhs);
	virtual ~CBossLayser() = default;

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

	_float m_fScale = { 5.f }; // 크기값을 정해 줌
	_float m_fLongX = { 50.f }; // X 축으로 1초에 얼마나 키울건지?

	_uint m_iDir = {}; // 어느 방향으로 이동할건지? 0=오른쪽, 1=왼쪽

	_float m_fLifeTime = { LIFE_TIME_LAYSER }; // 몇 초 후에 죽을건지?


	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Cube* m_pVIBuffer_Com = { nullptr };
	CCollider_Cube_AABB* m_pCollider_Com = { nullptr };

	_uint m_iFrame = { 0 };

public:
	static CBossLayser* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
