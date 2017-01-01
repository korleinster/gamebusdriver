#include "stdafx.h"
#include "DynamicMesh.h"

#include <assert.h>
#include "ResourcesMgr.h"
#include "AniBuffer.h"
#include "FbxParser.h"
#include "TimeMgr.h"
#include <conio.h>
#include "Device.h"
#include "Shader.h"
#include "Texture.h"



CDynamicMesh::CDynamicMesh()
	:m_wCurrentAniIndex(-1)
	, m_fAniPlayTimer(0.f)
	, m_iRepeatTime(0.f)
{

}

CDynamicMesh::~CDynamicMesh()
{

}

CDynamicMesh* CDynamicMesh::Create(const char* szFilePath, vector<string> _vecAniName)
{
	CDynamicMesh* pComponent = new CDynamicMesh();

	if (FAILED(pComponent->Initialize(szFilePath, _vecAniName)))
		::Safe_Delete(pComponent);

	return pComponent;
}

CResources * CDynamicMesh::CloneResource()
{
	CResources* pResource = new CDynamicMesh(*this);

	pResource->AddRef();

	return pResource;
}

HRESULT CDynamicMesh::Initialize(const char* _pPath, vector<string> _vecAniName)
{
	m_eDrawType = DRAW_VERTEX;
	FbxManager* pFBXManager = FbxManager::Create();
	FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIOsettings);
	FbxScene* pFBXScene = FbxScene::Create(pFBXManager, "");

	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");
	//fbx를 임포트 초기화.

	FAILED_CHECK_RETURN(
		Load_Model(_pPath, _vecAniName, pFBXManager, pIOsettings, pFBXScene, pImporter),
		E_FAIL);

	CVIBuffer::CreateRasterizerState();

	pFBXScene->Destroy();
	pImporter->Destroy();
	pIOsettings->Destroy();
	pFBXManager->Destroy();

	m_bAniEnd = false;

	return S_OK;
}

HRESULT CDynamicMesh::Load_Model(const char* _pPath, vector<string> _vecAniName, FbxManager* _pFBXManager, FbxIOSettings* _pIOsettings, FbxScene* _pFBXScene, FbxImporter* _pImporter)
{
	int iFileFormat = EOF;
	string strFullPath;
	string strExtenstion = ".FBX";

	FbxAxisSystem CurrAxisSystem;//현재 fbx씬내에서의 좌표축을 가져올 변수
	FbxAxisSystem DestAxisSystem = FbxAxisSystem::eMayaYUp;//클라내에서 좌표축을 설정.
	FbxGeometryConverter lGeomConverter(_pFBXManager);

	FbxNode*		pFbxRootNode;
	Animation*		pAni;

	for (unsigned int i = 0; i < _vecAniName.size(); ++i)
	{
		// http://www.slideshare.net/quxn6/c4316-alignment-error
		pAni = new Animation();
		pAni->strAniName = _vecAniName[i];//fbx의 이름 주입.

		strFullPath.clear();
		strFullPath = _pPath;
		strFullPath += _vecAniName[i];//경로에 파일이름 추가
		strFullPath += strExtenstion;//파일이름 추가한거에 확장자명 추가.

		_pFBXManager->GetIOPluginRegistry()->DetectReaderFileFormat(strFullPath.c_str(), iFileFormat);
		//파일이 있는지 확인.

		FAILED_CHECK_MSG(
			_pImporter->Initialize(strFullPath.c_str(), iFileFormat),
			L"No Find Fbx Path");

		_pImporter->Import(_pFBXScene);
		//파일이 있으면 fbx씬에 추가를 한다.

		CurrAxisSystem = _pFBXScene->GetGlobalSettings().GetAxisSystem();
		//fbx의 씬에서 좌표축을 가져온다.
		if (CurrAxisSystem != DestAxisSystem) DestAxisSystem.ConvertScene(_pFBXScene);
		//지정한 좌표축과 fbx씬의 좌표축과 비교하여 다르면 지정한 좌표축으로 바꾼다.

		lGeomConverter.Triangulate(_pFBXScene, true);
		//씬내에서 삼각형화 할수 있는 모든 노드를 삼각형화시킴.

		pFbxRootNode = _pFBXScene->GetRootNode();
		NULL_CHECK_RETURN(pFbxRootNode, E_FAIL);
		//루트노드를 가져오고 확인작업.

		//pAni->pBaseBoneMatrix = new XMMATRIX[BONE_MATRIX_NUM];
		pAni->pBaseBoneMatrix = (XMMATRIX*)_aligned_malloc(
			sizeof(XMMATRIX) * BONE_MATRIX_NUM, 16);// 본의 행렬을 행렬*본행렬의 사이즈만큼 해서 초기화.
		//new (pAni->pBaseBoneMatrix)XMMATRIX();

		for (int i = 0; i < BONE_MATRIX_NUM; ++i)
			pAni->pBaseBoneMatrix[i] = XMMatrixIdentity();
		//단위행렬화 시킴.

		pAni->pAniBuffer = new CAniBuffer;
		pAni->pAniBuffer->SetFbxBoneIndex(&(pAni->mapIndexByName), pFbxRootNode);
		//본들을 세팅한다. 부모-자식노드간의 연결.


		CFbxParser::ParsingVertex(pFbxRootNode, pAni);
		//버텍스와 uv , 노말을 파싱한다.


		// Animation -----------------------
		auto AnimStack = _pFBXScene->GetSrcObject<FbxAnimStack>();
		NULL_CHECK_RETURN(AnimStack, E_FAIL);

		pAni->llAniMaxTime = AnimStack->GetLocalTimeSpan().GetDuration().GetMilliSeconds();
		//fbx에서 애니매이션의 최대시간을 가져온다.
		pAni->nAniNodeIdxCnt = pAni->mapIndexByName.size();

		pAni->ppAniMatrix = new XMMATRIX*[unsigned int(pAni->llAniMaxTime / 10)];
		pAni->ppResultMatrix = new XMMATRIX*[unsigned int(pAni->llAniMaxTime / 10)];

		for (long long i = 0; i < pAni->llAniMaxTime / 10; ++i)
		{
			//pAni->ppAniMatrix[i] = new XMMATRIX[pAni->nAniNodeIdxCnt];
			//pAni->ppResultMatrix[i] = new XMMATRIX[pAni->nAniNodeIdxCnt];

			pAni->ppAniMatrix[i] = (XMMATRIX*)_aligned_malloc(
				sizeof(XMMATRIX) * pAni->nAniNodeIdxCnt, 16);
			pAni->ppResultMatrix[i] = (XMMATRIX*)_aligned_malloc(
				sizeof(XMMATRIX) * pAni->nAniNodeIdxCnt, 16);
		}

		//Animation 상수 버퍼(컨스턴트버퍼) 생성


		//최초 상수버퍼를 생성한다.
		D3D11_BUFFER_DESC BD;
		::ZeroMemory(&BD, sizeof(D3D11_BUFFER_DESC));
		BD.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		BD.ByteWidth = sizeof(VS_CB_BONE_MATRIX);
		BD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BD.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

		CDevice::GetInstance()->m_pDevice->CreateBuffer(&BD, NULL, &pAni->pBoneMatrixBuffer);


		CDevice::GetInstance()->m_pDeviceContext->Map(
			pAni->pBoneMatrixBuffer,
			NULL,
			D3D11_MAP::D3D11_MAP_WRITE_DISCARD,
			NULL,
			&pAni->tMappedResource);//맵을 하여 버퍼를 갱신.

		pAni->pBoneMatrix = (VS_CB_BONE_MATRIX *)pAni->tMappedResource.pData;

		for (int i = 0; i < BONE_MATRIX_NUM; i++)
			pAni->pBoneMatrix->m_XMmtxBone[i] = XMMatrixIdentity();

		CDevice::GetInstance()->m_pDeviceContext->Unmap(
			pAni->pBoneMatrixBuffer,
			NULL);//갱신이 끝나면 언맵을 하여 닫는다.

		CFbxParser::ParsingAnimation(pFbxRootNode, pAni, AnimStack);


		//XMMATRIX ResultMtx;
		for (long long i = 0; i < pAni->llAniMaxTime / 10; ++i)
		{
			for (unsigned int j = 0; j < pAni->nAniNodeIdxCnt; ++j)
			{
				XMMATRIX ResultMtx(pAni->pBaseBoneMatrix[j] * (XMMATRIX)(pAni->ppAniMatrix[i][j]));
				pAni->ppResultMatrix[i][j] = ResultMtx;
			}
		}

		m_vecAni.push_back(pAni);
	}

	return S_OK;

}

void CDynamicMesh::PlayAnimation(int _iIdx, CShader* pVertexShader, CShader* pPixelShader, ConstantBuffer* cb, CTexture* pTexture)
{
	if (_iIdx < 0 || (unsigned)_iIdx > m_vecAni.size())
		return;


	//m_pShader->Render();
	//

	//m_vecAni[_iIdx]->fAniPlayTimer
	m_fAniPlayTimer
		+= m_vecAni[_iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

	//if (m_vecAni[_iIdx]->fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10)
	//	m_vecAni[_iIdx]->fAniPlayTimer = 0;

	//애니메이션 스테이트 체크해서 어디서부터 반복할지 넣어준다
	/*switch (_iIdx)
	{
	case CPlayingInfo::ANI_STATE_BOOSTER:
	case CPlayingInfo::ANI_STATE_DEAD:
	case CPlayingInfo::ANI_STATE_BREAK:
		if (m_fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10) {
			m_fAniPlayTimer = (m_vecAni[_iIdx]->llAniMaxTime / 10) - 1.f;
			Animend = true;
		}
		break;
	default:
		if (m_fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10) {
			m_fAniPlayTimer = 0;
			Animend = true;
		}
		break;
	}*/

	///////////임시임////////
	if (m_fAniPlayTimer > m_vecAni[_iIdx]->llAniMaxTime / 10) {
		m_fAniPlayTimer = 0;
		m_bAniEnd = true;
	}
	////////////////////////

	for (unsigned int i = 0; i < m_vecAni[_iIdx]->nAniNodeIdxCnt; ++i)
	{
		m_vecAni[_iIdx]->pBoneMatrix->m_XMmtxBone[i]
			//= m_vecAni[_iIdx]->ppResultMatrix[int(m_vecAni[_iIdx]->fAniPlayTimer)][i];
			= m_vecAni[_iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
	}


	if (m_vecAni[_iIdx]->pBoneMatrixBuffer != NULL)
	{
		CDevice::GetInstance()->m_pDeviceContext->UpdateSubresource(m_vecAni[_iIdx]->pBoneMatrixBuffer, 0, NULL, &cb, 0, 0);
		CDevice::GetInstance()->m_pDeviceContext->VSSetShader(pVertexShader->m_pVertexShader, NULL, 0);
		CDevice::GetInstance()->m_pDeviceContext->VSSetConstantBuffers(
			VS_SLOT_BONE_MATRIX, 8, &m_vecAni[_iIdx]->pBoneMatrixBuffer);
		CDevice::GetInstance()->m_pDeviceContext->PSSetShader(pPixelShader->m_pPixelShader, NULL, 0);
		CDevice::GetInstance()->m_pDeviceContext->PSSetShaderResources(0, 1, &pTexture->m_pTextureRV);
		CDevice::GetInstance()->m_pDeviceContext->PSSetSamplers(0, 1, &pTexture->m_pSamplerLinear);

		//cout << cb->matProjection._41 << "/" << cb->matProjection._42 << "/" << cb->matProjection._43 << endl;
	}

	m_vecAni[_iIdx]->pAniBuffer->Render();
	m_wCurrentAniIndex = _iIdx;
}

void CDynamicMesh::BWPlayAnim(int _iIdx)
{
	//애니메이션 스테이트 체크해서 어디서부터 반복할지 넣어준다
	/*switch (_iIdx)
	{
	case CPlayingInfo::ANI_STATE_BOOSTER:
		m_iRepeatTime = 120;
		break;
	case CPlayingInfo::ANI_STATE_DEAD:
		m_iRepeatTime = 100;
		break;
	case CPlayingInfo::ANI_STATE_BREAK:
		m_iRepeatTime = 50;
		break;
	default:
		m_iRepeatTime = 0;
		break;
	}*/

	
	m_iRepeatTime = 0;

	if (_iIdx < 0 || (unsigned)_iIdx > m_vecAni.size())
		return;


	//m_pShader->Render();
	CDevice::GetInstance()->m_pDeviceContext->RSSetState(m_pRasterizerState);




	//반대로 돌린다아ㅏ
	m_fAniPlayTimer
		-= m_vecAni[_iIdx]->fAniPlaySpeed * CTimeMgr::GetInstance()->GetTime();

	//AllocConsole();
	//_cprintf("%lf\n", m_fAniPlayTimer);

	if (m_fAniPlayTimer <= m_iRepeatTime) {
		m_bAniEnd = false;
		//return true;
	}

	//-----------------------------------------------------

	for (unsigned int i = 0; i < m_vecAni[_iIdx]->nAniNodeIdxCnt; ++i)
	{
		m_vecAni[_iIdx]->pBoneMatrix->m_XMmtxBone[i]
			= m_vecAni[_iIdx]->ppResultMatrix[int(m_fAniPlayTimer)][i];
	}


	if (m_vecAni[_iIdx]->pBoneMatrixBuffer != NULL)
	{
		CDevice::GetInstance()->m_pDeviceContext->VSSetConstantBuffers(
			VS_SLOT_BONE_MATRIX, 1, &m_vecAni[_iIdx]->pBoneMatrixBuffer);
	}

	m_vecAni[_iIdx]->pAniBuffer->Render();
	m_wCurrentAniIndex = _iIdx;
	//return false;
}