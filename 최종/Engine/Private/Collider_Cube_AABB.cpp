#include "Collider_Cube_AABB.h"

CCollider_Cube_AABB::CCollider_Cube_AABB(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CCollider(pGraphic_Device)
{
}

CCollider_Cube_AABB::CCollider_Cube_AABB(const CCollider_Cube_AABB& rhs) :
	CCollider(rhs),
    m_vMaxPoint(rhs.m_vMaxPoint),
    m_vMinPoint(rhs.m_vMinPoint)
{
}

HRESULT CCollider_Cube_AABB::Initialize_Prototype()
{
    m_iNumVertices = 8;
    m_iVertexStride = sizeof(VTXCUBE);
    m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
    m_dwPrimitive = 12;


    m_iIndexStride = m_iNumVertices >= 65535 ? 4 : 2;
    m_iNumIndices = 36;
    m_eIndexFormat = m_iIndexStride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;

    //For. VertexBuffer

    /* 정점 여섯개의 배열 공간을 할당한다. */
    if (FAILED(__super::Create_VertexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_VertexBuffer : __super,CVIBuffer_Cube"));
        return E_FAIL;
    }

    VTXCUBE* pVertex = { nullptr };

    /* 할당된 정점 배열공간의 가장 앞 주소를 얻어올 수 있다. */
    /* 이 공간을 걸어 잠근다. */

    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    pVertex[0].vPosition = _float3{ -0.5f, 0.5f, -0.5f };
    pVertex[0].vTexcoord = pVertex[0].vPosition;

    pVertex[1].vPosition = _float3{ 0.5f, 0.5f, -0.5f };
    pVertex[1].vTexcoord = pVertex[1].vPosition;

    pVertex[2].vPosition = _float3{ 0.5f, -0.5f, -0.5f };
    pVertex[2].vTexcoord = pVertex[2].vPosition;

    pVertex[3].vPosition = _float3{ -0.5f, -0.5f, -0.5f };
    pVertex[3].vTexcoord = pVertex[3].vPosition;

    pVertex[4].vPosition = _float3{ -0.5f, 0.5f, 0.5f };
    pVertex[4].vTexcoord = pVertex[4].vPosition;

    pVertex[5].vPosition = _float3{ 0.5f, 0.5f, 0.5f };
    pVertex[5].vTexcoord = pVertex[5].vPosition;

    pVertex[6].vPosition = _float3{ 0.5f, -0.5f, 0.5f };
    pVertex[6].vTexcoord = pVertex[6].vPosition;

    pVertex[7].vPosition = _float3{ -0.5f, -0.5f, 0.5f };
    pVertex[7].vTexcoord = pVertex[7].vPosition;

    m_pVB->Unlock();

    //For. IndexBuffer
    if (FAILED(__super::Create_IndexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_IndexBuffer : __super,CVIBuffer_Cube"));
        return E_FAIL;
    }

    _ushort* pIndices = { nullptr };
    m_pIB->Lock(0, 0, (void**)&pIndices, 0);

    /* +x */
    pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
    pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;
    /* -x */
    pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
    pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;
    /* +y */
    pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
    pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;
    /* -y */
    pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
    pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;
    /* +z */
    pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
    pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;
    /* -z */
    pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
    pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

    m_pIB->Unlock();

    D3DXMatrixIdentity(&m_WorldMatrix);

	return S_OK;
}

HRESULT CCollider_Cube_AABB::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCollider_Cube_AABB::Render()
{
    m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);

    /*_float3 vScale = Get_Scale();
    m_pGraphic_Device->GetTransform(D3DTS_WORLD, &m_WorldMatrix);
    Set_Scale(vScale);*/
    m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
    /*m_pGraphic_Device->GetTransform(D3DTS_WORLD, &m_WorldMatrix);
    Update_Collider_Info(m_WorldMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);*/
    __super::Render();
    m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    return S_OK;
}

void CCollider_Cube_AABB::Update_Collider_Info(_float4x4 vTargetWorldMatrix)
{
    _float3 vScale = Get_Scale();
    //타겟의 라업룩으로 콜라이더를 돌려줌.
    _float3 vPosition = { vTargetWorldMatrix.m[STATE_POSITION][0], vTargetWorldMatrix.m[STATE_POSITION][1], vTargetWorldMatrix.m[STATE_POSITION][2] };
    
    Set_State(STATE_RIGHT, _float3(1.f,0.f,0.f) * vScale.x);
    Set_State(STATE_UP,  _float3(0.f, 1.f, 0.f) * vScale.y);
    Set_State(STATE_LOOK,  _float3(0.f, 0.f, 1.f) * vScale.z);
    Set_State(STATE_POSITION, vPosition);

    //vMinPoint vMaxPoint 업데이트
    m_vMinPoint = vPosition - vScale * 0.5f;
    m_vMaxPoint = vPosition + vScale * 0.5f;
   
}



CCollider_Cube_AABB* CCollider_Cube_AABB::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CCollider_Cube_AABB* pInstance = new CCollider_Cube_AABB(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider_Cube_AABB"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider_Cube_AABB::Clone(void* pArg)
{
    CCollider_Cube_AABB* pInstance = new CCollider_Cube_AABB(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCollider_Cube_AABB"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_Cube_AABB::Free()
{
    __super::Free();
}
