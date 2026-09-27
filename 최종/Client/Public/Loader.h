#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

/* 다음 레벨의 자원들 로딩 */
/* 다음 레벨에서 사용할 원형 객체를 생성. */

class CLoader final : public CBase
{
private:
	CLoader(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Start();
	_bool isFinished() const { return m_isFinished; }

	void Output() {
		SetWindowText(g_hWnd, m_strLoadingText.c_str());
	}

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();
	HRESULT Loading_For_Normal1();
	HRESULT Loading_For_1945();
	HRESULT Loading_For_Normal2();
	HRESULT Loading_For_SnowBoss();
	
	//void Load_Terrain();

private:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };
	HANDLE m_hThread;						// 쓰레드 핸들
	CRITICAL_SECTION m_Critical_Section; 	// 크리티컬 섹션?
	LEVEL m_eNextLevelID = { LEVEL_END };	// 다음 레벨 아이디
	_bool m_isFinished = { false };			// 로딩이 완료되었는지
	wstring m_strLoadingText; 				// wstring : 로딩중 띄울 텍스트

	CGameInstance* m_pGameInstance = { nullptr };


public:
	//static CLoader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID);
	static CLoader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevelID);
	virtual void Free() override;
	//virtual void Free() override;
};

END