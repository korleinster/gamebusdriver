#pragma once

#include "DynamicMesh.h"

class CFbxParser
{
public:
	static void	ParsingVertex(FbxNode* _pNode, Animation* _pAnimation);
	static void	ParsingAnimation(FbxNode* _pNode, Animation* _pAnimation, FbxAnimStack* _pAniStatck);
};
