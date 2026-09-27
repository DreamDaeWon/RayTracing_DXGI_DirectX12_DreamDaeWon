#pragma once

#include "GameObject.h"

/* 알파블렌드가 필요한 객체들의 부모클래스이다. */
/* 알파소팅이 필요하기 때문에 정렬에 필요한 데이터를 제공하고 셋팅한다. */

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
protected:
	CBlendObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBlendObject(const CBlendObject& rhs);
	virtual ~CBlendObject() = default;

public:
	_float Get_ViewZ()  {
		Compute_ViewZ();
		return m_fViewZ;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float				m_fViewZ = { 0.f };

protected:
	HRESULT Compute_ViewZ();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END