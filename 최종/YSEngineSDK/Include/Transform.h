#pragma once
#include "Component.h"

BEGIN(Engine)

/* 1.객체의 월드공간에서의 상태를 표현한다(월드변환행렬) */
/* 2. 월드공간에서의 움직임에 대한 처리 기능 */

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct tagTransformDesc{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;

private:
	CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	//CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

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

	const _float4x4* Get_WorldMatrix_Inverse() const {
		_float4x4 InverseMatrix;
		return D3DXMatrixInverse(&InverseMatrix, nullptr, &m_WorldMatrix);
	}

	const _float4x4* Get_WorldMatrix() const {
		return &m_WorldMatrix;
	}

	void Set_WorldMatrix(_float4x4 _WorldMatrix) {
		memcpy(&m_WorldMatrix, &_WorldMatrix, sizeof(_float4x4));
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

public:
	HRESULT Bind_WorldMatrix();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);


	void Go_SR(_float fTimeDelta, _Out_ _float3* vGoalDir = nullptr);
	void Go_SL(_float fTimeDelta, _Out_ _float3* vGoalDir = nullptr);
	void Go_BR(_float fTimeDelta, _Out_ _float3* vGoalDir = nullptr);
	void Go_BL(_float fTimeDelta, _Out_ _float3* vGoalDir = nullptr);
	void Go_To_Dir(_float fTimeDelta, _float3 vDir);

	void Turn(const _float3& vAxis, _float fTimeDelta);
	void LookAt(const _float3& vAt);
	void LookAt_LandObject(const _float3& vAt);
	void Move_To_Target(const _float3& vTargetPoint, _float fTimeDelta ,_float fMinDistance = 0.f);
	void Move_To_Target(const CTransform* pTargetTransform, _float fTimeDelta ,_float fMinDistance = 0.f);

	_float Compute_Distance_Delta(_float fTimeDelta); //TODO: 용수 틱당 이동 거리 계산용

private:
	_float4x4		m_WorldMatrix;
	_float			m_fSpeedPerSec = { 0.f };
	_float			m_fRotationPerSec = { 0.f };

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	
};

END