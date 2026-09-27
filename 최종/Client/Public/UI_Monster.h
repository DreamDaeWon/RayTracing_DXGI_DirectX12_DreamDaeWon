#pragma once

#include"UI_Base.h"

BEGIN(Client)

class CUI_Monster final : public CUI_Base
{
private:
	CUI_Monster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Monster(const CUI_Base& rhs);
	virtual ~CUI_Monster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;

public:
	static	 CUI_Monster* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END