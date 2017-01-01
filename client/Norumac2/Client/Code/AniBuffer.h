#pragma once
#include "Mesh.h"


class CAniBuffer
	: public CMesh
{
public:
	long long m_llAniTime;
	string m_strName;
	vector<CAniBuffer*> m_vecChildBuffer;

public:
	VertexAni* m_pVertex;
	unsigned int m_nStartIndex;
	unsigned int m_nBaseVertex;

	unsigned int m_nVertices;
	unsigned int m_nStride;
	unsigned int m_nOffset;

public:
	VertexAni*	GetVertex(unsigned int _nIndex);
	void		SetVertexSize(unsigned int _iSize);

	CAniBuffer* GetChild(unsigned int _nindex);
	void		AddChild(CAniBuffer* _pChildBuffer);

	void		CreateBuffer();
	void		SetBuffer();

	void		Render();
	void		SetFbxBoneIndex(map<std::string, unsigned int>* _pIndexByName, FbxNode* _pNode);
	virtual CResources* CloneResource();

public:
	CAniBuffer();
	virtual ~CAniBuffer();


};