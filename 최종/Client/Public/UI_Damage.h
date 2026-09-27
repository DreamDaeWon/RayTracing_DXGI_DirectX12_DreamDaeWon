#pragma once
#include "UI_Base.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

/* 월드 좌표를 받는다 >> 이를 뷰포트 변환해서 뷰포트 상 폰트를 그릴 x, y 좌표를 계산한다.
* 데미지를 받는다 >> 그 데미지 숫자에 맞는 szBuf를 설정해준다.
* 시간을 갖고 있어서 일정 시간 안에 호출되었다면 이전 데미지에 더해서 출력해준다.
* 
* 애드 컴포넌트에서 텍스쳐 10장 받아온다.
* 랜더할때 데미지 단위 판단해서 어떻게 띄울지 판단한다? 두자리수면 두번 랜더하는 방식으로.
* 
*/

class CUI_Damage final : public CUI_Base
{
private:
	CUI_Damage(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI_Damage(const CUI_Base& rhs);
	virtual ~CUI_Damage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Hit_Damage(_float fCP);
	void Set_Life_Time();
	void Update_Position(_float3 vViewPos, _float fViewZ);
	void Set_RealDead();
	//void Set_Damage_Term() { m_fDamageTerm = 0.5f; }

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
	virtual HRESULT Reset_RenderState() override;

private:
	_bool m_bRealDead = { false };
	_float m_fLifeTime = { 0.f };
	_float m_fViewZ = { 0.f };
	_float m_fRatio = { 1.f };
	_uint m_iCP = { 0 }; //출력할 데미지를 저장할 공간
	_uint m_iFrame = { 0 };
	


public:
	static	 CUI_Damage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
