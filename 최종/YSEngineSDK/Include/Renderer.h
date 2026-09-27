#pragma once
#include "Base.h"

BEGIN(Engine)

/* 1. 화면에 그려져야할 객체들만 그려지는 순서대로 보관하는 클래스이다.*/
/* 2. 보관하고 있는 순서대로 객체들의 Draw콜(렌더함수를호출한다.)을 수행한다.*/

class CRenderer final: public CBase
{
public:
	enum RENDER_GROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };

private:
	CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDER_GROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Render();
	void Render_Object_List_Clear();
private:

	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();



	HRESULT Set_RenderState_Blend();
	HRESULT Reset_RenderState_Blend();
	void Alpha_Sorting();

private:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };
	list<class CGameObject*>	m_RenderObjectList[RENDER_END]; //렌더 그룹별로 리스트를 관리

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END

