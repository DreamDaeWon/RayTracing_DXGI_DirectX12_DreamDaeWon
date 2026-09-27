#pragma once

#include "Base.h"

BEGIN(Engine)

class CKey_Manager final : public CBase
{
private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	HRESULT Initialize();

public:
	_bool		Key_Pressing(_uint _iKey);
	_bool		Key_Down(_uint _iKey);
	_bool		Key_Up(_uint _iKey);

private:
	_bool			m_bKeyState[VK_MAX];

public:
	static	CKey_Manager* Create();
	virtual void Free() override;
};

END
