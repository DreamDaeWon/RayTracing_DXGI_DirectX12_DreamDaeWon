#pragma once
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Cube_AABB final : public CCollider
{
private:
	CCollider_Cube_AABB(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Cube_AABB(const CCollider_Cube_AABB& rhs);
	virtual ~CCollider_Cube_AABB() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;	
	virtual HRESULT Render() override;
	virtual void Update_Collider_Info(_float4x4 vTargetWorldMatrix) override; // CCollider을(를) 통해 상속됨


	const _float3 Get_Min_Point() { return m_vMinPoint; }
	const _float3 Get_Max_Point() { return m_vMaxPoint; }

private:
	_float3 m_vMinPoint = { 0.f, 0.f, 0.f };
	_float3 m_vMaxPoint = { 0.f, 0.f, 0.f };

public:
	static CCollider_Cube_AABB* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END