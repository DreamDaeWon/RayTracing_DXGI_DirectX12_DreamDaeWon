#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC {
		_float fMouseSensor = { 1.f };
	}CAMERA_FREE_DESC;

private:
	CCamera_Free(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	void Mouse_Axis_Turn(_float fTimeDelta); 

private:
	_float m_fMouseSensor = { 1.f };
	POINT m_ptMouse = {};

public:
	static CCamera_Free* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END