#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC {
		_float3 vEye = {};
		_float3 vAt = {};
		_float fFovy = { 0.f };
		_float fAspect = { 0.f };
		_float fNear = { 0.f };
		_float fFar = { 0.f };
	}CAMERA_DESC;

protected:
	CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual _uint Tick(_float fTimedelta)override;
	virtual void Late_Tick(_float fTimedelta)override;

protected:
	HRESULT Bind_PipeLines();

protected:
	_float m_fFovy = { 0.f };
	_float m_fAspect = { 0.f };
	_float m_fNear = { 0.f };
	_float m_fFar = { 0.f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
