#pragma once
#include "GameObject.h"
#include "Client_Defines.h"



BEGIN(Client)

class CComet_Manager final : public CGameObject
{
private:
	CComet_Manager(LPDIRECT3DDEVICE9 pGraphic_Device);
	CComet_Manager(const CComet_Manager& rhs);
	virtual ~CComet_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;



};

END
