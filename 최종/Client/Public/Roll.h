#pragma once
#include "Player_Skill.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Sphere;
END
BEGIN(Client)

class CRoll final: public CPlayer_Skill
{
public:
	typedef struct tagRoll_Desc : public CPlayer_Skill::GAMEOBJECT_DESC {
		
	}ROLL_DESC;


private:
	CRoll(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRoll(const CRoll& rhs);
	virtual ~CRoll() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Key_Input(_float fTimeDelta);

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState() override;
	virtual HRESULT Reset_RenderState() override;
private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Sphere* m_pVIBuffer_Com = { nullptr };

public:
	static CRoll* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END