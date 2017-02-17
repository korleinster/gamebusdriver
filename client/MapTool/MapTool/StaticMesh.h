#pragma once
#include "Mesh.h"
class CStaticMesh :
	public CMesh
{
public:
	CStaticMesh();
	virtual ~CStaticMesh();

public:
	vector<VTXTEX> vecVTXTEX;

public:
	static CStaticMesh* Create(const char* szFilePath, const char* szFileName);
	virtual CResources* CloneResource();

public:
	virtual HRESULT Load_StaticMesh(const char* szFilePath, const char* szFileName, FbxManager* _pFBXManager, FbxIOSettings* _pIOsettings, FbxScene* _pFBXScene, FbxImporter* _pImporter);
public:
	virtual HRESULT Initalize(const char* szFilePath, const char* szFileName);

};

