#pragma once

BEGIN(Engine)

typedef struct  {
	HWND hWnd;
	_uint iWinSizeX;
	_uint iWinSizeY;
}ENGINE_DESC;

typedef struct {
	_float3 vPosition;
	_float2 vTexcoord; //텍스쳐 좌표
}VTXPOSTEX;

typedef struct {
	_float3 vPosition;
	_float3 vTexcoord; //텍스쳐 좌표
}VTXCUBE;

END