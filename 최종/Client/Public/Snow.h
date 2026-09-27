
#pragma once
#include "LandObject.h"
#include "Client_Defines.h"

#define SNOW_LIFE_TIME 3.f

BEGIN(Engine)
class CVIBuffer_Sphere;
class CTexture;
//콜라이더 실험 BEGIN.
class CCollider_Sphere;
//콜라이더 실험 END.
END

BEGIN(Client)

class CSnow final : public CLandObject
{
public:
	typedef struct tagSnow_Desc : public CLandObject::LANDOBJECT_DESC {

		_float3 m_vLook; // 바라보는 방향
		_float3 m_vPos; // 위치
		_float m_fScale; // 크기
		_uint m_iTexNum; // 어떤 사진을 사용할지?
		_float m_fSpeed; // 속도

	}SNOW_DESC;

private:
	CSnow(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSnow(const CSnow& rhs);
	virtual ~CSnow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Return_Look_Position(_float3* pvLook, _float3* pvPosition); // 현재 보는 곳과 위치를 알려주는 함수
	_float3 Return_ViewPort_Pos(); // 뷰 포트 상의 현재 위치를 알려주는 함수

	void Set_Life_Time() {
		m_fLifeTime = SNOW_LIFE_TIME;
		m_bDead = false;
		m_pUI_Damage->Set_RealDead();
	}

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard(); // 빌보드 함수 카메라 보도록 고정
	void Look_Camera_Fixed_Y_Axis(); // 카메라를 봐도 010벡터와 모두 외적하게 Look와 Right벡터를 다시 만들어주는 함수

	void Collision_Player(); //TODO : 용수 - 안 맞았는데 맞음. 버그 있음.
	void Collision_Box();//용수 프레임 너무 떨어짐. 충돌도 잘 안되는 것 같음.

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Sphere* m_pVIBuffer_Com = { nullptr };

	//콜라이더 실험 BEGIN.
	CCollider_Sphere* m_pCollider_Com = { nullptr };
	//콜라이더 실험 END.

	// 속도
	_float m_fGoSpeed = { 10.f }; 

	// 받아오는 몬스터의 룩벡터
	_float3 m_vBossLook = {};
	_float3 m_vBossPos = {};
	_float m_fScale = {};

	// 라이프타임
	_float m_fLifeTime = { 0.f };
	
	// 어떤 색의 공 출력할건지?
	_uint m_iTexNum = { 0 };


public:
	static CSnow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END