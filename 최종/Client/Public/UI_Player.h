#pragma once

#include"UI_Base.h"

//TODO: 예은 - UI만지작
/*
24.01.12 예은 작업
뒷배경, 하트 UI에 대해 한번에 출력되도록 작업
transform 컴포넌트 복제 불가 문제
-> 해결방안 : transform을 하나로 두되, 그것을 중점으로 여러 객체를 Render할 때만 조절하자

24.01.13
데미지 입을시 부드러운 프레임 전환
체력 회복_ 같은 프레임


*/

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CUI_Player final : public CUI_Base
{
public:
	typedef struct tagUI_Player_Desc : public CUI_Base::UI_BASE_DESC
	{
		class CState* pPlayerStateCom = {nullptr};
	}UI_PLAYER_DESC;
private:
	enum UI{UI_HEARTBACK, UI_HEART, UI_GATTOBACK, UI_GATTOFACE, UI_END};

private:
	CUI_Player(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Player(const CUI_Base& rhs);
	virtual ~CUI_Player() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

//TODO: 예은 추가
private:
	
	_bool							m_bGoneHeart1 = false;
	_bool							m_bGoneHeart2 = false;
	_bool							m_bGoneHeart3 = false;
	_bool							m_bGattoHeart = false;

	_bool							m_bEnd = false;

	_float						m_fHeartFrame = { 12.f };
	_float						m_fHeartDeadFrame = { 0.f };
	_float						m_fGattoFaceFrame = { 0.f };
	_float						m_fHeartSizeX, m_fHeartSizeY;

	CTexture*					m_pTextureHeartCom = { nullptr };
	CTexture*					m_pTextureGattoBackCom = { nullptr };
	CTexture*					m_pTextureGattoFaceCom = { nullptr };
	class CState*				m_pPlayerStateCom = {nullptr};

private:
	HRESULT							Render_Again(_float fX, _float fY, _uint iFrame, _uint iRenderState, UI e_UIId);


private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;
public:
	static	 CUI_Player* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END