#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Wall final : public CVIBuffer
{
private:
	CVIBuffer_Wall(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Wall(const CVIBuffer_Wall& rhs);
	virtual ~CVIBuffer_Wall() = default;


public:
	virtual HRESULT Initialize_Prototype(_uint iVertexNumX, _uint iVertexNumZ);
	virtual HRESULT Initialize(void* pArg) override;

	void Get_X_Z_Num(_uint* iXNum, _uint* iZNum);

private:
	_uint m_iVertexNumX = { 0 };
	_uint m_iVertexNumY = { 0 };

public:
	static CVIBuffer_Wall* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iVertexNumX, _uint iVertexNumZ);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END