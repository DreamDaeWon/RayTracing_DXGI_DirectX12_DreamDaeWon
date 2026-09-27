#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"
#include "UI_Damage.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain;
END


/* 지형을 타고 움직이는 개체들의 부모클래스 */
/* 지형의 높이를 구해서 태우는 기능. */


BEGIN(Client)

class CLandObject abstract : public CBlendObject
{
public:
	typedef struct tagLandObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		CTransform* pTerrainTranformCom = { nullptr };
		CVIBuffer_Terrain* pTerrainVIBufferCom = { nullptr };
		_float3		vPos = { 0.f,0.f,0.f };
		_float fTerm = { 3.f };

		_uint		iMonsterType = { 0 };			// 준수 추가
		_float		fdelayTime = { 0.f };
	}LANDOBJECT_DESC;

protected:
	CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLandObject(const CLandObject& rhs);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT SetUp_OnTerrain(_float fRevision = 0.f);
	_float3 Return_ViewPort_Pos();
	void Chase_Player(_float fTimeDelta, _float Min_Distance);
	_float  Set_Texture_Monster_Dir(_float3 vGameLook);
	_float  Set_Texture_Player_Dir(_float3 vGameLook);

	void Set_Terrain(LANDOBJECT_DESC* pLandDesc);

	void Set_Pos(_float3 vPosition) {
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
	}
	

protected:
	CTransform* m_pTerrainTranformCom = { nullptr };
	CVIBuffer_Terrain* m_pTerrainVIBufferCom = { nullptr };
	
	// 용수 추가. 데미지 폰트 . 얘가 몬스터의 부모
	CUI_Damage* m_pUI_Damage = { nullptr };

	// 준수 추가
	_uint m_iMonsterType = { 0 };

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END
