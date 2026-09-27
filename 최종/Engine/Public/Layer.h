#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final: public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	HRESULT Add_GameObject_In_Layer(class CGameObject* pGameObject);
	HRESULT Get_Object(class CGameObject** ppGameObject, _uint iIndex);
	class CGameObject* Get_Object(_uint iIndex);
	class CComponent* Get_Component(const wstring& strComTag, _uint iIndex);

	list<class CGameObject*>* Get_List() { return &m_GameObjectList; }
private:
	list<class CGameObject*> m_GameObjectList;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END