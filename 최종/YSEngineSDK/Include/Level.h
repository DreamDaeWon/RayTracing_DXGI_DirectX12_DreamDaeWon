#pragma once
#include "Base.h"

BEGIN(Engine)

/* 클랑이언트개발자가 만드는 모든 레벨 클래스들의 부모가 되는 클래스다. */
class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize() { return S_OK; }
	virtual void	Tick(_float fTimeDelta) {}
	virtual HRESULT Render() { return S_OK; }

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };

public:
	virtual void Free() override;

};

END