#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final: public CVIBuffer
{
private:
	CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;


public:
	virtual HRESULT Initialize_Prototype(_uint iVertexNumX, _uint iVertexNumZ);
	virtual HRESULT Initialize_Prototype(const wstring& strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	//내가 만든것.
	D3DXPLANE Find_Poligon_XZ_Plane(_float3 vPosition); //사용하지 말것. 락언락
	_float Landing_Object(_float3 vPosition); //TODO:용수 수정해라

	_float Compute_Height(const _float3& vTargetPosition);
	//인덱스 버퍼 생성 두개의 함수로 구현하는거 만들기.




private:
	_uint m_iVertexNumX = { 0 };
	_uint m_iVertexNumZ = { 0 };

	//_float3* m_pVerticesPos = { nullptr }; //TODO : 이거 부모에 있어야 될거같다.


public:
	static CVIBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iVertexNumX, _uint iVertexNumZ);
	static CVIBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const wstring& strHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END