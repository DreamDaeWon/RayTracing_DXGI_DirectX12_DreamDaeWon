#pragma once
#include "Collider.h"

#define CCOLLIDER_SPHERE_SLICE 12 
#define CCOLLIDER_SPHERE_STACK 9
#define CCOLLIDER_SPHERE_RADIUS 1

BEGIN(Engine)

class ENGINE_DLL CCollider_Sphere final : public CCollider
{
private:
	CCollider_Sphere(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Sphere(const CCollider_Sphere& rhs);
	virtual ~CCollider_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	virtual void Update_Collider_Info(_float4x4 vTargetWorldMatrix) override; // CCollider을(를) 통해 상속됨

	void Set_Range(_float fRange) { m_fRange = fRange; }
	_float Get_Range() { return m_fRange; }

private:
	_float m_fRange = { 1.f };

public:
	static CCollider_Sphere* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END