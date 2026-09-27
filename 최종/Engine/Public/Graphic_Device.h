#pragma once
#include "Base.h"


BEGIN(Engine)


class CGraphic_Device final: public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT		Init_Device(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, LPDIRECT3DDEVICE9* ppGraphic_Device);

	void		Render_Begin();
	void		Render_End(HWND hWnd = nullptr);

private:
	void		Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);

	// COM 객체 : 개발 환경에 상관 없이 호환이 가능한 컴포넌트 사용을 위해 ms가 규정한 표준화된 방법
	//			 윈도우라는 특정 시스템에 대하여 동일하게 작동하도록 COM 객체를 통해 통신 규약을 삽입함
private:
	LPDIRECT3D9				m_pSDK;
	LPDIRECT3DDEVICE9		m_pDevice;

	// dx 상에서 2D 이미지를 출력하는 객체
	LPD3DXSPRITE			m_pSprite;
	// dx 상에서 폰트 출력을 담당하는 객체
	LPD3DXFONT				m_pFont;

public:
	static CGraphic_Device* Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, _Out_ LPDIRECT3DDEVICE9* ppGraphic_Device);
	virtual void Free() override;
};
END