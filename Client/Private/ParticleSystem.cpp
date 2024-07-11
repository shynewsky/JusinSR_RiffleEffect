#include "stdafx.h"
#include "..\Public\ParticleSystem.h"

#include "GameInstance.h"
#include <random>

CParticleSystem::CParticleSystem(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& prototype)
	: CBlendObject{ prototype }
{
}

HRESULT CParticleSystem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleSystem::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_StateDesc, pArg, sizeof(STATE_DESC));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_ParticleSystem()))
		return E_FAIL;

	if (FAILED(Ready_VIBuffer()))
		return E_FAIL;

	if (m_StateDesc.eType != PARTICLE_WEAPON && m_StateDesc.eType != PARTICLE_FLAME)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_StateDesc.vPosition);
		m_pTransformCom->Scaling(m_StateDesc.vParticleScale.x, m_StateDesc.vParticleScale.y, 0);
	}
	else
		Set_OrthoLH();

	m_fViewZ = 50.f;
	m_bStart = false;

	return S_OK;
}

void CParticleSystem::Set_OrthoLH()
{

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	//m_pTransformCom->Scaling(10.f, 10.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_StateDesc.vPosition.x - g_iWinSizeX * 0.5f, -m_StateDesc.vPosition.y + g_iWinSizeY * 0.5f, m_StateDesc.vPosition.z));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);

}

void CParticleSystem::Priority_Update(_float fTimeDelta)
{
}

int CParticleSystem::Update(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	if (FAILED(Remove_DeadParticles()))
		return E_FAIL;

	//시간 누적
	m_dCurrentDuration += fTimeDelta;
	if (m_StateDesc.dDuration > m_dCurrentDuration)
	{
		switch (m_StateDesc.eType)
		{
		case Client::CParticleSystem::PARTICLE_FLARE:
			Particle_Flare_Play(fTimeDelta);
			break;
		case Client::CParticleSystem::PARTICLE_SPARK:
			Particle_Spark_Play(fTimeDelta);
			break;
		case Client::CParticleSystem::PARTICLE_TEST:
			Particle_TEST_Play(fTimeDelta);
			break;
		case Client::CParticleSystem::PARTICLE_WEAPON:
			Particle_Weapon_Play(fTimeDelta);
			break;
		case Client::CParticleSystem::PARTICLE_FLAME:
			Particle_Flame_Play(fTimeDelta);
			break;
		default:
			break;
		}
	}

	/* 혹시 모든 파티클이 죽었다면 바로 Dead 처리*/
	if (m_bStart && Check_AllParticleDead())
	{
		m_bDead = true;
		return OBJ_DEAD;
	}

	//파티클 갱신
	if (FAILED(Update_Particles(fTimeDelta)))
		return E_FAIL;

	//파티클을 버퍼로 
	if (FAILED(Update_VIBuffer()))
		return E_FAIL;


	return OBJ_NOEVENT;
}

void CParticleSystem::Late_Update(_float fTimeDelta)
{
	if (m_StateDesc.eType != PARTICLE_FLARE &&m_StateDesc.eType != PARTICLE_WEAPON && m_StateDesc.eType != PARTICLE_FLAME)
	{
		SetUp_BillBoard();
		
	}
	if (m_StateDesc.eType == PARTICLE_FLAME)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
	}
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	m_pTextureCom->Move_Frame(fTimeDelta);
	
}

HRESULT CParticleSystem::Render()
{
	if (nullptr == m_pVBuffer)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	/*if (FAILED(m_pTextureCom->Bind_Texture(m_pTextureCom->Get_Frame().iCurrentTex)))
		return E_FAIL;*/

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// 소스 블렌드 및 대상 블렌드 설정
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 알파 블렌드 기능 설정
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	// 필요한 경우 알파 테스트 비활성화
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_PrevViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_PrevProjMatrix);


	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));

	if (m_StateDesc.eType != PARTICLE_WEAPON && m_StateDesc.eType != PARTICLE_FLAME)
	{
		m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &m_PrevViewMatrix), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &m_PrevProjMatrix), sizeof(_float4x4));
	}
	else
	{
		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
		m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &m_ViewMatrix), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));
	}
	

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_StateDesc.iTextureNum));
	if(m_StateDesc.eType == PARTICLE_FLAME)
		m_pShaderCom->Begin(5);
	else
		m_pShaderCom->Begin(0);

	if (FAILED(Render_VIBuffer()))
		return E_FAIL;

	m_pShaderCom->End();


	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_PrevViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_PrevProjMatrix);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}


HRESULT CParticleSystem::Render_VIBuffer()
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pVBuffer)
		return E_FAIL;

	m_pGraphic_Device->SetStreamSource(0, m_pVBuffer, 0, m_iStride);
	m_pGraphic_Device->SetFVF(m_dwFVF);
	m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumPrimitive);

	return S_OK;
}

HRESULT CParticleSystem::Particle_Flare_Play(_float fTimeDelta)
{
	m_bStart = true;
	const _float3 buoyancy = _float3(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		m_ParticleArray[i].vVelocity = m_ParticleArray[i].vVelocity + buoyancy * fTimeDelta;
	}

	return S_OK;
}

HRESULT CParticleSystem::Particle_Spark_Play(_float fTimeDelta)
{
	m_bStart = true;
	_bool emitParticle = false;
	_bool found = false;
	float positionX, positionY, positionZ;
	int index, i, j;
	const _float3 buoyancy = _float3(0.0f, 2.0f, 0.0f);
	m_dCurrentSpawnTime += fTimeDelta;
	emitParticle = false;

	if (m_dCurrentSpawnTime > (m_StateDesc.dSpawnTime / m_StateDesc.fParticlePerSecond))
	{
		m_dCurrentSpawnTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_iCurrentParticleCount < (m_StateDesc.iMaxParticleCount - 1)))
	{
		m_iCurrentParticleCount++;

		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.vParticleDeviation.x;
		positionY = 0;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_StateDesc.vParticleDeviation.z;
		_float3 vPosition = _float3(positionX, positionY, positionZ);

		index = 0;
		found = false;
		while (!found)
		{
			if ((m_ParticleArray[index].bActive == false) || Calculate_ViewZ(m_ParticleArray[index].vPosition) < Calculate_ViewZ(vPosition))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		i = m_iCurrentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_ParticleArray[i].bActive = m_ParticleArray[j].bActive;
			m_ParticleArray[i].vPosition = m_ParticleArray[j].vPosition;
			//m_ParticleArray[i].vVelocity = m_ParticleArray[j].vVelocity;
			m_ParticleArray[i].dCurrentLifeTime = m_ParticleArray[j].dCurrentLifeTime;
			i--;
			j--;
		}

		m_ParticleArray[i].bActive = true;
		m_ParticleArray[i].vPosition = vPosition;
		m_ParticleArray[i].vVelocity =  m_ParticleArray[i].vVelocity + buoyancy * fTimeDelta;
		m_ParticleArray[i].dCurrentLifeTime = 0;
	}

	return S_OK;
}

HRESULT CParticleSystem::Particle_TEST_Play(_float fTimeDelta)
{
	m_bStart = true;
	const _float3 gravity = _float3(0.0f, -3.f, 0.0f);
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		m_ParticleArray[i].vVelocity = m_ParticleArray[i].vVelocity + gravity * fTimeDelta;
	}

	return S_OK;
}

HRESULT CParticleSystem::Particle_Weapon_Play(_float fTimeDelta)
{
	m_bStart = true;


	return S_OK;
}

HRESULT CParticleSystem::Particle_Flame_Play(_float fTimeDelta)
{
	m_bStart = true;
	const _float3 buoyancy = _float3(0.0f, 2.0f, 0.0f);
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		m_ParticleArray[i].vVelocity = m_ParticleArray[i].vVelocity + buoyancy * fTimeDelta;
	}

	return S_OK;
}

HRESULT CParticleSystem::Ready_Components(void * pArg)
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Basic"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/*CTexture::TEXTUREDESC  TextureDesc;
	TextureDesc.iEndTex = m_StateDesc.iTextureNum - 1;*/

	/* For Com_Texture */
	if (FAILED(__super::Add_Component(m_StateDesc.eTextureLevel, m_StateDesc.strTextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Transform*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleSystem::Ready_ParticleSystem()
{
	m_ParticleArray = new PARTICLE_INFO[m_StateDesc.iMaxParticleCount];
	ZeroMemory(m_ParticleArray, sizeof(PARTICLE_INFO) * m_StateDesc.iMaxParticleCount);


	if (nullptr == m_ParticleArray)
		return E_FAIL;

	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		m_ParticleArray[i].bActive = false;
	}

	m_iCurrentParticleCount = 0;
	m_dCurrentSpawnTime = 0;


	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> randomTheta(-3.141592f, 3.141592f);
	uniform_real_distribution<float> randomSpeed(3.f, 4.f);
	uniform_real_distribution<float> randomLife(0.0f, 0.15f);

	switch (m_StateDesc.eType)
	{
	case PARTICLE_TEST:

		for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
		{
			const float theta = randomTheta(gen);

			_float3 vVelocity = _float3(cos(theta), -sin(theta), 0.0) * randomSpeed(gen);
			
			_float4x4 ViewMatrix;
			_float3 vLook;
			m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
			D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

			vLook = *(_float3*)&ViewMatrix.m[2][0];
			float angleAroundY = atan2f(vLook.x, vLook.z);
			//angleAroundY *= D3DX_PI*0.5f;

			D3DXMatrixRotationZ(&ViewMatrix, angleAroundY);
			float angleInDegrees = D3DXToDegree(angleAroundY);

			D3DXVec3TransformNormal(&vVelocity, &vVelocity, &ViewMatrix);

			//m_ParticleArray[i].vVelocity = _float3(cos(theta), -sin(theta), 0.0) * randomSpeed(gen);


			m_ParticleArray[i].vVelocity = vVelocity;
			m_ParticleArray[i].vVelocity /= 5.f;
			m_ParticleArray[i].bActive = true;
			m_ParticleArray[i].dCurrentLifeTime = randomLife(gen);
		}

		break;


	case PARTICLE_WEAPON:

		for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
		{
			const float theta = randomTheta(gen);

			m_ParticleArray[i].vVelocity = (_float3(cos(theta), -sin(theta), 0.0) * randomSpeed(gen))*2.f;
			m_ParticleArray[i].bActive = true;
			m_ParticleArray[i].dCurrentLifeTime = randomLife(gen);
		}

		break;
	

	case PARTICLE_FLAME:
		for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
		{
			const float theta = randomTheta(gen);

			m_ParticleArray[i].vVelocity = (_float3(cos(theta), -sin(theta), 0.0) * randomSpeed(gen))*0.3f;
			m_ParticleArray[i].bActive = true;
			m_ParticleArray[i].dCurrentLifeTime = randomLife(gen);
		}
		break;

	case PARTICLE_FLARE:
		for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
		{
			const float theta = randomTheta(gen);

			m_ParticleArray[i].vVelocity = (_float3(cos(theta), -sin(theta), 0.0) * randomSpeed(gen)) * 0.1f;
			m_ParticleArray[i].bActive = true;
			m_ParticleArray[i].dCurrentLifeTime = randomLife(gen);
		}
		break;
	}


	return S_OK;
}

HRESULT CParticleSystem::Ready_VIBuffer()
{
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = m_StateDesc.iMaxParticleCount * 6;
	m_iNumPrimitive = m_iNumVertices / 3;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0);

	m_Vertices = new VTXTEX[m_iNumVertices];
	if (nullptr == m_Vertices)
		return E_FAIL;
	ZeroMemory(m_Vertices, sizeof(VTXTEX) * m_iNumVertices);

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iStride * m_iNumVertices, D3DUSAGE_DYNAMIC, m_dwFVF, D3DPOOL_DEFAULT, &m_pVBuffer, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleSystem::Update_Particles(_float fTimeDelta)
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		if (m_ParticleArray[i].bActive)
		{

			switch (m_StateDesc.eType)
			{
			case Client::CParticleSystem::PARTICLE_FLARE:
				m_ParticleArray[i].dCurrentLifeTime += fTimeDelta;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;

				break;
			case Client::CParticleSystem::PARTICLE_SPARK:
				m_ParticleArray[i].dCurrentLifeTime += fTimeDelta;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;

				if (m_ParticleArray[i].dCurrentLifeTime == m_StateDesc.dParticleLifeTime*0.5)
					m_ParticleArray[i].vVelocity *= -1;

				break;
			case Client::CParticleSystem::PARTICLE_TEST:
				m_ParticleArray[i].dCurrentLifeTime += fTimeDelta;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;
				break;

			case Client::CParticleSystem::PARTICLE_WEAPON:
				m_ParticleArray[i].dCurrentLifeTime += fTimeDelta;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;
				break;

			case Client::CParticleSystem::PARTICLE_FLAME:
				m_ParticleArray[i].dCurrentLifeTime += fTimeDelta;
				m_ParticleArray[i].vPosition += m_ParticleArray[i].vVelocity;
				break;

			case Client::CParticleSystem::PARTICLE_END:
				break;


			default:
				break;
			}
		}
	}
	return S_OK;
}

HRESULT CParticleSystem::Update_VIBuffer()
{
	ZeroMemory(m_Vertices, sizeof(VTXTEX) * m_iNumVertices);

	VTXTEX* pVertices = nullptr;
	int index = 0;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	_float4x4 ViewMatrix, InvViewMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&InvViewMatrix, nullptr, &ViewMatrix);

	if (m_StateDesc.eType == PARTICLE_WEAPON || m_StateDesc.eType == PARTICLE_FLAME)
		D3DXMatrixIdentity(&InvViewMatrix);

	// 카메라 방향 벡터 (z축)
	_float3 CameraRight = _float3(InvViewMatrix._11, InvViewMatrix._21, InvViewMatrix._31); // 오른쪽 벡터
	_float3 CameraUp = _float3(InvViewMatrix._12, InvViewMatrix._22, InvViewMatrix._32); // 위쪽 벡터

	if (m_StateDesc.eType== PARTICLE_FLAME)
	{
		
		m_StateDesc.vParticleScale.x *= -1.f;
	}

	for (_int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		// 파티클 위치
		_float3 ParticlePos = m_ParticleArray[i].vPosition;

		// 각 정점 위치 계산
		_float3 vRight = CameraRight * m_StateDesc.vParticleScale.x * 0.5f;
		_float3 vUp = CameraUp * m_StateDesc.vParticleScale.y * 0.5f;

		// 우상단 삼각형
		pVertices[index].vPosition = ParticlePos - vRight + vUp;
		pVertices[index].vTexcoord = _float2(0.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		pVertices[index].vPosition = ParticlePos + vRight + vUp;
		pVertices[index].vTexcoord = _float2(1.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		pVertices[index].vPosition = ParticlePos + vRight - vUp;
		pVertices[index].vTexcoord = _float2(1.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;

		// 좌하단 삼각형
		pVertices[index].vPosition = ParticlePos - vRight + vUp;
		pVertices[index].vTexcoord = _float2(0.f, 0.f);
		m_Vertices[index] = pVertices[index];
		++index;

		pVertices[index].vPosition = ParticlePos + vRight - vUp;
		pVertices[index].vTexcoord = _float2(1.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;

		pVertices[index].vPosition = ParticlePos - vRight - vUp;
		pVertices[index].vTexcoord = _float2(0.f, 1.f);
		m_Vertices[index] = pVertices[index];
		++index;
	}

	m_pVBuffer->Unlock();

	return S_OK;
}


HRESULT CParticleSystem::Remove_DeadParticles()
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		if ((true == m_ParticleArray[i].bActive) && (m_StateDesc.dParticleLifeTime <= m_ParticleArray[i].dCurrentLifeTime))
		{
			m_ParticleArray[i].bActive = false; 
			--m_iCurrentParticleCount; 

			for (int j = i; j < m_StateDesc.iMaxParticleCount - 1; ++j) 
			{
				m_ParticleArray[j].bActive = m_ParticleArray[j + 1].bActive;
				m_ParticleArray[j].vPosition = m_ParticleArray[j + 1].vPosition;
				m_ParticleArray[j].vVelocity = m_ParticleArray[j + 1].vVelocity;
				m_ParticleArray[j].dCurrentLifeTime = m_ParticleArray[j + 1].dCurrentLifeTime;
			}
		}
	}

	return S_OK;
}

_bool CParticleSystem::Check_AllParticleDead()
{
	for (int i = 0; i < m_StateDesc.iMaxParticleCount; ++i)
	{
		if (m_ParticleArray[i].bActive)
			return false;
	}
	return true;
}

_float CParticleSystem::Calculate_ViewZ(_float3 WorldPos)
{
	/*카메라의 월드 매트릭스의 좌표 받아오기*/

	_float4x4 ViewInvMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewInvMatrix);
	D3DXMatrixInverse(&ViewInvMatrix, nullptr, &ViewInvMatrix);
	_float3 vCamPosition = *(_float3*)&ViewInvMatrix.m[3][0];
	_float3	 vDistance = vCamPosition - WorldPos;
	return D3DXVec3Length(&vDistance);
}

void CParticleSystem::SetUp_BillBoard()
{
	_float4x4 ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);  
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);      

	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_StateDesc.vParticleScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_StateDesc.vParticleScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CParticleSystem * CParticleSystem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CParticleSystem*	pInstance = new CParticleSystem(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticleSystem"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CParticleSystem::Clone(void * pArg)
{
	CParticleSystem*	pInstance = new CParticleSystem(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticleSystem"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticleSystem::Free()
{
	__super::Free();

	Safe_Delete_Array(m_ParticleArray);
	Safe_Delete_Array(m_Vertices);

	Safe_Release(m_pVBuffer);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

}
