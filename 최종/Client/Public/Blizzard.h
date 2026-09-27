#pragma once
#include "LandObject.h"
#include "Client_Defines.h"

#define BLIZZARD_START_Y 20.f
#define BLIZZARD_END_Y -10.f

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

//속도 무조건. 위치(xz). 크기
class CBlizzard final: public CLandObject
{
public:
	typedef struct tagBlizzardDesc : public CLandObject::LANDOBJECT_DESC {
		_float fScale = { 1.f };
	}BLIZZARD_DESC;
private:
	CBlizzard(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBlizzard(const CBlizzard& rhs);
	virtual ~CBlizzard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Set_RenderState();
	HRESULT Reset_RenderState();
	void SetUp_BillBoard();

	void Move(_float fTimeDelta);
	void Restart();//끝지점 오면 다시 시작 지점 가서 오게 하기.

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer_Com = { nullptr };

	CTransform* m_pCameraTransform = { nullptr };

public:
	static CBlizzard* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
