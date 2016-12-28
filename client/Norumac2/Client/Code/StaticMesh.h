#pragma once
#include "Mesh.h"
class CStaticMesh :
	public CMesh
{
public:
	CStaticMesh();
	virtual ~CStaticMesh();

public:
	static CStaticMesh* Create(const char* szFilePath);
	virtual CResources* CloneResource();

public:
	virtual HRESULT Load_StaticMesh(const char* szFilePath, FbxManager* _pFBXManager, FbxIOSettings* _pIOsettings, FbxScene* _pFBXScene, FbxImporter* _pImporter);
public:
	virtual HRESULT Initalize(const char* szFilePath);

};

