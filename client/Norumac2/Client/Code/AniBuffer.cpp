#include "stdafx.h"
#include "AniBuffer.h"
#include "Include.h"
#include "VIBuffer.h"
#include "Device.h"


CAniBuffer::CAniBuffer()
	:m_nOffset(0)
	, m_nStride(sizeof(VertexAni))
	, m_nStartIndex(0)
	, m_nBaseVertex(0)
	, m_nVertices(0)
{
	this->m_VertexBuffer = nullptr;
}

CAniBuffer::~CAniBuffer()
{
	for (auto iter : this->m_vecChildBuffer)
		delete iter;

	if (this->m_VertexBuffer != nullptr)
	{
		this->m_VertexBuffer->Release();
		this->m_VertexBuffer = nullptr;
	}
}

VertexAni* CAniBuffer::GetVertex(unsigned int _nIndex)
{
	if (this->m_nVertices <= _nIndex)
	{
		MSG_BOX(L"WARNNING! Out Of Range");
		return nullptr;
	}
	else
		return &m_pVertex[_nIndex];
}

void CAniBuffer::SetVertexSize(unsigned int _iSize)
{
	this->m_nVertices = _iSize;
	this->m_pVertex = new VertexAni[_iSize];
}

CAniBuffer* CAniBuffer::GetChild(unsigned int _nindex)
{
	return m_vecChildBuffer[_nindex];
}

void CAniBuffer::AddChild(CAniBuffer* _pChildBuffer)
{
	this->m_vecChildBuffer.push_back(_pChildBuffer);
}

void CAniBuffer::CreateBuffer()
{
	if (this->m_nVertices != 0)
	{
		unsigned int ByteWidth
			= ((unsigned int)((this->m_nStride * this->m_nVertices) / 16) * 16) + 16;

		D3D11_BUFFER_DESC BD;
		ZeroMemory(&BD, sizeof(D3D11_BUFFER_DESC));
		BD.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		BD.ByteWidth = ByteWidth;
		BD.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		BD.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA SD;
		ZeroMemory(&SD, sizeof(D3D11_SUBRESOURCE_DATA));
		SD.pSysMem = this->m_pVertex;

		if (FAILED(CDevice::GetInstance()->m_pDevice->CreateBuffer(&BD, &SD, &this->m_VertexBuffer)))
		{
			MSG_BOX(L"Vertex Buffer Create Failed!");
			getchar();
			exit(0);
		}
	}
}

void CAniBuffer::SetBuffer()
{
	if (this->m_VertexBuffer)
	{
		CDevice::GetInstance()->m_pDeviceContext->IASetVertexBuffers(
			0,
			1,
			&this->m_VertexBuffer,
			&this->m_nStride,
			&this->m_nOffset);
	}
}

void CAniBuffer::Render()
{
	if (this->m_nVertices != 0)
	{
		this->SetBuffer();
		CDevice::GetInstance()->m_pDeviceContext->RSSetState(m_pRasterizerState);
		CDevice::GetInstance()->m_pDeviceContext->Draw(this->m_nVertices, this->m_nStartIndex);
	}

	for (auto iter : this->m_vecChildBuffer)
		iter->Render();
}

void CAniBuffer::SetFbxBoneIndex(map<std::string, unsigned int>* _pIndexByName,
	FbxNode* _pNode)
{
	(*_pIndexByName)[_pNode->GetName()] = _pIndexByName->size();

	for (int i = 0; i < _pNode->GetChildCount(); ++i)
		this->SetFbxBoneIndex(_pIndexByName, _pNode->GetChild(i));
}

CResources* CAniBuffer::CloneResource()
{
	CResources* pResource = new CAniBuffer(*this);

	pResource->AddRef();

	return pResource;
}