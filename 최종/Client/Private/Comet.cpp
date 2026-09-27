#include "pch.h"
#include "Comet.h"
#include "GameInstance.h"

#include "TrailRenderer.h"
#include "Camera_Player_DW2.h"

CComet::CComet(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CLandObject(pGraphic_Device)
{
}

CComet::CComet(const CComet& rhs) :
	CLandObject(rhs)
{
}

HRESULT CComet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CBullet"));
		return E_FAIL;
	}


	if (FAILED(Add_Components()))
	{
		MSG_BOX(TEXT("Failed to Add_Components ,CBullet"));
		return E_FAIL;
	}


	if (nullptr != pArg)
	{
		COMET_DESC* pCometDesc = (COMET_DESC*)pArg;
		m_pTransform->Set_Scale(_float3(pCometDesc->fScale, pCometDesc->fScale, 1.f));
		m_pTransform->Set_State(CTransform::STATE_POSITION, pCometDesc->vPos);
		m_vPrePosition = pCometDesc->vPos;
	}

	m_vecTrailRenderer.reserve(COMET_TRAILMAXNUM);
	for (size_t i = 0; i < COMET_TRAILMAXNUM; i++)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_TrailRenderer"), TEXT("Prototype_GameObject_TrailRenderer"))))
		{
			MSG_BOX(TEXT("Failed to Add_Components ,CBullet"));
			return E_FAIL;
		}
		list<CGameObject*>* pList = m_pGameInstance->Get_List(LEVEL_STATIC, TEXT("Layer_TrailRenderer"));
		if (nullptr != pList)
		{
			auto iter = pList->end();
			--iter;

			CTrailRenderer* pTrailRenderer = dynamic_cast<CTrailRenderer*>((*iter));
			m_vecTrailRenderer.emplace_back(pTrailRenderer);
			Safe_AddRef(pTrailRenderer);
		}
	}

	m_pCameraPlayerDW2 = dynamic_cast<CCamera_Player_DW2*> (m_pGameInstance->Get_Object(LEVEL_1945, TEXT("Layer_Camera")));
	Safe_AddRef(m_pCameraPlayerDW2);
	m_pCameraTransform = dynamic_cast<CTransform*>(m_pCameraPlayerDW2->Get_Component(g_strTransformTag));
	Safe_AddRef(m_pCameraTransform);
	m_pTransform->Turn(_float3(0.f, 1.f, 0.f), 2);
	//m_pTransform->Set_Scale(_float3(0.2f, 0.2f, 1.f));

	return S_OK;
}

_uint CComet::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJECT_NOTHING;

	//m_pTransform->Go_Straight(fTimeDelta); //움직인다.
	Move(fTimeDelta);
	m_fTrailTerm += fTimeDelta;
	if (m_fTrailTerm > COMET_TRAILTERM)
	{
		m_vPrePosition = m_pTransform->Get_State(CTransform::STATE_POSITION);
		m_fTrailTerm = 0.f;
	}
		
	if (true == m_pCameraPlayerDW2->Get_CanShotBullet()&& m_pCameraPlayerDW2->Get_Camera_Mode() == CCamera_Player_DW2::CAMERA_PLAYER_RIGHT_VIEW)//탑뷰일때는 그냥 나오고, 라이트뷰일때는 꼬리가 나오게
	{
		Set_Trail();
	}
	
	SetUp_BillBoard();
	return OBJECT_NOTHING;
}

void CComet::Late_Tick(_float fTimeDelta)
{
	Restart();
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CComet::Render()
{

	if (FAILED(m_pTransform->Bind_WorldMatrix()))
	{
		MSG_BOX(TEXT("Failed to Bind_WorldMatrix : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Texture(0, 0)))
	{
		MSG_BOX(TEXT("Failed to Bind_Texture : Render"));
		return E_FAIL;
	}
	if (FAILED(Set_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Set_RenderState : Render"));
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer_Com->Render()))
	{
		MSG_BOX(TEXT("Failed to Render : Render"));
		return E_FAIL;
	}
	if (FAILED(Reset_RenderState()))
	{
		MSG_BOX(TEXT("Failed to Reset_RenderState : Render"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CComet::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBuffer_Com))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_VIBuffer_Rect : Add_Components"));
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TrailRenderer"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		MSG_BOX(TEXT("Failed to Prototype_Component_Texture_TrailRenderer : CComet::Add_Components"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CComet::Set_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	return S_OK;
}

HRESULT CComet::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

void CComet::SetUp_BillBoard()
{
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_TUTORIAL, TEXT("Layer_Camera_Free"), g_strTransformTag));
	//CTransform* pCameraTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(g_eLevel, TEXT("Layer_Camera_Player"), g_strTransformTag));
	

	if (nullptr == m_pCameraTransform)
		return;

	const _float4x4 pCameraWorldMatrix = *m_pCameraTransform->Get_WorldMatrix();


	_float3 vScale = m_pTransform->Get_Scale();

	m_pTransform->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]) * vScale.x);
	m_pTransform->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_UP][0]) * vScale.y);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *D3DXVec3Normalize((_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0], (_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]) * vScale.z);

	/*m_pTransform->Set_State(CTransform::STATE_RIGHT, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_RIGHT][0]);
	m_pTransform->Set_State(CTransform::STATE_LOOK, *(_float3*)&pCameraWorldMatrix.m[CTransform::STATE_LOOK][0]);*/

}

void CComet::Set_Trail()
{
	_float3 vPos = m_pTransform->Get_State(CTransform::STATE_POSITION); //현재틱에서 움직인 위치 가져오고
	_float3 vDir = vPos - m_vPrePosition; //이전틱과 현재틱 위치의 방향벡터 구하고
	_float fLength = D3DXVec3Length(&vDir); //방향벡터의 길이 구하고
	D3DXVec3Normalize(&vDir, &vDir); //방향벡터 노말라이즈 해주고
	vPos = (vPos + m_vPrePosition) * 0.5f; // 이전틱과 현재틱의 중간 위치 구해주고.
	m_vecTrailRenderer[m_iTrailCursor]->Set_Pos_XScale_Rotation(vPos, fLength * 2.f, vDir);//구한 값들로 트레일 세팅
	++m_iTrailCursor;
	if (COMET_TRAILMAXNUM <= m_iTrailCursor)
		m_iTrailCursor = 0;
	
}

void CComet::Move(_float fTimeDelta)
{
	_float3 vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	vPos.z -= 20.f * fTimeDelta;
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

void CComet::Restart()
{
	_float3 vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);

	if (COMET_END_Z > vPos.z)
	{
		vPos.z = COMET_START_Z;
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
		m_vPrePosition = vPos;
	}
}

CComet* CComet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CComet* pInstance = new CComet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CComet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CComet::Clone(void* pArg)
{
	CComet* pInstance = new CComet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CComet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComet::Free()
{
	for (auto iter : m_vecTrailRenderer)
		Safe_Release(iter);
	m_vecTrailRenderer.clear();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBuffer_Com);

	Safe_Release(m_pCameraPlayerDW2);
	Safe_Release(m_pCameraTransform);
	__super::Free();
}
