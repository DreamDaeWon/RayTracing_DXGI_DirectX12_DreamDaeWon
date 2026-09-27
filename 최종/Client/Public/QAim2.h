#pragma once

#include "UI_Base.h"
#include"Client_Defines.h"

BEGIN(Client)
class CQAim2 final : public CUI_Base
{
public:
    typedef struct tagQAimDesc : public CGameObject::GAMEOBJECT_DESC
    {
        _float               fMouseSensor = { };

    }QAIM_DESC2;
private:
    CQAim2(LPDIRECT3DDEVICE9 pGraphic_Device);
    CQAim2(const CQAim2& rhs);
    virtual ~CQAim2() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual _uint Tick(_float fTimeDelta);
    virtual void Late_Tick(_float fTimeDelta);
    virtual HRESULT Render();

private:
    virtual HRESULT Add_Components() override;
    virtual HRESULT Set_RenderState(_ulong lAphaRef = 0) override;
    virtual HRESULT Reset_RenderState() override;
private:
    _float               m_fMouseSensor = { 1.0f };
    _float               m_fTime = { 0.f };
    POINT               m_ptCurMouse = {};
    POINT               m_ptMouse = {};


    CTransform* m_pCameraTransform = { nullptr }; // 카메라의 트랜스폼
    class CCamera_Player_DW2* m_pCameraObject = { nullptr }; // 카메라의 오브젝트

    _float               m_fMaxX = { 0.f }; // 얼마나 넘어가면 화면을 돌릴지에 대한 Max값
    _float               m_fMinX = { 0.f }; // 얼마나 넘어가면 화면을 돌릴지에 대한 Min값

    _float               m_fMaxY = { 0.f };// 얼마나 넘어가면 화면을 돌릴지에 대한 Max값
    _float               m_fMinY = { 0.f };// 얼마나 넘어가면 화면을 돌릴지에 대한 Min값

    _float               m_fNowCurX = { 0.f }; // 현재 마우스의 X위치 값 조준점의 좌표
    _float               m_fNowCurY = { 0.f }; // 현재 마우스의 Y위치 값 조준점의 좌표

    _float               m_fTrunSpeed = { 0.f }; // 화면 회전 속도 값

    _float               m_fAngle = { 0.f }; // 현재 Y시야범위각도

    _float               m_fAngleY = { 0.f }; // 최대 Y시야범위각도

    wstring               m_strOutAngle = {}; // 띄울 텍스트

public:
    static CQAim2* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;


};

END