#pragma once
#include "Base.h"


//역할 오브젝트의 관리.
//원본 객체 생성, 보관
//사본 객체 생성, 보관, 틱
//객체 컨테이너들의 정리, 릴리즈



/* 원형객체들을 보관한다 ..*/
/* 원형객체를 검색 후, 복제하여 사본객체를 생성한다. */
/* 실제 게임내에 사용하고자하는 사본객체들을 내 기준에 따라 그룹(CLayer)지어 보관한다 .*/
/* 사본객체들의 틱함수를 반복적으로 호출해 준다 .*/

BEGIN(Engine)

class CObjectManager final: public CBase
{
private:
	CObjectManager();
	virtual ~CObjectManager() = default;


public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag,class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	HRESULT Add_Object(class CGameObject* pGameObject, _uint iLevelIndex, const wstring& strLayerTag);

public:
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex);

public:
	//지금은 For. Camera Player 
	HRESULT Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _Out_ CGameObject** ppObject, _uint iIndex);
	CGameObject* Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);
	
	list<class CGameObject*>* Get_List(_uint iLevelIndex, const wstring& strLayerTag);

public:
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layers(_uint iLevelIndex,const wstring& strLayerTag);

private:
	_uint m_iLevels = { 0 };

	map<const wstring, class CGameObject*> m_PrototypeMap;
	map<const wstring, class CLayer*>* m_pObjectListMap = { nullptr };
	//map<const wstring, list<class CGameObject*>>* m_pObjectListMap = { nullptr };

public:
	static CObjectManager* Create(_uint iNumLevels); //디바이스?
	virtual void Free() override;
};

END
