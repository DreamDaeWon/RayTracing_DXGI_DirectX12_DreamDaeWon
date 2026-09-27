#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TYPE { TYPE_TEX2D, TYPE_TEXCUBE, TYPE_END };

private:
	CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const wstring& strFullPath, _uint iNumTextures);
	//	virtual HRESULT Initialize_Prototype(TYPE eType, const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Texture(_ulong dwStage, _uint iTextureIndex);

private:
	_uint							m_iNumTextures = { 0 };
	vector<LPDIRECT3DBASETEXTURE9>	m_Textures;
	//LPDIRECT3DCUBETEXTURE9
	//LPDIRECT3DTEXTURE9

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eType, const wstring& strFullPath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

