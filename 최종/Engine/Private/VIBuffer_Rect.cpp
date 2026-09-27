#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& rhs) :
    CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{

    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
    m_dwPrimitive = 2;

    m_pVerticesPos = new _float3[m_iNumVertices];

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

    pVertex[0].vPosition = m_pVerticesPos[0] = _float3{ -0.5f, 0.5f, 0.f };
    pVertex[0].vTexcoord = _float2{ 0.f, 0.f };

    pVertex[1].vPosition = m_pVerticesPos[1] = _float3{ 0.5f, 0.5f, 0.f };
    pVertex[1].vTexcoord = _float2{ 1.f, 0.f };

    pVertex[2].vPosition = m_pVerticesPos[2] = _float3{ 0.5f, -0.5f, 0.f };
    pVertex[2].vTexcoord = _float2{ 1.f, 1.f };

    pVertex[3].vPosition = m_pVerticesPos[3] = _float3{ -0.5f, -0.5f,0.f };
    pVertex[3].vTexcoord = _float2{ 0.f, 1.f };

    m_pVB->Unlock();

    //For. IndexBuffer
    if (FAILED(__super::Create_IndexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_IndexBuffer : __super,CVIBuffer_Rect"));
        return E_FAIL;
    }

    _ushort* pIndices = { nullptr };
    _ushort* pIndicesInfo = new _ushort[m_iNumIndices];
    m_pIB->Lock(0, 0, (void**)&pIndices, 0);

    pIndices[0] = pIndicesInfo[0] = 0;
    pIndices[1] = pIndicesInfo[1] = 1;
    pIndices[2] = pIndicesInfo[2] = 2;

    pIndices[3] = pIndicesInfo[3] = 0;
    pIndices[4] = pIndicesInfo[4] = 2;
    pIndices[5] = pIndicesInfo[5] = 3;

    m_pIndicesInfo = pIndicesInfo;
    m_pIB->Unlock();

    return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
    return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Rect::Free()
{
    __super::Free();
}
