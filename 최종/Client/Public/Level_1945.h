#pragma once
#include "Level.h"
#include "Client_Defines.h"


BEGIN(Client)

class CLevel_1945 final : public CLevel
{
private:
	CLevel_1945(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_1945() = default;


public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround_Parsing();
	HRESULT Ready_Layer_UI_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Bullet(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Ready_Land_Object();
	HRESULT Ready_Layer_Camera_Player(const wstring& strPlayerLayerTag, const wstring& strCameraLayerTag);
	HRESULT Ready_Layer_QAim(const wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);
	HRESULT Ready_Layer_Comet(const wstring& strLayerTag);
	HRESULT Ready_Layer_Trigger();
	HRESULT Ready_Layer_Trigger_BlackHole(const wstring& strLayerTag);
	
	// 운석생성을 위한 함수
	void SponMoonStone(_float fTimeDelta); // 총괄
	void SponMoonStoneLine(); // 운석을 일자로 생성(1칸 비도록)
	void SponMoonStoneRandom(); // 운석의 크기와 체력 속도를 랜덤으로 생성
	void SponMoonStoneDiagonal(); // 운석을 대각선으로 설정 오른쪽 또는 왼쪽으로(1칸 비도록)


	// 몬스터 단계를 위한 함수
	void SponShootMonster(_float fTimeDelta); // 총괄
	void SponShootMonster_Right(); // 오른쪽
	void SponShootMonster_Left(); // 왼쪽
	void SponShootMonster_Right_Down(); // 오른쪽 아래
	void SponShootMonster_Left_Down(); // 왼쪽 아래
	void SponShootMonster_Down(); // 그냥 아래

	// 보스를 위한 함수
	HRESULT SponBoss(_float fTimeDelta);


	void Coliision_Player_BlackHole();

	void Load_Terrain();
	void Load_Wall();
	//void Load_Object();

private:
	_bool m_bSpawnBoss = { false }; // 보스가 나왔는지?


	_float m_fTime = { 0.f }; // 전체 시간 값
	// MoonStone 관련변수
	_float3 m_vStartMoonStonePos = { 33.f, 0.f, 140.f }; // 얼마만큼의 범위에서 돌아다닐 건지?
	_float m_fMoonStoneTime = {0.f}; // 운석을 생성하기 위한 시간변수

	_float m_fMoonStoneSponTime = { 2.f }; // 몇 초에 1번씩 운석을 생성할건지?

	_float m_fMoonStonePatternTime = {10.f}; // 운석 패턴을 고르기 위한 시간 값
	_float m_fMoonStonePatternTime_Plus = { 0.f }; // 운석 패턴 시간이 다 되었는지?

	enum MOONSTONEPATTERN { PATTERN_LINE, PATTERN_RANDOM, PATTERN_DIAGONAL, PATTERN_END };
	MOONSTONEPATTERN m_eMoonStonePattern = { PATTERN_LINE }; // 시작 패턴
	_int m_iEmpty = {}; // 비어야 하는 공간의 번호를 넣어주는 곳.
	//=======================================================
	
	// ShootMonster관련변수
	_float m_fShootMonsterTime = { 0.f }; // 슛 몬스터를 생성하기 위한 시간변수

	_float m_fShootMonsterSponTime = { 8.f }; // 몇 초에 1번씩 운석을 생성할건지?

	_float m_fShootMonsterPatternTime = { 10.f }; // ShootMonster 패턴을 고르기 위한 시간 값
	_float m_fShootMonsterPatternTime_Plus = { 0.f }; // ShootMonster 패턴 시간이 다 되었는지?



	// 시간제어==================================================================================

	_float m_fMonsterStageTime = { 30.f }; // 얼마의 시간이 지나면 몬스터를 생성하는 스테이지를 연출할건지?
	_float m_fBossSponTime = { 70.f }; // 얼마의 시간이 지나면 보스를 생성할건지?
	_float m_fAllAttakTime = { 100.f }; // 총공격 시간이 얼마인지?

	_float3 m_vShootMonsterStartPos = { 60.f,0.f,50.f }; // 얼마만큼의 범위에서 하는지?
	enum SHOOTMONSTERPATTERN { SHOOTMONSTER_LEFT, SHOOTMONSTER_RIGHT, SHOOTMONSTER_DOWN, SHOOTMONSTER_LEFT_DOWN, SHOOTMONSTER_RIGHT_DOWN, SHOOTMONSTER_END };

	SHOOTMONSTERPATTERN m_eShootMonsterPattern = { SHOOTMONSTER_DOWN }; // 시작 패턴
	
	//////////////////////////////


	_uint m_iMonsterNum = { 0 };
public:
	static CLevel_1945* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END