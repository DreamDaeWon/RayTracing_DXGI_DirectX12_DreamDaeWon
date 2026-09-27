#pragma once
#include "LandObject.h"
#include "Client_Defines.h"

#define COMET_TRAILMAXNUM 1
#define COMET_TRAILTERM 0.15f
#define COMET_START_Z 80.f
#define COMET_END_Z -10.f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

//속도 무조건. 위치(xz). 크기
class CComet final: public CLandObject
{
public:
	typedef struct tagCometDesc : public CLandObject::LANDOBJECT_DESC {
		_float fScale = { 1.f };
	}COMET_DESC;
private:
	CComet(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComet(const CComet& rhs);
	virtual ~CComet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();

	void Set_Trail();

	void Move(_float fTimeDelta);
	void Restart();//끝지점 오면 다시 시작 지점 가서 오게 하기.

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

	class CCamera_Player_DW2* m_pCameraPlayerDW2 = { nullptr }; //카메라 주소.
	CTransform* m_pCameraTransform = { nullptr };

	vector<class CTrailRenderer*> m_vecTrailRenderer = {}; //트레일 벡터 컨테이너
	_uint m_iTrailCursor = { 0 }; //랜덤엑세스를 위한 트레일 벡터의 커서
	_float m_fTrailTerm = { 0.f }; //트레일 마다의 사이 시간.
	_float3 m_vPrePosition = { 0.f,0.f,0.f }; //이전 위치를 저장하는 공간.

	_float m_fLifeTime = { 0.f }; //라이프타임. 0이상일 때만 출력. 틱마다 감소.
	_float m_fFrame = { 0.f };

	//리셋될 위치와 시작 z를 


public:
	static CComet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
