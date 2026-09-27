#pragma once

#include "Camera.h"
#include "Client_Defines.h"
#include <random>

BEGIN(Client)
class CCamera_Player_DW2 final : public CCamera
{
public:
    typedef struct tagCameraPlayer2Desc : public CCamera::CAMERA_DESC
    {
        class CAirPlane* pAirPlane = { nullptr };
        _float      fMouseSensor = { 1.f };
        _uint      iLevel = { 0 };
    }CAMERA_PLAYER_DW_DESC2;

public:
    enum CAMERA_MODE { CAMERA_3VIEW, CAMERA_FREE, CAMERA_1VIEW,  CAMERA_QUTER, CAMERA_PLAYER_RIGHT_VIEW, CAMERA_MONSTER_SCENE, CAMERA_END };

private:
    CCamera_Player_DW2(LPDIRECT3DDEVICE9 pGrahpic_Device);
    CCamera_Player_DW2(const CCamera_Player_DW2& rhs);
    virtual ~CCamera_Player_DW2() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual _uint Tick(_float fTimeDelta) override;
    virtual void Late_Tick(_float fTimeDelta) override;
    _uint Get_Camera_Mode() { 
        if (m_eCamera_Mode == CAMERA_QUTER)
        {
            m_eCamera_Mode = 0;
        }
        return m_eCamera_Mode; }

    void SetCameraShaking(_bool _bShake)
    {
        m_bShake = _bShake;
    }
    void SetCameraMode(CAMERA_MODE eCameraMode)
    {
        m_eCamera_Mode = eCameraMode;
    }
    void SetCameraMonsterScene(_bool _bMonsterScene)
    {
        m_bMonsterScene = _bMonsterScene;
    }
    _bool Get_CanShotBullet() { return m_bCanShotBullet; }

private:
    void Camera_Free_Mode(_float fTimeDelta);
    void Camera_3View_Mode(_float fTimeDelta);
    void Camera_AirPlane_TopView_Mode(_float fTimeDelta);
    void Camera_1View_Mode(_float fTimeDelta);
    void Camera_Quter_Mode(_float fTimeDelta);
    void Camera_Player_Right(_float fTimeDelta);
    void Camera_Shaking(_float fTimeDelta);

    void Camera_Monster_Scene(_float fTimeDelta);

    _float You_Want_Look_TurnAngle(_float3 _TargetLook); // 내가 원하는 방향까지 돌아야 하는 각도를 구해준다.
    _float You_Want_Look_TurnUpAngle(_float3 _TargetLook); // 내가 원하는 방향까지 돌아야 하는 각도를 구해준다.

    _float The_angle_at_which_you_turn_in_the_direction_you_want(_float3 _TargetLook); // 현재의 룩벡터에서 원하는 방향까지의 각도를 구해주는 함수 // 용수형의 관용은 어디까지인가?

    //For. Collision
private:
    void Wall_Collision();

private:
    class CTransform* m_pAirPlaneTransform = { nullptr };
    class CAirPlane*    m_pAirPlane = { nullptr };

private:
    // 몬스터 씬을 위한 변수
    enum MonsterScene {SCENE_RIGHT_VIEW, SCENE_MOVE, SCENE_MONSTER_SPON, SCENE_READY, SCENE_LOOK_AT_MONSTER, SCENE_END}; // 몬스터 씬을 위한 enum타입
    _float m_fMonsterSceneTime = { 0.f }; // 몬스터 장면을 위한 시간(계속 timedelta를 더해줄 값)
    _bool m_bMonsterScene = { false }; // 몬스터 등장 장면을 보여줄건지?
    _float3 m_vMonsterScenePos = {2.f, 0.5f, -4.f}; // 플레이어의 위치에서 얼마나 옆에 카메라를 둘 건지?
    _float3 m_vMoveMonsterScenePos = {}; // 얼마나 이동해야 하는지?
    _float3 m_vShootMonsterSponPos[3] = {}; // 어디에 배치해서 보여줄건지?
    _float m_fReadyTime = { 2.f }; // 대기시간
    MonsterScene m_eMonsterScene = { SCENE_RIGHT_VIEW }; // 몬스터 장면관리
    MonsterScene m_eMonsterNextScene = { SCENE_END }; // 잠시 대기상태 이후에 어떤 상태로 있을건지?
    _float m_fLookMonsterAngle = { 0.f }; // 보스 몬스터를 보기 위해 돌아야 하는 각도
    _float m_fLookMonsterUpAngle = { 0.f }; // 보스 몬스터를 보기 위해 위로 돌아야 하는 각도
    _bool m_bGoPlayerRightView = { false }; // 카메라가 라이트뷰로 갔는지?




    // 화면 전환중인지 아닌지 판별하는 변수
    _bool m_bCanShotBullet = { false }; // 총을 쏠 수 있는 상태인지 아닌지?

    // AirPlane를 위한 멤버변수
    _bool m_bAirPlaneTopViewFirst = { true }; // 탑뷰가 처음인지 아닌지 확인하는 변수
    _float3 m_vAirPlaneTopViewPos = { 25.f, 20.f, 25.f }; // Top 뷰에서의 카메라 위치

    _float m_fAirPlaneTopViewTime = { 1.f }; // 기본값을 1로 줘야 함
    _float m_fAirPlaneTopAngle = { 0.f }; // 얼마나 돌려야 하는지?
    _float m_fAirPlaneTop_UpAngle = { 90.f }; // Up벡터를 얼마나 돌려야 하는지?
    _float3 m_vAirPlaneBeforePos = {}; // 이전 위치값
    _float4x4 m_MatrixBeforeAirPlane = {}; // 회전값을 먹이기 위한 매트릭스
    

    // 서서히 이동하는 시간
    _float m_fMoveTopViewTime = { 0.5f }; // 얼만큼의 시간만큼 Top뷰로 이동할건지?
    _float m_fMove3ViewTime = { 0.5f }; // 얼만큼의 시간만큼 3인칭으로 이동할건지?
    ////////////////

    _float3 m_vAirPlaneNew_Look = {}; // 현재의 Up벡터와 외적하는 Look 벡터
    _float3 m_vAirPlaneNew_Right = {}; // 현재의 Up벡터와 외적하는 Right 벡터




    _float3 m_vMoveAirPlanePos = {}; // 3인칭에서 Top뷰로 서로 움직일 때 얼마나 움질일건지?



    _float3 m_vAirPlane3ViewPos = { 0.f,0.5f,-2.f }; //플레이어보다 얼마나 뒤에 있을건지?
    _float m_fAirPlane3ViewTime = { 0.f };


    //////////////////////////////


    POINT m_ptMouse = { 0,0 };
    _float m_fMouseSensor = 0.1f;
    _uint m_eCamera_Mode = CAMERA_3VIEW; // 현재 카메라 모드
    _float m_fBeforeFov = { 60.f };
    _float m_fCamera_Change_Time = { 0.f }; // 카메라 체인지 시간
    _float m_fZoomFov = { 0.f }; // 줄일 카메라 각도(얼마나 줌 할껀지)
     _uint m_eBeforeCameraMode = CAMERA_3VIEW; // 이전 카메라 모드
    _float m_fCamera_Player_Right_Time = { 0.f }; // 카메라 플레이어 오른쪽으로 가는 체인지 시간
    _bool m_bViewChange = { false };
    _float3 m_vBefore_Distance_Away = { 0.f,0.f,0.f }; // 카메라가 플레이어 오른쪽으로 가기 전 플레이어와 카메라가 떨어진 거리
    _float3 m_vDistance_Away = { 0.f,0.f,0.f }; // 플레이어와 카메라가 떨어진 거리

    _float m_f3ViewUp = { 0.f }; // 3인칭 뷰일 때 얼마나 위로 보낼건지?
    _float m_f3ViewBack = { 0.f }; // 3인칭 뷰일 때 얼마나 뒤로 보낼건지?

    _float m_fRightViewRight = { 0.f }; // Right뷰일 때 얼마나 오른쪽으로 보낼건지?
    _float m_fRightViewUp = { 0.f }; // Right뷰일 때 얼마나 위쪽으로 보낼건지?
    _float m_fRightViewBack = { 0.f }; // Right뷰일 때 얼마나 뒤로 보낼건지?
    _float m_fAngle = { 0.f }; // Right뷰일 때 얼마나 돌려야 하는지?
    _float m_fRightViewChangeTime = { 0.5f }; // 얼만큼의 시간동안 시점 변환을 진행할 건지?
    _float m_fRightViewUpTime = { 0.05f }; // 얼만큼의 시간을 매 틱 마다 더해줄 건지?
    _float3 m_vBeforeLook = {}; // 3인칭에서 딱 눌렀을 때 룩벡터를 저장
    _float3 m_vBeforeRight = {}; // 3인칭에서 딱 눌렀을 때 롸이트벡터를 저장
    _float3 m_vBeforeUp = {}; // 3인칭에서 딱 눌렀을 때 Up벡터를 저장

    // 셰이킹 함수
    _float3 m_vShakeBeforePos = {}; // 이전 위치
    _float m_fShakeSize = {0.05f}; // 셰이킹 강도
    _bool m_bShake = { false }; // 셰이킹 효과 사용중


public:
    static CCamera_Player_DW2* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};

END