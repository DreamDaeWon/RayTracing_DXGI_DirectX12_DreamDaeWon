#pragma once
//#include "BaseUI.h"
#include "UI_Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CMonsterWarningUI final: public CUI_Base
{
public:
	typedef struct tagMonsterWarningUI_Desc : public CUI_Base::UI_BASE_DESC {
		_uint iMonsterIndex = 0;
	}MONSTER_WARNING_UI_DESC;

private:
	CMonsterWarningUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonsterWarningUI(const CMonsterWarningUI& rhs);
	virtual ~CMonsterWarningUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	// CUI_Base을(를) 통해 상속됨
	virtual HRESULT Set_RenderState(_ulong lAphaRef) override;
	HRESULT Reset_RenderState();

private:
	void Arlert_Monster();

private:
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };
	class CMonster* m_pMonster = { nullptr };

	_bool m_bOutOfWindow = { false };
	_uint m_iMonsterIndex = { 0 };

public:
	static CMonsterWarningUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

END