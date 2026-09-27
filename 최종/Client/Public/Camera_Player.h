#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Player final : public CCamera
{
public:
	enum STATE{ STATE_QUARTER, STATE_TPS, STATE_ZOOM, STATE_END};
	typedef struct tagCameraPlayerDesc : public CAMERA_DESC {
		class CPlayer* pPlayer = { nullptr };
		_float fQuarterViewDistance = { 0.f };
		_float fTPSViewDistance = { 0.f };
	}CAMERA_PLYAER_DESC;

private:
	CCamera_Player(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:

	void Key_Input();
	void Chase_Player();
	void Mouse_Axis_Turn(_float fTimeDelta);


private:
	//플레이어 포인터 
	class CPlayer* m_pPlayer = { nullptr };
	_float m_fQuarterViewDistance = { 0.f };
	_float m_fTPSViewDistance = { 0.f };
	STATE m_eState = { STATE_END };

	POINT			m_ptMouse = {};
	/*생성에 대한 고찰
	* 객체원형을 생성 >> 플레이어 생성하고 카메라 플레이어 생성. >> 이러면 카메라플레이어에 플레이어를 갖게 하기가 컨테이너에서 꺼내서 넣어줘야하는데
	* 꽤나 불편할지도.
	* 또는 플레이어가 만들어질때 카메라도 같이 생성? 만약 그렇다면 그냥 플레이어가 카메라를 멤버로 갖는다? 이러면 카메라플레이어에 플레이어 주소 넣어주는건 
	* 쉽다. 어차피 카메라가 없는 플레이어라는게 말이 안된다는 관점으로 접근하면 이렇게 하는게 맞을지도.
	* 플레이어의 사본이 생성될때 카메라(원형? 사본(pArg로 데이터 넘겨줌)?)가 같이 생성. 
	*/

public:
	static CCamera_Player* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END