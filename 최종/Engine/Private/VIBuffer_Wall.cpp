#include "VIBuffer_Wall.h"

CVIBuffer_Wall::CVIBuffer_Wall(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_Wall::CVIBuffer_Wall(const CVIBuffer_Wall& rhs)
    : CVIBuffer(rhs),
    m_iVertexNumX(rhs.m_iVertexNumX),
    m_iVertexNumY(rhs.m_iVertexNumY)
{
}

HRESULT CVIBuffer_Wall::Initialize_Prototype(_uint iVertexNumX, _uint iVertexNumY)
{
    m_iVertexNumX = iVertexNumX;
    m_iVertexNumY = iVertexNumY;

    m_pVerticesPos = new _float3[iVertexNumX * iVertexNumY];

    _uint iMaxTileNum = (iVertexNumX - 1) * (iVertexNumY - 1);
    m_iNumVertices = iVertexNumX * iVertexNumY;
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
    m_dwPrimitive = 2 * iMaxTileNum;


    m_iIndexStride = m_iNumVertices >= 65535 ? 4 : 2;
    m_iNumIndices = 6 * iMaxTileNum;
    m_eIndexFormat = m_iIndexStride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;

    //For. VertexBuffer

    /* 정점 여섯개의 배열 공간을 할당한다. */
    if (FAILED(__super::Create_VertexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_VertexBuffer : __super,CVIBuffer_Terrain"));
        return E_FAIL;
    }

    VTXPOSTEX* pVertex = { nullptr };

    /* 할당된 정점 배열공간의 가장 앞 주소를 얻어올 수 있다. */
    /* 이 공간을 걸어 잠근다. */
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    for (size_t j = 0; j < iVertexNumY; j++)
    {
        for (size_t i = 0; i < iVertexNumX; i++)
        {
            pVertex[i + j * iVertexNumX].vPosition = m_pVerticesPos[i + j * iVertexNumX] = _float3{ 0.f, (_float)i, (_float)j };
            pVertex[i + j * iVertexNumX].vTexcoord = _float2{ (_float)i, (_float)j };
        }
    }

    m_pVB->Unlock();

    //For. IndexBuffer
    if (FAILED(__super::Create_IndexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_IndexBuffer : __super,CVIBuffer_Terrain"));
        return E_FAIL;
    }
    _uint iIndex = 0;
    if (m_iIndexStride == 2)
    {
        _ushort* pIndices = { nullptr };
        _ushort* pIndicesInfo = new _ushort[m_iNumIndices];
        m_pIB->Lock(0, 0, (void**)&pIndices, 0);
        // 3 3 사각형 몇개? 4개 그러면 몇번? 4번 돌아야함.
        for (_ushort j = 0; j < iVertexNumY - 1; j++)
        {
            for (_ushort i = 0; i < iVertexNumX - 1; i++)
            {
                pIndices[iIndex] = pIndicesInfo[iIndex] = i + j * m_iVertexNumX + m_iVertexNumX;
                pIndices[iIndex + 1] = pIndicesInfo[iIndex + 1] = i + j * m_iVertexNumX + m_iVertexNumX + 1;
                pIndices[iIndex + 2] = pIndicesInfo[iIndex + 2] = i + j * m_iVertexNumX + 1;

                pIndices[iIndex + 3] = pIndicesInfo[iIndex + 3] = i + j * m_iVertexNumX + m_iVertexNumX;
                pIndices[iIndex + 4] = pIndicesInfo[iIndex + 4] = i + j * m_iVertexNumX + 1;
                pIndices[iIndex + 5] = pIndicesInfo[iIndex + 5] = i + j * m_iVertexNumX;

                iIndex += 6;                //5 0      11 1    17
            }
        }
        m_pIndicesInfo = pIndicesInfo;
        m_pIB->Unlock();
    }
    else if (m_iIndexStride == 4)
    {
        _uint* pIndices = { nullptr };
        _uint* pIndicesInfo = new _uint[m_iNumIndices];
        m_pIB->Lock(0, 0, (void**)&pIndices, 0);

        for (_uint j = 0; j < iVertexNumY; j++)
        {
            for (_uint i = 0; i < iVertexNumX; i++)
            {                                                                       //i = 0  i = 1
                pIndices[iIndex] = pIndicesInfo[iIndex] = i + j * m_iVertexNumX + m_iVertexNumX;
                pIndices[iIndex + 1] = pIndicesInfo[iIndex + 1] = i + j * m_iVertexNumX + m_iVertexNumX + 1;
                pIndices[iIndex + 2] = pIndicesInfo[iIndex + 2] = i + j * m_iVertexNumX + 1;

                pIndices[iIndex + 3] = pIndicesInfo[iIndex + 3] = i + j * m_iVertexNumX + m_iVertexNumX;
                pIndices[iIndex + 4] = pIndicesInfo[iIndex + 4] = i + j * m_iVertexNumX + 1;
                pIndices[iIndex + 5] = pIndicesInfo[iIndex + 5] = i + j * m_iVertexNumX;

                iIndex += 6;                //5 0      11 1    17
            }
        }
        m_pIndicesInfo = pIndicesInfo;
        m_pIB->Unlock();
    }

    return S_OK;
}

HRESULT CVIBuffer_Wall::Initialize(void* pArg)
{
    return S_OK;
}

void CVIBuffer_Wall::Get_X_Z_Num(_uint* iXNum, _uint* iZNum)
{
}

CVIBuffer_Wall* CVIBuffer_Wall::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iVertexNumX, _uint iVertexNumZ)
{
    CVIBuffer_Wall* pInstance = new CVIBuffer_Wall(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype(iVertexNumX, iVertexNumZ)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Wall"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Wall::Clone(void* pArg)
{
    CVIBuffer_Wall* pInstance = new CVIBuffer_Wall(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_Wall"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Wall::Free()
{
    __super::Free();
}
