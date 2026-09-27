#pragma once
#include "LandObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
//콜라이더 실험 BEGIN.
class CCollider_Sphere;
//콜라이더 실험 END.
END

BEGIN(Client)

class CEarthQuake final : public CLandObject
{
public:
	typedef struct tagEarthQuake_Desc : public CLandObject::LANDOBJECT_DESC {
		_float3 m_vPos; // 위치
		_float m_fScale; // 크기
		_float m_fLifeTime; // 몇 초 뒤에 사라질 건지?

	}EARTH_QUAKE_DESC;

private:
	CEarthQuake(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEarthQuake(const CEarthQuake& rhs);
	virtual ~CEarthQuake() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Return_Look_Position(_float3* pvLook, _float3* pvPosition); // 현재 보는 곳과 위치를 알려주는 함수
	_float3 Return_ViewPort_Pos(); // 뷰 포트 상의 현재 위치를 알려주는 함수

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard(); // 빌보드 함수 카메라 보도록 고정
	void Look_Camera_Fixed_Y_Axis(); // 카메라를 봐도 010벡터와 모두 외적하게 Look와 Right벡터를 다시 만들어주는 함수

	void Collision_Player();

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

	//콜라이더 실험 BEGIN.
	CCollider_Sphere* m_pCollider_Com = { nullptr };
	//콜라이더 실험 END.

	// 카메라 트랜스폼
	CTransform* m_pCameraTranformCom = { nullptr };

	// 받아오는 몬스터의 룩벡터
	_float3 m_vBossPos = {};
	_float m_fScale = {};

	// 라이프타임
	_float m_fLifeTime = { 0.f };

	// 어떤 색의 공 출력할건지?
	_uint m_iTexNum = { 0 };


public:
	static CEarthQuake* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override; // 이거 보스가 실행
	virtual void Free() override;
};

END