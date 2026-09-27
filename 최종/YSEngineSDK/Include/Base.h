#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;


public:
	//크리에이트? 애드레퍼런스
	unsigned int AddRef();

	//릴리즈? 
	unsigned int Release();

private:
	unsigned int m_iRefCnt = { 0 }; //0초기화 했던가?


public:
	//프리?
	virtual void Free();
};

END

