#pragma once

#ifndef StaticMesh_h__
#define StaticMesh_h__

#include "Mesh.h"

class CStaticMesh : public CMesh
{
public:
	CStaticMesh* Create(const char* szFilePath);
	virtual CResources* CloneResource();

public:
	virtual HRESULT Load_StaticMesh(const char*,
		FbxManager*, FbxIOSettings*, FbxScene*, FbxImporter*);
	virtual void		Init_Shader();
public:
	virtual HRESULT Initalize(const char* szFilePath);

public:
	CStaticMesh();
	virtual ~CStaticMesh();

};
#endif // StaticMesh_h__