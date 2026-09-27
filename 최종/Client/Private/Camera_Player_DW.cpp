#include "pch.h"
#include "Camera_Player_DW.h"
#include "GameInstance.h"
#include "Player.h"
_uint g_eLevel;
_bool g_UI = true;// Ui를 사용할건지?
CCamera_Player_DW::CCamera_Player_DW(LPDIRECT3DDEVICE9 pGrahpic_Device)
    : CCamera(pGrahpic_Device)
{

}

CCamera_Player_DW::CCamera_Player_DW(const CCamera_Player_DW& rhs)
    : CCamera(rhs)
{

}

HRESULT CCamera_Player_DW::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Player_DW::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        CAMERA_PLAYER_DW_DESC*  Camera_Player_Desc = (CAMERA_PLAYER_DW_DESC*)pArg;
        m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag));
        m_pPlayer = Camera_Player_Desc->pPlayer;
        Safe_AddRef(m_pPlayer);
        Safe_AddRef(m_pPlayerTransform);
       //Camera_Player_Desc->fSpeedPerSec = 1.f;
        Camera_Player_Desc->fRotationPerSec = 1.f;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    //GetCursorPos(&m_ptMouse);

    // 3인칭 뷰 시점 수정
    m_f3ViewUp = { 0.f };
    m_f3ViewBack = { 0.f };

    // 오른쪽 뷰 시점 수정
    m_fRightViewRight = { 0.06f };
    m_fRightViewUp = { 0.02f };
    m_fRightViewBack = { 0.1f };
    //m_eCamera_Mode == CAMERA_3VIEW;
    return S_OK;
}

_uint CCamera_Player_DW::Tick(_float fTimeDelta)
{
   if (m_eCamera_Mode == CAMERA_BOSS_END)
   {
           m_bZoom = false;
           m_eCamera_Mode = CAMERA_BOSS_END;
           if (FAILED(__super::Bind_PipeLines()))
               return 1;
   }

    if(m_eCamera_Mode != CAMERA_BOSS)
    {

        if (m_eCamera_Mode == CAMERA_QUTER)
        {
            m_eCamera_Mode = 0;
        }

        if (m_pPlayer->Get_CurState() == CPlayer::STATE_AIMING || m_pPlayer->Get_CurState() == CPlayer::STATE_AIMING_WALK || m_pPlayer->Get_CurState() == CPlayer::STATE_AIMING_ZOOM)
        {
            m_eCamera_Mode = CAMERA_PLAYER_RIGHT_VIEW;
        }
        else
        {
            m_eCamera_Mode = CAMERA_3VIEW;
            m_fCamera_Player_Right_Time = 0.f;
        }

        if (GetKeyState('V') & 0x0001)
            m_eCamera_Mode = CAMERA_FREE;

        if (m_pGameInstance->Key_Down('L'))
        {
            if (!m_bCameraTrapLevelUp)
            {
                m_fCamera_Change_Time = 0.5f;
            }
            m_bCameraTrapLevelUp = !m_bCameraTrapLevelUp;

        }

        if (m_pPlayer->Get_CurState() == CPlayer::STATE_AIMING_ZOOM)
        {
            m_bZoom = true;
        }
        else
            m_bZoom = false;
    }
    else if(m_eCamera_Mode == CAMERA_BOSS)
    {
        m_eCamera_Mode = CAMERA_BOSS;
    }

    if (FAILED(__super::Bind_PipeLines()))
        return 1;

    return OBJECT_NOTHING;
}

void CCamera_Player_DW::Late_Tick(_float fTimeDelta)
{
    if (LEVEL_LOADING == m_pGameInstance->Get_Level() || LEVEL_1945 == m_pGameInstance->Get_Level())
        return;

    if (m_eCamera_Mode == CAMERA_BOSS_END)
    {
        Camera_Boss_End(fTimeDelta);
        return;
    }
    // 예외처리
    if (m_eCamera_Mode == CAMERA_FREE)
    {
        Camera_Free_Mode(fTimeDelta);
    }
    else if (m_eCamera_Mode == CAMERA_1VIEW)
    {
        Camera_1View_Mode(fTimeDelta);
        m_ptMouse = { (_long)(g_iWinSizeX * 0.5), (_long)(g_iWinSizeY * 0.5) };
        ClientToScreen(g_hWnd, &m_ptMouse);
        SetCursorPos(m_ptMouse.x, m_ptMouse.y);
    }
    else if (m_eCamera_Mode == CAMERA_3VIEW)
    {
        Camera_3View_Mode(fTimeDelta);
    }
    else if (m_eCamera_Mode == CAMERA_PLAYER_RIGHT_VIEW)
    {
        Camera_Player_Right(fTimeDelta);
        m_ptMouse = { (_long)(g_iWinSizeX * 0.5), (_long)(g_iWinSizeY * 0.5) };
        ClientToScreen(g_hWnd, &m_ptMouse);
        SetCursorPos(m_ptMouse.x, m_ptMouse.y);
    }
    else if (m_eCamera_Mode == CAMERA_BOSS)
    {
        if (m_pGameInstance->Get_List(LEVEL_SNOWBOSS, TEXT("Layer_Monster"))->size() != 0)
        {
            m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(20.f, 0.3f, 20.f));
            CTransform* pBossTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Monster"), g_strTransformTag));
            m_pTransform->LookAt(pBossTransform->Get_State(CTransform::STATE_POSITION));
        }
    }

    if (m_bShake)
    {
        Camera_Shaking(fTimeDelta);
    }
    if (m_pGameInstance->Key_Down('Q'))
        m_bRebound = true;
    if (m_bRebound)
    {
        Camera_Rebound(fTimeDelta);
    }

    Camera_ZOOM_Mode(fTimeDelta);
    //TODO: 용수 벽충돌
    if(g_eLevel != LEVEL_SNOWBOSS)
    {
        Wall_Collision();
    }
}

void CCamera_Player_DW::Camera_Free_Mode(_float fTimeDelta)
{
    ShowCursor(true); // DW질문 : 동작 속도가 많이 느린가?


    if (GetKeyState(VK_UP) & 0x8000)
        m_pTransform->Go_Straight(fTimeDelta);
    if (GetKeyState(VK_DOWN) & 0x8000)
        m_pTransform->Go_Backward(fTimeDelta);
    if (GetKeyState(VK_LEFT) & 0x8000)
        m_pTransform->Go_Left(fTimeDelta);
    if (GetKeyState(VK_RIGHT) & 0x8000)
        m_pTransform->Go_Right(fTimeDelta);




    if (FAILED(__super::Bind_PipeLines()))
        return;

}

void CCamera_Player_DW::Camera_3View_Mode(_float fTimeDelta)
{
    m_fCamera_Player_Right_Time = 0.f;
    m_fAngle = 0.f;
    m_pTransform->Set_State(CTransform::STATE_POSITION, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

    // 카메라가 이전 상태처럼 아래로 치우치면 안되기 때문에 Look와 Right를 010벡터와 외적하여 다시계산
    // 이 때 플레이어에게 고정시키면 안돌아감 카메라에 따라서 플레이어가 돌아가기 때문에
    _float3 vCameraLook = { m_pTransform->Get_State(CTransform::STATE_LOOK) };
    _float3 vCameraRight = { m_pTransform->Get_State(CTransform::STATE_RIGHT) };
    D3DXVec3Cross(&vCameraLook, &vCameraRight, &_float3(0.f, 1.f, 0.f));
    D3DXVec3Cross(&vCameraRight, &_float3(0.f, 1.f, 0.f), &vCameraLook);

    m_pTransform->Set_State(CTransform::STATE_UP, m_pPlayerTransform->Get_State(CTransform::STATE_UP));
    m_pTransform->Set_State(CTransform::STATE_LOOK, vCameraLook);
    m_pTransform->Set_State(CTransform::STATE_RIGHT, vCameraRight);

    /* 스크린 기준의 마우스 위치를 얻어온다. */
    //GetCursorPos(&ptMouse);


    /* 왼쪽으로 움직이면 -, 오른쪽으로 움짖ㄱ였다 + */
    /*_long MouseMoveX = ptMouse.x - m_ptMouse.x;

    if (0 != MouseMoveX)
    {
        m_pTransform->Turn(m_pPlayerTransform->Get_State(CTransform::STATE_UP), fTimeDelta * MouseMoveX * m_fMouseSensor);
    }*/


    // 카메라의 위치를 축에따라서 플레이어 뒤쪽으로 이동
    if (m_fCamera_Change_Time < 1.f) // 서서히 이동
    {
        m_fCamera_Change_Time += 0.05f;
    }

    //m_pTransform->Go_Backward(m_fCamera_Change_Time);

    if(m_bCameraTrapLevelUp)
    {
        m_pTransform->Go_Backward(m_fCamera_Change_Time * 0.3f);
        m_pTransform->Go_Up(1.f * m_fCamera_Change_Time);
    }
    // 원본
    else
    {
        m_pTransform->Go_Backward(m_fCamera_Change_Time);
        m_pTransform->Go_Up(0.5f * m_fCamera_Change_Time);
    }

    // 카메라가 플레이어를 보게 만들기
    m_pTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

    // 플레이어는 카메라의 룩벡터 방향을 보지만 아래로 치우치면 안되기 때문에 Look와 Right를 010벡터와 외적하여 다시계산
   /* _float3 vLook = { m_pTransform->Get_State(CTransform::STATE_LOOK) };
    _float3 vRight = { m_pTransform->Get_State(CTransform::STATE_RIGHT) };
    D3DXVec3Cross(&vLook, &vRight, &_float3(0.f, 1.f, 0.f));
    D3DXVec3Cross(&vRight, &_float3(0.f, 1.f, 0.f), &vLook);*/


    //m_pPlayerTransform->Set_State(CTransform::STATE_LOOK, vLook);
    //m_pPlayerTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));
    //m_pPlayerTransform->Set_State(CTransform::STATE_RIGHT, vRight);
    if (FAILED(__super::Bind_PipeLines()))
        return;

   /* ptMouse = { (long)(g_iWinSizeX * 0.5),(long)(g_iWinSizeY * 0.5) };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);

    m_ptMouse = ptMouse;*/
}

void CCamera_Player_DW::Camera_1View_Mode(_float fTimeDelta)
{
    ShowCursor(false);
    m_pTransform->Set_State(CTransform::STATE_POSITION, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

    // 카메라가 이전 상태처럼 아래로 치우치면 안되기 때문에 Look와 Right를 010벡터와 외적하여 다시계산
    // 이 때 플레이어에게 고정시키면 안돌아감 카메라에 따라서 플레이어가 돌아가기 때문에
    _float3 vCameraLook = { m_pTransform->Get_State(CTransform::STATE_LOOK) };
    _float3 vCameraRight = { m_pTransform->Get_State(CTransform::STATE_RIGHT) };
    D3DXVec3Cross(&vCameraLook, &vCameraRight, &_float3(0.f, 1.f, 0.f));
    D3DXVec3Cross(&vCameraRight, &_float3(0.f, 1.f, 0.f), &vCameraLook);

    m_pTransform->Set_State(CTransform::STATE_UP, m_pPlayerTransform->Get_State(CTransform::STATE_UP));
    m_pTransform->Set_State(CTransform::STATE_LOOK, vCameraLook);
    m_pTransform->Set_State(CTransform::STATE_RIGHT, vCameraRight);

    m_pPlayerTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK));
    m_pPlayerTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT));

    if (FAILED(__super::Bind_PipeLines()))
        return;
}

void CCamera_Player_DW::Camera_Quter_Mode(_float fTimeDelta)
{

}

void CCamera_Player_DW::Camera_ZOOM_Mode(_float fTimeDelta)
{
    if (m_bZoom)
    {
        //m_fBeforeFov = D3DXToDegree(m_fFovy);
        //m_fFovy = D3DXToRadian(30.f);
        if (m_fZoomFov < 40.f)
        {

            m_fZoomFov += 40.f * fTimeDelta*2.f;
        }
        else
        {
            m_fZoomFov = 40.f;
        }
        _float Zoom = 60.f - m_fZoomFov;
        m_fFovy = D3DXToRadian(60.f - m_fZoomFov);
        //if (FAILED(__super::Bind_PipeLines()))
           //return;
    }
    else
    {
        m_fZoomFov = 0.f;
        m_fFovy = D3DXToRadian(60.f);
        m_fBeforeFov = D3DXToDegree(m_fFovy);
       // if (FAILED(__super::Bind_PipeLines()))
          // return;
    }

}

void CCamera_Player_DW::Camera_Player_Right(_float fTimeDelta)
{

    m_fRightViewRight = { 0.06f };
    m_fRightViewUp = { 0.48f };
    m_fRightViewBack = { 0.9f };

    m_fCamera_Change_Time = 0.f;
    // 이게 진짜 코드야1!!!!
    ////////////////////// 대원TODO : 이거 상하좌우 다 움직이게 할 것
    m_pTransform->Set_State(CTransform::STATE_POSITION, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
    CPlayer* pPlayerObject = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));

    _float3 vCameraLook = { m_pTransform->Get_State(CTransform::STATE_LOOK) };
    _float3 vCameraRight = { m_pTransform->Get_State(CTransform::STATE_RIGHT) };
    D3DXVec3Cross(&vCameraLook, &vCameraRight, &_float3(0.f, 1.f, 0.f));
    D3DXVec3Cross(&vCameraRight, &_float3(0.f, 1.f, 0.f), &vCameraLook);


    if (m_fCamera_Player_Right_Time == 0.f)
    {
        _float3 vMouse = pPlayerObject->Get_MousePos(); // 마우스 위치를 가져 옴

        // 마우스 위치에서 현재 카메라의 위치를 빼서 방향벡터를 구함
   

         vMouse = (vMouse - m_pTransform->Get_State(CTransform::STATE_POSITION)); 
       
        D3DXVec3Normalize(&vMouse, &vMouse); // 내적하기 위해 정규화


        m_pTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));
        m_vBeforeLook = m_pTransform->Get_State(CTransform::STATE_LOOK); // 들어왔을 때의 룩벡터 저장
        m_vBeforeRight = m_pTransform->Get_State(CTransform::STATE_RIGHT); // 들어왔을 때의 라이트 벡터 저장

        D3DXVec3Normalize(&m_vBeforeLook, &m_vBeforeLook);
        D3DXVec3Normalize(&m_vBeforeRight, &m_vBeforeRight);

        D3DXVec3Cross(&m_vBeforeLook, &m_vBeforeRight, &_float3(0.f, 1.f, 0.f)); //
        D3DXVec3Cross(&m_vBeforeRight, &_float3(0.f, 1.f, 0.f), &m_vBeforeLook);

        D3DXVec3Normalize(&m_vBeforeLook, &m_vBeforeLook);
        D3DXVec3Normalize(&m_vBeforeRight, &m_vBeforeRight);

        // Right 벡터와 내적후 양수면 오른쪽 음수면 왼쪽으로 돌아야 한다.
        if (D3DXVec3Dot(&m_vBeforeRight, &vMouse) >= 0.f)
        {
            m_fAngle = D3DXVec3Dot(&m_vBeforeLook, &vMouse);
            if (m_fAngle >= 1.f)
                m_fAngle = 1.f;
            else if (m_fAngle <= -1.f)
                m_fAngle = -1.f;
            m_fAngle = acosf(m_fAngle);
            m_fAngle = D3DXToDegree(m_fAngle);
            if (D3DXVec3Dot(&m_vBeforeLook, &vMouse) >= 0.f) // 이게 양수면 그냥 돌면 된다.
            {

            }
            else
            {
                m_fAngle = m_fAngle;
                
            }
        }
        else // 왼쪽
        {
            m_fAngle = D3DXVec3Dot(&m_vBeforeLook, &vMouse);
            if (m_fAngle >= 1.f)
                m_fAngle = 1.f;
            else if (m_fAngle <= -1.f)
                m_fAngle = -1.f;
            m_fAngle = acosf(m_fAngle);
            m_fAngle = D3DXToDegree(m_fAngle);
            if (D3DXVec3Dot(&m_vBeforeLook, &vMouse) >= 0.f) // 이게 양수면 그냥 돌면 된다.
            {
               // m_fAngle += m_fAngle * 0.3f;
            }
            else
            {
                //m_fAngle += m_fAngle * 0.2f;
            }
            m_fAngle += m_fAngle * 0.2f;
            m_fAngle = -m_fAngle;
        }
      
    }


   

 
    

    m_fRightViewRight = { 0.06f };
    m_fRightViewUp = { 0.48f };
    m_fRightViewBack = { 0.9f };

    if (m_fCamera_Player_Right_Time < 0.5f) // 서서히 이동
    {
        m_bCanShotBullet = false;
        // 서서히 이동 할 때는 정면을 보도록 설정
        // 위치랑 축들 다시 돌리고
       
        m_pTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));
        m_pTransform->Set_State(CTransform::STATE_LOOK, m_vBeforeLook);
        m_pTransform->Set_State(CTransform::STATE_RIGHT, m_vBeforeRight);


        // 돌리고
        m_pTransform->Turn(_float3(0.f, 1.f, 0.f), (m_fAngle * m_fCamera_Player_Right_Time) * 0.02f); // 돌기


        // 뒤로빼고
        m_pTransform->Go_Backward(1.f);
        m_pTransform->Go_Up(0.5f);

     
 
        // 앞으로 보내고
        m_pTransform->Go_Right(m_fRightViewRight * m_fCamera_Player_Right_Time * 2.f);
        m_pTransform->Go_Down(m_fRightViewUp * m_fCamera_Player_Right_Time * 2.f);
        m_pTransform->Go_Straight(m_fRightViewBack * m_fCamera_Player_Right_Time * 2.f );

        m_fCamera_Player_Right_Time += fTimeDelta;
    }
    else
    {
        m_bCanShotBullet = true;
        m_pPlayerTransform->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));
        m_pPlayerTransform->Set_State(CTransform::STATE_LOOK, vCameraLook);
        m_pPlayerTransform->Set_State(CTransform::STATE_RIGHT, vCameraRight);

        m_pTransform->Go_Backward(1.f);
        m_pTransform->Go_Up(0.5f);

       m_pTransform->Go_Right(m_fRightViewRight * m_fCamera_Player_Right_Time * 2.f);
       m_pTransform->Go_Down(m_fRightViewUp * m_fCamera_Player_Right_Time * 2.f);
       m_pTransform->Go_Straight(m_fRightViewBack * m_fCamera_Player_Right_Time * 2.f);

      
    }
   
 


   if (FAILED(__super::Bind_PipeLines()))
       return;


}

void CCamera_Player_DW::Camera_Shaking(_float fTimeDelta)
{

    if (m_eCamera_Mode == CAMERA_BOSS)
    {
        m_fShakeSize = 0.1f;
        m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x + m_fShakeSize * m_fShakePlusMinus,
            m_pTransform->Get_State(CTransform::STATE_POSITION).y + m_fShakeSize * m_fShakePlusMinus,
            m_pTransform->Get_State(CTransform::STATE_POSITION).z));
        m_fShakePlusMinus = -m_fShakePlusMinus;
        if (FAILED(__super::Bind_PipeLines()))
            return;
        return;
    }
   // m_fShakeSize = 0.05f;
    m_fShakeSize = 0.05f;
    m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_pTransform->Get_State(CTransform::STATE_POSITION).x + m_fShakeSize * m_fShakePlusMinus,
        m_pTransform->Get_State(CTransform::STATE_POSITION).y + m_fShakeSize * m_fShakePlusMinus,
        m_pTransform->Get_State(CTransform::STATE_POSITION).z));
    m_fShakePlusMinus = -m_fShakePlusMinus;
    if (FAILED(__super::Bind_PipeLines()))
        return;
}

void CCamera_Player_DW::Camera_Rebound(_float fTimeDelta)
{
	m_fReboundTime += fTimeDelta * m_fReboundSpeed;

    m_pTransform->Turn(-m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta*m_fReboundSize);
    if (m_fReboundSize > 0.f && m_fReboundSize * m_fReboundTime >= m_fReboundSize)
    {
        m_fReboundSize = m_fReboundSize * (-1.f);
        m_fReboundTime = 0.f;
    }
    else if (m_fReboundSize < 0.f && m_fReboundSize * m_fReboundTime <= m_fReboundSize)
    {
		m_fReboundSize = m_fReboundSize * (-1.f);
        m_bRebound = false; 
        m_fReboundTime = 0.f;

    }

    if (FAILED(__super::Bind_PipeLines()))
        return;
}

void CCamera_Player_DW::Camera_Boss_End(_float fTimeDelta)
{

    switch (m_eBoss_End_Camera_Now_State)
    {
    case BOSS_END_READY_LOOK_BOSS:
    {
        g_UI = false;
        //m_pGameInstance->StopAll();
        //m_pGameInstance->PlayBGM(L"End_BGM.wav");
        //m_pGameInstance->VolumeDown(CSound_Manager::CHANNEL_BGM,0.1f);

        m_bCanShotBullet = false;
        m_bCameraTrapLevelUp = false;
        m_bShake = false;
        CTransform* pBossTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Monster"), g_strTransformTag));
        m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(pBossTransform->Get_State(CTransform::STATE_POSITION).x, 0.3f, pBossTransform->Get_State(CTransform::STATE_POSITION).z - 5.f));
        m_pTransform->LookAt(pBossTransform->Get_State(CTransform::STATE_POSITION));
        m_pTransform->Go_Left((5.f/10.f));

        m_fBossEndTime = 0.f;
        m_eBoss_End_Camera_Now_State = BOSS_END_LOOK_BOSS;
        break;
    }
    case BOSS_END_LOOK_BOSS:
        if (m_fBossEndTime <= 4.f)
        {
            m_pTransform->Go_Right((8.f * fTimeDelta / 4.f) / 10.f);
            m_fBossEndTime += fTimeDelta;
        }
        else
        {
            m_fBossEndTime = 0.f;
            m_eBoss_End_Camera_Now_State = BOSS_END_READY_LOOK_PLAYER;
        }
        break;
    case BOSS_END_READY_LOOK_PLAYER:
    {
        // 보스 트렌스폼 가져오기
        CTransform* pBossTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Monster"), g_strTransformTag));
       //플레이어가 보스를 보도록(010벡터와 보정 되어있음)
        m_pPlayerTransform->LookAt(pBossTransform->Get_State(CTransform::STATE_POSITION));
        m_pPlayer->SetGameLook(pBossTransform->Get_State(CTransform::STATE_POSITION) - m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
    
    //카메라가 플레이어의 앞으로 가서 얼굴을 보도록========
        // 카메라가 플레이어의 위치로 이동
        m_pTransform->Set_State(CTransform::STATE_POSITION,m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));
        
        // 카메라가 플레이어랑 똑같이 보스를 바라보도록
        m_pTransform->LookAt(pBossTransform->Get_State(CTransform::STATE_POSITION));

        // 카메라를 앞으로 이동
        m_pTransform->Go_Straight(2.f / 10.f);
        // 카메라가 플레이어를 바라보도록
        m_pTransform->LookAt(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

        // 그 후 왼쪽으로 이동
        m_pTransform->Go_Left(3.f / 10.f);



        // 모두 설정완료 되면
        m_fBossEndTime = 0.f;
        m_eBoss_End_Camera_Now_State = BOSS_END_LOOK_PLAYER;
        break;
    }
    case BOSS_END_LOOK_PLAYER:
        if (m_fBossEndTime <= 3.f) // 2초동안 이동
        {
            m_pTransform->Go_Right((4.f * fTimeDelta / 3.f) / 10.f); // 아까 왼쪽으로 간 거리 *2 만큼 이동
            m_fBossEndTime += fTimeDelta;
        }
        else
        {
            m_fBossEndTime = 0.f;
            m_eBoss_End_Camera_Now_State = BOSS_END_ZOOM_OUT_READY;
        }
        break;
    case BOSS_END_ZOOM_OUT_READY:
    {
        // 보스의 트렌스폼을 가져옴
        CTransform* pBossTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_SNOWBOSS, TEXT("Layer_Monster"), g_strTransformTag));
        
        // 보스와 플레이어의 가운데 위치 좌표를 구해 줌
        _float3 m_vCenter = { (pBossTransform->Get_State(CTransform::STATE_POSITION) + m_pPlayerTransform->Get_State(CTransform::STATE_POSITION)) * 0.5f };
        
        // 조금 위와 뒤로 빠지기
        m_pTransform->Set_State(CTransform::STATE_POSITION, _float3(m_vCenter.x, m_vCenter.y + 4.f, m_vCenter.z - 5.f));

        // 플레이어와 보스의 가운데를 바라보기
        m_pTransform->LookAt(m_vCenter);

        
        // 모두 설정완료 되면
        m_fBossEndTime = 0.f;
        m_eBoss_End_Camera_Now_State = BOSS_END_ZOOM_OUT;
        break;
    }
    case BOSS_END_ZOOM_OUT:
        if (m_fBossEndTime <= 4.f) // 뒤로 빼는 시간
        {
            _float m_BackSpeed = 6.f;
            m_pTransform->Go_Backward(m_BackSpeed * fTimeDelta / 10.f); //뒤로 빠지는 속도
            m_fBossEndTime += fTimeDelta;
        }
        else // 다 뒤로 뺐으면
        {
            m_fBossEndTime = 0.f;
            m_eBoss_End_Camera_Now_State = BOSS_END_LOOK_UP;
        }
        break;

    case BOSS_END_LOOK_UP:

        if (m_fBossEndTime < 2.0f)
        {
            m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), D3DXToRadian(-90.f * fTimeDelta / 2.0f));
            m_fBossEndTime += fTimeDelta;
        }
        else
        {
            m_fBossEndTime = 0.f;
            m_eBoss_End_Camera_Now_State = BOSS_END_SHOW_CREADIT;
        }

        break;
    case BOSS_END_SHOW_CREADIT:

        break;
    default:
        break;
    }

    if (FAILED(__super::Bind_PipeLines()))
        return;

}

void CCamera_Player_DW::Wall_Collision()
{
    _float3 vRayPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    _float3 vRayDir = m_pTransform->Get_State(CTransform::STATE_POSITION) - vRayPos;
    _float fLength = D3DXVec3Length(&vRayDir);
    D3DXVec3Normalize(&vRayDir, &vRayDir);

    list<CGameObject*>* pWallList = m_pGameInstance->Get_List(g_eLevel, TEXT("Layer_Wall"));
    if (nullptr == pWallList)
        return;

    _float fMinDist(1000.f); //큰 값으로 초기화
    for (auto& iter : *pWallList)//Wall List 전부를 순회하면서 충돌을 판단한다.
    {
        CCollider_Rect* pCollider_Rect = dynamic_cast<CCollider_Rect*>(iter->Get_Component(TEXT("Com_Collider_Rect")));
        if (nullptr == pCollider_Rect)
            continue;
        _float fDist(1000.f);
        if (m_pGameInstance->Collision_Rect_Ray_Same_Space(pCollider_Rect, vRayDir, vRayPos, fLength, &fDist)) //충돌이 일어났을 때
        {
            if (fMinDist > fDist) //
            {
                fMinDist = fDist;
                m_pTransform->Set_State(CTransform::STATE_POSITION, vRayPos + vRayDir * (fMinDist - 1.f));
                __super::Bind_PipeLines();
            }
        }
    }
}

CCamera_Player_DW* CCamera_Player_DW::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCamera_Player_DW* pInstance = new CCamera_Player_DW(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CCamera_Player_DW"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Player_DW::Clone(void* pArg)
{
    CCamera_Player_DW* pInstance = new CCamera_Player_DW(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CCamera_Player_DW"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Player_DW::Free()
{
    Safe_Release(m_pPlayerTransform);
    Safe_Release(m_pPlayer);
    __super::Free();
}