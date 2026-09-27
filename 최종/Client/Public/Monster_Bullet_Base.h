#pragma once
#include "LandObject.h"

#define BULLET_LIFE_TIME 1.f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider_Sphere;
END

BEGIN(Client)

class CMonster_Bullet_Base abstract : public CLandObject
{
public:
	typedef struct tagMonsterBulletBaseDesc : public CLandObject::LANDOBJECT_DESC {
		_float3 m_vPos; // 위치
		_float m_fScale; // 크기
		_float3 m_vLook; // 룩벡터
		_float m_fLifeTime; // 몇 초 뒤에 사라질 건지?
		_float m_fSpeed; // 속도

	}MONSTER_BULLET_BASE_DESC;

protected:
	CMonster_Bullet_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster_Bullet_Base(const CMonster_Bullet_Base& rhs);
	virtual ~CMonster_Bullet_Base() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	//충돌처리시 필요한 데이터를 반환하는 함수
	void Get_RayPos_RayDir_Length(_float fTimeDelta, _Out_ _float3* vRayPos, _Out_ _float3* vRayDir, _Out_ _float* fLength)
	{
		*vRayDir = m_pTransform->Get_State(CTransform::STATE_LOOK);
		*fLength = m_pTransform->Compute_Distance_Delta(fTimeDelta);
		*vRayPos = m_pTransform->Get_State(CTransform::STATE_POSITION) - *vRayDir * *fLength;
	} 
	
	void Set_Life_Time() { 
		m_fLifeTime = BULLET_LIFE_TIME; 
		m_bDead = false; 
	}

	void Set_Pos_Dir(_float3 vRayPos, _float3 vRayDir)
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, vRayPos);
		m_pTransform->LookAt(vRayPos + vRayDir);
	}

public:
	void Return_Look_Position(_float3* pvLook, _float3* pvPosition); // 현재 보는 곳과 위치를 알려주는 함수
	_float3 Return_ViewPort_Pos(); // 뷰 포트 상의 현재 위치를 알려주는 함수
	void SetUp_BillBoard(); // 빌보드 함수 카메라 보도록 고정

protected:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Set_RenderState();
	virtual HRESULT Reset_RenderState();

protected:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	CCollider_Sphere* m_pCollider_Com = { nullptr };
	CTransform* m_pCameraTransform = { nullptr };

	// 속도
	_float m_fSpeed = { 1.f };

	// 받아오는 몬스터의 룩벡터
	_float3 m_vBossPos = {};

	// 크기 값
	_float m_fScale = { 0.5f };

	// 라이프타임. 0이상일 때만 출력. 틱마다 감소.
	_float m_fLifeTime = { 20.f };

	// 어떤 색의 공 출력할건지?
	_uint m_iTexNum = { 0 };

	_float3 m_vLook = { 0.f, 1.f, 0.f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END