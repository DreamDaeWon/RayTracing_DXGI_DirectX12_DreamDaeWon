#pragma once
//#include "Base.h"
#include "Transform.h"

/* 클라이언트 개발자가 만들어야할 게임오브젝트들(플레이어몬스터)의 부모가되는 클래스다. */
/* 객체들을 만들 때 Prototype이라는 디자인패턴을 활용하여 객체를 생성한다. */
/* Prototype : 원형. */
/* 로딩간에 다음레벨에 필요한 객체의 원형을 미리 생성한다. */
/* 로딩이 끝나고 실제 사용하고자하는 레벨에 진입하게되면 레벨에 필요한 실제 객체를 Create(x), Clone(o)만든다. */

BEGIN(Engine)

//class CTransform;

class ENGINE_DLL CGameObject abstract:  public CBase
{
public:
	typedef struct tagGameObject_Desc : public CTransform::TRANSFORM_DESC
	{

	}GAMEOBJECT_DESC;

protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Get_Component(const wstring& strComTag);
	void Set_Dead() { m_bDead = !m_bDead; } //TODO: 용수 오브젝트 죽이기용
	_bool Get_Dead() { return m_bDead; } //TODO: 용수 오브젝트 죽었는지 확인용

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, _Out_ class CComponent** ppOut, void* pArg = nullptr);

protected:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CTransform* m_pTransform = { nullptr };

	map<const wstring, class CComponent*>	m_Components;

	_bool m_bDead = { false };


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};


END
