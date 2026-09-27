#pragma once
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_AirPlane.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Tile.h"
#include "VIBuffer_Wall.h"
#include "Collider_Rect.h"
#include "Collider_Sphere.h"
#include "Collider_Cube_AABB.h"
#include "Transform.h"
#include "Texture.h"

/* 1. 컴포넌트들의 원형을 레벨별로 보관한다. */
/* 2. 지정한 원형을 복제하여 사본객체를 생성하고 리턴한다 .*/

BEGIN(Engine)

class  CComponent_Manager final: public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

private:
	_uint m_iNumLevels = { 0 };
	map<const wstring, CComponent*>* m_pPrototypes = { nullptr };
	typedef map<const wstring, CComponent*>	PROTOTYPES;
	
public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END