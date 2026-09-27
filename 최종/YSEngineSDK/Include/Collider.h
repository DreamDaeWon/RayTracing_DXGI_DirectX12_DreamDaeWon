#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider abstract: public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

protected:
	CCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

public:
	_float3 Get_State(STATE eState) const
	{
		return *(_float3*)&m_WorldMatrix.m[eState][0];
	}

	_float3 Get_Scale() {

		_float3 vScale = {};
		_float3 vRight = { Get_State(STATE_RIGHT) };
		_float3 vUp = { Get_State(STATE_UP) };
		_float3 vLook = { Get_State(STATE_LOOK) };

		vScale.x = D3DXVec3Length(&vRight);
		vScale.y = D3DXVec3Length(&vUp);
		vScale.z = D3DXVec3Length(&vLook);

		return  vScale;
	}

	void Set_State(STATE eState, const _float3& vState)
	{
		memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof(_float3));
	}

	void Set_Scale(_float3 vScale)
	{
		_float3 vRight = Get_State(STATE_RIGHT);
		_float3 vUp = Get_State(STATE_UP);
		_float3 vLook = Get_State(STATE_LOOK);
		vRight = *D3DXVec3Normalize(&vRight, &vRight) * vScale.x;
		vUp = *D3DXVec3Normalize(&vUp, &vUp) * vScale.y;
		vLook = *D3DXVec3Normalize(&vLook, &vLook) * vScale.z;
		Set_State(STATE_RIGHT, vRight);
		Set_State(STATE_UP, vUp);
		Set_State(STATE_LOOK, vLook);
	}

	const _float4x4* Get_WorldMatrix() const {
		return &m_WorldMatrix;
	}

public:
	virtual void Update_Collider_Info(_float4x4 vTargetWorldMatrix) = 0; //TODO:매틱 위치 등의 정보를 업데이트 해주는 함수

	void Turn(const _float3& vAxis, _float fTimeDelta);

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();


protected:
	//class CGameObject* m_pTargetObject = { nullptr }; //포지션 받을때 필요한가 싶다?
	_float4x4		m_WorldMatrix = {};//콜라이더의 월드 행렬 


protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = { nullptr };
	_uint						m_iVertexStride = { 0 }; /* 정점하나의 크기(Byte) */
	_uint						m_iNumVertices = { 0 };  /* 정점의 갯수*/
	_ulong						m_dwFVF = { 0 };		/* 정점이 담고 있는 벡터의 종류를 나타냄 */
	_ulong						m_dwPrimitive = { 0 };		/* 정점을 찍을 방식 */

	LPDIRECT3DINDEXBUFFER9		m_pIB = { nullptr };
	_uint						m_iIndexStride = { 0 }; /* 인덱스 하나의 크기(Byte) */
	_uint						m_iNumIndices = { 0 };  /* 인덱스의 갯수*/
	D3DFORMAT					m_eIndexFormat = {};




public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
