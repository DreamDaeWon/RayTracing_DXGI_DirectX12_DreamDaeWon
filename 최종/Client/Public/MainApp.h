#pragma once

#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();


private:
	HRESULT Ready_Default_Setting();
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_Component_For_Static();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };


public:
	/* 정적인 멤버함수 : */
	/* 1. 객체화되지 않아도 호출 할 수 있는 멤버함수. */
	/* 2. 함수안에서 this포인터가 유효하지 않다. */
	static CMainApp* Create();
	virtual void Free();	

};
END
