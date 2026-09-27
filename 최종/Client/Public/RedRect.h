#pragma once
#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
//콜라이더 실험 BEGIN.
class CCollider_Rect;
//콜라이더 실험 END.
END

BEGIN(Client)

class CRedRect final : public CLandObject
{
public:
	typedef struct tagRedRect_Desc : public CLandObject::LANDOBJECT_DESC {
		_float3 vPosition; // 위치
		_float fLifeTime = 1.f; // 몇 초 뒤에 사라질 건지?
		_float3 vScale = { 1.f, 1.f, 1.f }; // 크기
		_uint iDir = 1;
		_bool bDead = false; // 처음에 어떤 값을 넣어줄 지
	}RED_RECT_DESC;

private:
	CRedRect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRedRect(const CRedRect& rhs);
	virtual ~CRedRect() = default;

public:
	void Set_LifeTime() {
		m_fLifeTime = m_fSetLifeTime;
		Set_Dead();
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void InitializeDW(void* pArg);

	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

private:
	void SteamAttack();
	void LaserAttak(_uint _Dir);

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

	class CSteamAttack* m_pSteamAttack = { nullptr };

	_float3		m_vPos = {};
	_float		m_fScale = { 0.f };

	_float3 m_vScale = {};

	_float m_fLifeTime = { 1.f };

	_float m_fSetLifeTime = {}; // 몇초로 시간을 설정해 줄건지?

	_uint m_iDir = {};

public:
	static CRedRect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END