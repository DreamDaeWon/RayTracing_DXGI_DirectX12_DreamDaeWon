#pragma once
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CCollider_Rect final : public CCollider
{
public:
	typedef struct tagColliderRect {
		_float3 LeftTop = { 0.f,0.f,0.f };
		_float3 RightTop = { 0.f,0.f,0.f };
		_float3 LeftBottom = { 0.f,0.f,0.f };
		_float3 RightBottom = { 0.f,0.f,0.f };
	}COL_RECT;


private:
	CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider_Rect(const CCollider_Rect& rhs);
	virtual ~CCollider_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;	
	virtual HRESULT Render() override;
	virtual void Update_Collider_Info(_float4x4 vTargetWorldMatrix) override; // CCollider을(를) 통해 상속됨

	const COL_RECT* Get_Points() { return &m_tRect; }

private:
	COL_RECT m_tRect = {}; //사각형을 구성하는 네 점

public:
	static CCollider_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

END