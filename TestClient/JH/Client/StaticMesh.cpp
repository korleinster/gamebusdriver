#include "stdafx.h"
#include "StaticMesh.h"
#include <assert.h>
#include "Shader.h"
#include "Device.h"


CStaticMesh::CStaticMesh()
{

}

CStaticMesh::~CStaticMesh()
{

}

CStaticMesh * CStaticMesh::Create(const char* szFilePath)
{
	CStaticMesh* pStaticMesh = new CStaticMesh;

	if (FAILED(pStaticMesh->Initalize(szFilePath)))
	{
		::Safe_Delete(pStaticMesh);
	}

	return pStaticMesh;
}

CResources * CStaticMesh::CloneResource(void)
{
	CResources* pResource = new CStaticMesh(*this);

	pResource->AddRef();

	return pResource;
}
HRESULT CStaticMesh::Initalize(const char* szFilePath)
{
	FbxManager* pFBXManager = FbxManager::Create();
	FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIOsettings);
	FbxScene* pFBXScene = FbxScene::Create(pFBXManager, "");

	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");


	FAILED_CHECK_RETURN(
		Load_StaticMesh(szFilePath, pFBXManager, pIOsettings, pFBXScene, pImporter),
		E_FAIL);

	CMesh::CreateRasterizerState();
	//Init_Shader();

	pFBXScene->Destroy();
	pImporter->Destroy();
	pIOsettings->Destroy();
	pFBXManager->Destroy();


	return S_OK;
}

HRESULT CStaticMesh::Load_StaticMesh(const char* szFilePath,
	FbxManager* _pFBXManager,
	FbxIOSettings* _pIOsettings,
	FbxScene* _pFBXScene,
	FbxImporter* _pImporter)
{
	HRESULT hr = E_FAIL;

	vector<UINT> vecIndeces;

	if (!(_pImporter->Initialize(szFilePath, -1, _pFBXManager->GetIOSettings())))
		FAILED_CHECK_MSG(E_FAIL, L"Static Mesh Init Failed");
	if (!(_pImporter->Import(_pFBXScene)))
		FAILED_CHECK_MSG(E_FAIL, L"Static Mesh Import Failed");

	FbxGeometryConverter clsConverter(_pFBXManager);
	clsConverter.Triangulate(_pFBXScene, false);
	FbxNode* pRootNode = _pFBXScene->GetRootNode();

	if (!pRootNode)
		return E_FAIL;

	vector<VTXTEX> vecVTXTEX;

	for (int i = 0; i < pRootNode->GetChildCount(); ++i)
	{
		FbxNode* pChildNode = pRootNode->GetChild(i);

		if(pChildNode->GetNodeAttribute() == NULL)
			continue;

		FbxNodeAttribute::EType AttributeType = pChildNode->GetNodeAttribute()->GetAttributeType();

		if(AttributeType != FbxNodeAttribute::eMesh)
			continue;

		FbxMesh* pMesh = (FbxMesh*)pChildNode->GetNodeAttribute();
		D3DXVECTOR3 vPos;
		D3DXVECTOR2 vOutUV;
		D3DXVECTOR3 vOutNormal;
		FbxVector4* mControlPoints = pMesh->GetControlPoints();
		int iVTXCounter = 0;
		


		for (int j = 0; j < pMesh->GetPolygonCount(); j++)
		{
			int iNumVertices = pMesh->GetPolygonSize(j);
			assert(iNumVertices == 3);
			FbxGeometryElementUV* VtxUV = pMesh->GetElementUV(0);
			FbxGeometryElementNormal* VtxNormal = pMesh->GetElementNormal(0);

			

			for (int k = 0; k < iNumVertices; k++)
			{
				//정점 데이터 얻는곳
				int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
				int iTextureUVIndex = pMesh->GetTextureUVIndex(j, k);
				++iVTXCounter;

				vPos.x = (float)mControlPoints[iControlPointIndex].mData[0];
				vPos.y = (float)mControlPoints[iControlPointIndex].mData[1];
				vPos.z = (float)mControlPoints[iControlPointIndex].mData[2];

				//uv 얻기
				switch (VtxUV->GetMappingMode())
				{
				case FbxGeometryElement::eByControlPoint:

					switch (VtxUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						vOutUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(iControlPointIndex).mData[0]);
						vOutUV.y = 1.f - static_cast<float>(VtxUV->GetDirectArray().GetAt(iControlPointIndex).mData[1]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int index = VtxUV->GetIndexArray().GetAt(iControlPointIndex);
						vOutUV.x = static_cast<float>(VtxUV->GetDirectArray().GetAt(index).mData[0]);
						vOutUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray().GetAt(index).mData[1]);
					}
					break;

					default:
						throw std::exception("Invalid Reference");
					}
					break;
				case FbxGeometryElement::eByPolygonVertex:
					switch (VtxUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{

					}
					case FbxGeometryElement::eIndexToDirect:
					{
						
						vOutUV.x = static_cast<float>(VtxUV->GetDirectArray()
							.GetAt(iTextureUVIndex).mData[0]);
						vOutUV.y = 1 - static_cast<float>(VtxUV->GetDirectArray()
							.GetAt(iTextureUVIndex).mData[1]);
					}
						break;
					default:
						throw std::exception("invalid Reference");
					}
					break;
				default:
					throw std::exception("Invalid Reference");
					break;
				}
				VTXTEX vtxtex;
				vtxtex.vPos = vPos;
				vtxtex.vNormal = vOutNormal;
				vtxtex.vTexUV = vOutUV;
				vecVTXTEX.push_back(vtxtex);


				//int index = VtxUV->GetIndexArray().GetAt(iTextureUVIndex);
				vecIndeces.push_back(VtxUV->GetIndexArray().GetAt(iTextureUVIndex));

	


			}
		}
	}


	unsigned int n = vecVTXTEX.size();
	VTXTEX* pVTXTex = new VTXTEX[n];
	for (unsigned int i = 0; i < vecVTXTEX.size(); ++i)
	{
		pVTXTex[i].vPos = vecVTXTEX[i].vPos;
		pVTXTex[i].vNormal = vecVTXTEX[i].vNormal;
		pVTXTex[i].vTexUV - vecVTXTEX[i].vTexUV;
	}

	m_iVertices = vecVTXTEX.size();
	m_iVertexStrides = sizeof(VTXTEX);
	m_iVertexOffsets = 0;

	MakeVertexNormal((BYTE*)pVTXTex, NULL);

	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.ByteWidth = m_iVertexStrides * m_iVertices;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData;
	ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
	tData.pSysMem = pVTXTex;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&tBufferDesc, &tData, &m_VertexBuffer);

	if (FAILED(hr))
		return E_FAIL;


	m_iIndex = 3 * vecIndeces.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * vecIndeces.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &vecIndeces[0];
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&ibd, &iinitData, &m_IndexBuffer);

	if (FAILED(hr))
		return E_FAIL;


	D3D11_BUFFER_DESC cbd;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;
	hr = CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"System Message", L"Constant Buffer Error", MB_OK);
		return hr;
	}


	return S_OK;
}

void CStaticMesh::Init_Shader()
{
	// Texture Shader ---------
	/*m_pShader = dynamic_cast<CTextureShader*>(
		CResourceMgr::GetInstance()->CloneResource(
			CResourceMgr::RESOURCE_ATTRI_STATIC, CResourceMgr::RESOURCE_TYPE_SHADER, L"Shader_Texture"));
	NULL_CHECK(m_pShader);*/
}