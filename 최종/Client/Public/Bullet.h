#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#define BULLET_LIFE_TIME 1.f
#define TRAILMAXNUM 10
#define TRAILTERM 0.05f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBullet final : public CGameObject
{
//public:
//	typedef struct tagBulletDesc : public CGameObject
//	{
//		_float fCP = { 0.f };
//	}BULLET_DESC;

private:
	CBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

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
		m_vPrePosition = vRayPos;
		m_pTransform->Set_State(CTransform::STATE_POSITION, vRayPos);
		m_pTransform->LookAt(vRayPos + vRayDir);
	}

	void Set_Frame(_float fFrame) { m_fFrame = fFrame; }

	_bool Get_Chase() { return m_bChase; }
	void Set_Chase(_bool bChase) { m_bChase = bChase; }



private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();

	void Set_Trail();

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	vector<class CTrailRenderer*> m_vecTrailRenderer = {}; //트레일 벡터 컨테이너
	_uint m_iTrailCursor = { 0 }; //랜덤엑세스를 위한 트레일 벡터의 커서
	_float m_fTrailTerm = { 0.f }; //트레일 마다의 사이 시간.

	//_float m_fCP = { 0.f };//총알의 공격력
	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	_float m_fFrame = { 0.f };
	_float3 m_vPrePosition = { 0.f,0.f,0.f };

	_bool m_bChase = { false };// 추적변수
	
public:
	static CBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END