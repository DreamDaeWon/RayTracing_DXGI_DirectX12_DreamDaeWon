#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Update();
	void Transform_PickingToLocalSpace(class CTransform* pTransform,_Out_ _float3* pRayDir, _Out_ _float3* pRayPos);
	void Transform_PickingToLocalSpace(_float4x4 WorldMaxrixInv,_Out_ _float3* pRayDir, _Out_ _float3* pRayPos);

	//TODO: 용수 월드 공간의 마우스 레이를 리턴함. 콜리전용
	void Get_World_Mouse_Ray(_Out_ _float3* pRayDir, _Out_ _float3* pRayPos) 
	{
		*pRayDir = m_vRayDir;
		*pRayPos = m_vRayPos;
	}

private:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };
	HWND m_hWnd;
	_float3 m_vRayDir = {}; //월드 공간의 레이
	_float3 m_vRayPos = {};
	_uint m_iWinSizeX = { 0 };
	_uint m_iWinSizeY = { 0 };

public:
	static CPicking* Create(LPDIRECT3DDEVICE9 pGraphic_Device, HWND hWnd,_uint iWinSizeX,_uint iWinSizeY);
	virtual void Free() override;
};

END

