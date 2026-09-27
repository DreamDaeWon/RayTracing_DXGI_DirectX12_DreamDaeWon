#include "Collider_Rect.h"

CCollider_Rect::CCollider_Rect(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CCollider(pGraphic_Device)
{
}

CCollider_Rect::CCollider_Rect(const CCollider_Rect& rhs) :
	CCollider(rhs)
{
}

HRESULT CCollider_Rect::Initialize_Prototype()
{

    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
    m_dwPrimitive = 2;

    //m_pVerticesPos = new _float3[m_iNumVertices];

    m_iIndexStride = m_iNumVertices >= 65535 ? 4 : 2;
    m_iNumIndices = 6;
    m_eIndexFormat = m_iIndexStride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;

    //For. VertexBuffer

    /* 정점 여섯개의 배열 공간을 할당한다. */
    if (FAILED(__super::Create_VertexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_VertexBuffer : __super,CVIBuffer_Rect"));
        return E_FAIL;
    }

    VTXPOSTEX* pVertex = { nullptr };

    /* 할당된 정점 배열공간의 가장 앞 주소를 얻어올 수 있다. */
    /* 이 공간을 걸어 잠근다. */
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    pVertex[0].vPosition  = _float3{ -0.5f, 0.5f, 0.f };
    pVertex[0].vTexcoord = _float2{ 0.f, 0.f };

    pVertex[1].vPosition  = _float3{ 0.5f, 0.5f, 0.f };
    pVertex[1].vTexcoord = _float2{ 1.f, 0.f };

    pVertex[2].vPosition  = _float3{ 0.5f, -0.5f, 0.f };
    pVertex[2].vTexcoord = _float2{ 1.f, 1.f };

    pVertex[3].vPosition  = _float3{ -0.5f, -0.5f,0.f };
    pVertex[3].vTexcoord = _float2{ 0.f, 1.f };

    m_pVB->Unlock();

    //For. IndexBuffer
    if (FAILED(__super::Create_IndexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_IndexBuffer : __super,CVIBuffer_Rect"));
        return E_FAIL;
    }

    _ushort* pIndices = { nullptr };
    //_ushort* pIndicesInfo = new _ushort[m_iNumIndices];
    m_pIB->Lock(0, 0, (void**)&pIndices, 0);

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;
                
    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    //m_pIndicesInfo = pIndicesInfo;
    m_pIB->Unlock();


    D3DXMatrixIdentity(&m_WorldMatrix);
    return S_OK;
}

HRESULT CCollider_Rect::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCollider_Rect::Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    //m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

    /*_float3 vScale = Get_Scale();
    m_pGraphic_Device->GetTransform(D3DTS_WORLD, &m_WorldMatrix);
    Set_Scale(vScale);*/
    m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
   /* Update_Collider_Info(m_WorldMatrix);*/
    __super::Render();
    m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    return S_OK;
}

void CCollider_Rect::Update_Collider_Info(_float4x4 vTargetWorldMatrix)
{
    _float3 vScale = Get_Scale();

    //타겟의 라업룩으로 콜라이더를 돌려줌.
    _float3 vRight = { vTargetWorldMatrix.m[STATE_RIGHT][0], vTargetWorldMatrix.m[STATE_RIGHT][1], vTargetWorldMatrix.m[STATE_RIGHT][2] };
    _float3 vUp = { vTargetWorldMatrix.m[STATE_UP][0], vTargetWorldMatrix.m[STATE_UP][1], vTargetWorldMatrix.m[STATE_UP][2] };
    _float3 vLook = { vTargetWorldMatrix.m[STATE_LOOK][0], vTargetWorldMatrix.m[STATE_LOOK][1], vTargetWorldMatrix.m[STATE_LOOK][2] };
    _float3 vPosition = { vTargetWorldMatrix.m[STATE_POSITION][0], vTargetWorldMatrix.m[STATE_POSITION][1], vTargetWorldMatrix.m[STATE_POSITION][2] };
    
    vRight = *D3DXVec3Normalize(&vRight, &vRight) * vScale.x;
    vUp = *D3DXVec3Normalize(&vUp, &vUp) * vScale.y;
    vLook = *D3DXVec3Normalize(&vLook, &vLook) * vScale.z;

    Set_State(STATE_RIGHT, vRight);
    Set_State(STATE_UP, vUp);
    Set_State(STATE_LOOK, vLook);
    Set_State(STATE_POSITION, vPosition);

    //사각형을 구성하는 네 점을 업데이트 해줌. 012 023 으로 그릴 것이다.
    m_tRect.LeftTop = vPosition - vRight * 0.5f + vUp * 0.5f; //왼쪽위
    m_tRect.RightTop = vPosition + vRight * 0.5f + vUp * 0.5f; //오른쪽 위
    m_tRect.RightBottom = vPosition + vRight * 0.5f - vUp * 0.5f; //오른쪽 아래
    m_tRect.LeftBottom = vPosition - vRight * 0.5f - vUp * 0.5f; //왼쪽 아래
}



CCollider_Rect* CCollider_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCollider_Rect* pInstance = new CCollider_Rect(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider_Rect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider_Rect::Clone(void* pArg)
{
    CCollider_Rect* pInstance = new CCollider_Rect(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider_Rect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_Rect::Free()
{
    __super::Free();
}
