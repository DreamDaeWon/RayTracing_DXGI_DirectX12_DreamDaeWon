#include "VIBuffer_BattleCruiser.h"

CVIBuffer_BattleCruiser::CVIBuffer_BattleCruiser(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_BattleCruiser::CVIBuffer_BattleCruiser(const CVIBuffer_BattleCruiser& rhs) :
    CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_BattleCruiser::Initialize_Prototype()
{

    m_iNumVertices = 28;
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);
    m_dwPrimitive = 42;

    m_pVerticesPos = new _float3[m_iNumVertices];

    m_iIndexStride = m_iNumVertices >= 65535 ? 4 : 2;
    m_iNumIndices = 126;
    m_eIndexFormat = m_iIndexStride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32;

    //For. VertexBuffer

    /* 정점 여섯개의 배열 공간을 할당한다. */
    if (FAILED(__super::Create_VertexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_VertexBuffer : __super,CVIBuffer_BattleCruiser"));
        return E_FAIL;
    }

    VTXPOSTEX* pVertex = { nullptr };

    /* 할당된 정점 배열공간의 가장 앞 주소를 얻어올 수 있다. */
    /* 이 공간을 걸어 잠근다. */
    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

// 새로 바꾼 정점

        // 왼쪽 날개
    pVertex[0].vPosition = m_pVerticesPos[0] = _float3{ -0.8f, -0.25f, -0.5f };
    pVertex[0].vTexcoord = _float2{ 0.12f, 0.37f };

    pVertex[1].vPosition = m_pVerticesPos[1] = _float3{ -0.6f, 0.1f, -0.5f };
    pVertex[1].vTexcoord = _float2{ 0.18f, 0.37f };

    pVertex[2].vPosition = m_pVerticesPos[2] = _float3{ -0.4f, 0.1f, -0.5f };
    pVertex[2].vTexcoord = _float2{ 0.25f, 0.37f };

    pVertex[3].vPosition = m_pVerticesPos[3] = _float3{ -0.2f, -0.25f, -0.5f };
    pVertex[3].vTexcoord = _float2{ 0.43f, 0.44f };

    pVertex[4].vPosition = m_pVerticesPos[4] = _float3{ -0.8f, -0.25f, 0.1f };
    pVertex[4].vTexcoord = _float2{ 0.12f, 0.31f };

    pVertex[5].vPosition = m_pVerticesPos[5] = _float3{ -0.6f, 0.1f, 0.1f };
    pVertex[5].vTexcoord = _float2{ 0.18f, 0.31f };

    pVertex[6].vPosition = m_pVerticesPos[6] = _float3{ -0.4f, 0.1f, 0.1f };
    pVertex[6].vTexcoord = _float2{ 0.25f, 0.31f };

    pVertex[7].vPosition = m_pVerticesPos[7] = _float3{ -0.2f, -0.25f, 0.1f };
    pVertex[7].vTexcoord = _float2{ 0.29f, 0.31f };

    pVertex[24].vPosition = m_pVerticesPos[24] = _float3{ -0.5f, -0.25f, -0.5f };
    pVertex[24].vTexcoord = _float2{ 0.27f, 0.44f };

    pVertex[25].vPosition = m_pVerticesPos[25] = _float3{ -0.5f, -0.25f, 0.1f };
    pVertex[25].vTexcoord = _float2{ 0.23f, 0.23f };



    //오른쪽 날개
    pVertex[8].vPosition = m_pVerticesPos[8] = _float3{ 0.2f, -0.25f, -0.5f };
    pVertex[8].vTexcoord = _float2{ 0.56f, 0.44f };

    pVertex[9].vPosition = m_pVerticesPos[9] = _float3{ 0.4f, 0.1f, -0.5f };
    pVertex[9].vTexcoord = _float2{ 0.74f, 0.37f };

    pVertex[10].vPosition = m_pVerticesPos[10] = _float3{ 0.6f, 0.1f, -0.5f };
    pVertex[10].vTexcoord = _float2{ 0.81f, 0.37f };

    pVertex[11].vPosition = m_pVerticesPos[11] = _float3{ 0.8f, -0.25f, -0.5f };
    pVertex[11].vTexcoord = _float2{ 0.87f, 0.37f };

    pVertex[12].vPosition = m_pVerticesPos[12] = _float3{ 0.2f, -0.25f, 0.1f };
    pVertex[12].vTexcoord = _float2{ 0.70f, 0.31f };

    pVertex[13].vPosition = m_pVerticesPos[13] = _float3{ 0.4f, 0.1f, 0.1f };
    pVertex[13].vTexcoord = _float2{ 0.74f, 0.31f };

    pVertex[14].vPosition = m_pVerticesPos[14] = _float3{ 0.6f, 0.1f, 0.1f };
    pVertex[14].vTexcoord = _float2{ 0.81f, 0.31f };

    pVertex[15].vPosition = m_pVerticesPos[15] = _float3{ 0.8f, -0.25f, 0.1f };
    pVertex[15].vTexcoord = _float2{ 0.87f, 0.31f };

    pVertex[26].vPosition = m_pVerticesPos[26] = _float3{ 0.5f, -0.25f, -0.5f };
    pVertex[26].vTexcoord = _float2{ 0.72f, 0.44f };

    pVertex[27].vPosition = m_pVerticesPos[27] = _float3{ 0.5f, -0.25f, 0.1f };
    pVertex[27].vTexcoord = _float2{ 0.77f, 0.23f };



    // 몸체
    pVertex[16].vPosition = m_pVerticesPos[16] = _float3{ -0.2f, 0.25f, -0.5f };
    pVertex[16].vTexcoord = _float2{ 0.48f, 0.31f };

    pVertex[17].vPosition = m_pVerticesPos[17] = _float3{ 0.2f, 0.25f, -0.5f };
    pVertex[17].vTexcoord = _float2{ 0.51f, 0.31f };

    pVertex[18].vPosition = m_pVerticesPos[18] = _float3{ -0.2f, 0.25f, 0.25f };
    pVertex[18].vTexcoord = _float2{ 0.42f, 0.15f };

    pVertex[19].vPosition = m_pVerticesPos[19] = _float3{ 0.2f, 0.25f, 0.25f };
    pVertex[19].vTexcoord = _float2{ 0.57f, 0.15f };

    pVertex[20].vPosition = m_pVerticesPos[20] = _float3{ 0.2f, -0.25f, 0.25f };
    pVertex[20].vTexcoord = _float2{ 0.72f, 0.15f };

    pVertex[21].vPosition = m_pVerticesPos[21] = _float3{ -0.2f, -0.25f, 0.25f };
    pVertex[21].vTexcoord = _float2{ 0.27f, 0.15f };

    pVertex[22].vPosition = m_pVerticesPos[22] = _float3{ -0.2f, -0.25f, 1.0f };
    pVertex[22].vTexcoord = _float2{ 0.42f, 0.f };

    pVertex[23].vPosition = m_pVerticesPos[23] = _float3{ 0.2f, -0.25f, 1.0f };
    pVertex[23].vTexcoord = _float2{ 0.57f, 0.f };





    m_pVB->Unlock();

    //For. IndexBuffer
    if (FAILED(__super::Create_IndexBuffer()))
    {
        MSG_BOX(TEXT("Failed to Create_IndexBuffer : __super,CVIBuffer_BattleCruiser"));
        return E_FAIL;
    }

    _ushort* pIndices = { nullptr };
    _ushort* pIndicesInfo = new _ushort[m_iNumIndices];
    m_pIB->Lock(0, 0, (void**)&pIndices, 0);


    //왼쪽 날개=========================================

      // 뒷면
    pIndices[0] = pIndicesInfo[0] = 0;
    pIndices[1] = pIndicesInfo[1] = 1;
    pIndices[2] = pIndicesInfo[2] = 24;

    pIndices[3] = pIndicesInfo[3] = 1;
    pIndices[4] = pIndicesInfo[4] = 2;
    pIndices[5] = pIndicesInfo[5] = 24;

    pIndices[6] = pIndicesInfo[6] = 24;
    pIndices[7] = pIndicesInfo[7] = 2;
    pIndices[8] = pIndicesInfo[8] = 3;


    // 왼쪽면
    pIndices[9] = pIndicesInfo[9] = 4;
    pIndices[10] = pIndicesInfo[10] = 1;
    pIndices[11] = pIndicesInfo[11] = 0;

    pIndices[12] = pIndicesInfo[12] = 4;
    pIndices[13] = pIndicesInfo[13] = 5;
    pIndices[14] = pIndicesInfo[14] = 1;


    // 윗면
    pIndices[15] = pIndicesInfo[15] = 6;
    pIndices[16] = pIndicesInfo[16] = 1;
    pIndices[17] = pIndicesInfo[17] = 5;

    pIndices[18] = pIndicesInfo[18] = 6;
    pIndices[19] = pIndicesInfo[19] = 2;
    pIndices[20] = pIndicesInfo[20] = 1;


    // 오른쪽면
    pIndices[21] = pIndicesInfo[21] = 7;
    pIndices[22] = pIndicesInfo[22] = 2;
    pIndices[23] = pIndicesInfo[23] = 6;

    pIndices[24] = pIndicesInfo[24] = 7;
    pIndices[25] = pIndicesInfo[25] = 3;
    pIndices[26] = pIndicesInfo[26] = 2;


    // 앞면
    pIndices[27] = pIndicesInfo[27] = 5;
    pIndices[28] = pIndicesInfo[28] = 4;
    pIndices[29] = pIndicesInfo[29] = 25;

    pIndices[30] = pIndicesInfo[30] = 6;
    pIndices[31] = pIndicesInfo[31] = 5;
    pIndices[32] = pIndicesInfo[32] = 25;

    pIndices[33] = pIndicesInfo[33] = 6;
    pIndices[34] = pIndicesInfo[34] = 25;
    pIndices[35] = pIndicesInfo[35] = 7;

    // 아랫면
    pIndices[36] = pIndicesInfo[36] = 3;
    pIndices[37] = pIndicesInfo[37] = 7;
    pIndices[38] = pIndicesInfo[38] = 4;

    pIndices[39] = pIndicesInfo[39] = 3;
    pIndices[40] = pIndicesInfo[40] = 4;
    pIndices[41] = pIndicesInfo[41] = 0;

    //=============================================


    //오른쪽 날개=========================================

        // 뒷면
    pIndices[42] = pIndicesInfo[42] = 9;
    pIndices[43] = pIndicesInfo[43] = 26;
    pIndices[44] = pIndicesInfo[44] = 8;

    pIndices[45] = pIndicesInfo[45] = 9;
    pIndices[46] = pIndicesInfo[46] = 10;
    pIndices[47] = pIndicesInfo[47] = 26;

    pIndices[48] = pIndicesInfo[48] = 10;
    pIndices[49] = pIndicesInfo[49] = 11;
    pIndices[50] = pIndicesInfo[50] = 26;


    // 왼쪽면
    pIndices[51] = pIndicesInfo[51] = 13;
    pIndices[52] = pIndicesInfo[52] = 8;
    pIndices[53] = pIndicesInfo[53] = 12;

    pIndices[54] = pIndicesInfo[54] = 13;
    pIndices[55] = pIndicesInfo[55] = 9;
    pIndices[56] = pIndicesInfo[56] = 8;


    // 윗면
    pIndices[57] = pIndicesInfo[57] = 14;
    pIndices[58] = pIndicesInfo[58] = 9;
    pIndices[59] = pIndicesInfo[59] = 13;

    pIndices[60] = pIndicesInfo[60] = 14;
    pIndices[61] = pIndicesInfo[61] = 10;
    pIndices[62] = pIndicesInfo[62] = 9;

    // 오른쪽면
    pIndices[63] = pIndicesInfo[63] = 15;
    pIndices[64] = pIndicesInfo[64] = 10;
    pIndices[65] = pIndicesInfo[65] = 14;

    pIndices[66] = pIndicesInfo[66] = 15;
    pIndices[67] = pIndicesInfo[67] = 11;
    pIndices[68] = pIndicesInfo[68] = 10;

    // 앞면
    pIndices[69] = pIndicesInfo[69] = 13;
    pIndices[70] = pIndicesInfo[70] = 12;
    pIndices[71] = pIndicesInfo[71] = 27;

    pIndices[72] = pIndicesInfo[72] = 14;
    pIndices[73] = pIndicesInfo[73] = 13;
    pIndices[74] = pIndicesInfo[74] = 27;

    pIndices[75] = pIndicesInfo[75] = 15;
    pIndices[76] = pIndicesInfo[76] = 14;
    pIndices[77] = pIndicesInfo[77] = 27;

    // 아래면
    pIndices[78] = pIndicesInfo[78] = 11;
    pIndices[79] = pIndicesInfo[79] = 15;
    pIndices[80] = pIndicesInfo[80] = 12;

    pIndices[81] = pIndicesInfo[81] = 11;
    pIndices[82] = pIndicesInfo[82] = 12;
    pIndices[83] = pIndicesInfo[83] = 8;

    //===========================================================



    // 몸체==========================================================

        // 뒷면
    pIndices[84] = pIndicesInfo[84] = 16;
    pIndices[85] = pIndicesInfo[85] = 8;
    pIndices[86] = pIndicesInfo[86] = 3;

    pIndices[87] = pIndicesInfo[87] = 16;
    pIndices[88] = pIndicesInfo[88] = 17;
    pIndices[89] = pIndicesInfo[89] = 8;

    // 왼쪽면
    pIndices[90] = pIndicesInfo[90] = 18;
    pIndices[91] = pIndicesInfo[91] = 3;
    pIndices[92] = pIndicesInfo[92] = 21;

    pIndices[93] = pIndicesInfo[93] = 18;
    pIndices[94] = pIndicesInfo[94] = 16;
    pIndices[95] = pIndicesInfo[95] = 3;

    // 윗면
    pIndices[96] = pIndicesInfo[96] = 19;
    pIndices[97] = pIndicesInfo[97] = 16;
    pIndices[98] = pIndicesInfo[98] = 18;

    pIndices[99] = pIndicesInfo[99] = 19;
    pIndices[100] = pIndicesInfo[100] = 17;
    pIndices[101] = pIndicesInfo[101] = 16;

    // 오른쪽면
    pIndices[102] = pIndicesInfo[102] = 17;
    pIndices[103] = pIndicesInfo[103] = 20;
    pIndices[104] = pIndicesInfo[104] = 8;

    pIndices[105] = pIndicesInfo[105] = 17;
    pIndices[106] = pIndicesInfo[106] = 19;
    pIndices[107] = pIndicesInfo[107] = 20;

    // 아래면
    pIndices[108] = pIndicesInfo[108] = 8;
    pIndices[109] = pIndicesInfo[109] = 22;
    pIndices[110] = pIndicesInfo[110] = 3;

    pIndices[111] = pIndicesInfo[111] = 8;
    pIndices[112] = pIndicesInfo[112] = 23;
    pIndices[113] = pIndicesInfo[113] = 22;

    //========================================================


    // 몸체의 앞에 튀어나온 부리========================================
        // 왼쪽면
    pIndices[114] = pIndicesInfo[114] = 22;
    pIndices[115] = pIndicesInfo[115] = 18;
    pIndices[116] = pIndicesInfo[116] = 21;

    // 윗면
    pIndices[117] = pIndicesInfo[117] = 19;
    pIndices[118] = pIndicesInfo[118] = 22;
    pIndices[119] = pIndicesInfo[119] = 23;

    pIndices[120] = pIndicesInfo[120] = 19;
    pIndices[121] = pIndicesInfo[121] = 18;
    pIndices[122] = pIndicesInfo[122] = 22;

    // 오른쪽면
    pIndices[123] = pIndicesInfo[123] = 19;
    pIndices[124] = pIndicesInfo[124] = 23;
    pIndices[125] = pIndicesInfo[125] = 20;
    //======================================================끝!!!!!

    m_pIndicesInfo = pIndicesInfo;
    m_pIB->Unlock();

    return S_OK;
}

HRESULT CVIBuffer_BattleCruiser::Initialize(void* pArg)
{
    return S_OK;
}

CVIBuffer_BattleCruiser* CVIBuffer_BattleCruiser::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CVIBuffer_BattleCruiser* pInstance = new CVIBuffer_BattleCruiser(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_BattleCruiser"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_BattleCruiser::Clone(void* pArg)
{
    CVIBuffer_BattleCruiser* pInstance = new CVIBuffer_BattleCruiser(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CVIBuffer_BattleCruiser"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_BattleCruiser::Free()
{
    __super::Free();
}
