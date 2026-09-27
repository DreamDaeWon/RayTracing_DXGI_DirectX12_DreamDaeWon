#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sphere final : public CVIBuffer
{
private:
	CVIBuffer_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device, _uint _Sphere_Slices, _uint _Sphere_Stacks);
	CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs);
	virtual ~CVIBuffer_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	_float		m_Sphere_Radius = { 0 }; // 구의 반지름
	_uint		m_Sphere_Slices = { 8 }; // 구를 몇 개로 자를 것인가?
	_uint		m_Sphere_Stacks = { 0 }; // m_Sphere_Slices에서 정한 선과 선 사이를 몇 개로 나눌 것인지?
	_float		m_Angle = { 0.f };		// Slices로 잘랐을 때 하나의 각도는 얼마인가?
	_float		m_One_Radius = { 0.f }; // 지름을 Stacks로 잘랐으 때 하나의 z좌표는 얼마인가?


public:
	// Create를 할 때 반지름과 세로 가로로 얼마나 자를건지 넣어준다.
	static CVIBuffer_Sphere* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint _Sphere_Slices, _uint _Sphere_Stacks);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END