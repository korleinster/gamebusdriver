#pragma once
#include "Mesh.h"

struct VS_CB_BONE_MATRIX
{
	XMMATRIX m_XMmtxBone[BONE_MATRIX_NUM];// 본의 행렬. 128개가 넘어가면 vtf문제가 생김
};

/*
//애니메이션 관련 변수들 입니다.
float g_fAnimationPlayTime = 0.0f;				//애니메이션 재생 시간
XMMATRIX **g_pAnimationMatrix = nullptr;		//애니메이션 Matrix[시간별][인덱스별]
XMMATRIX * g_pBaseBoneMatrix = nullptr;			//애니메이션 BaseMatrix[인덱스별]
XMMATRIX ** g_pResultMatrix = nullptr;			//애니메이션 최종 변환 Matrix[시간별][인덱스별]
long long g_llAnimationMaxTime = 0;				//애니메이션 최대길이
unsigned int g_uiAnimationNodeIndexCount = 0;	//애니메이션 노드의 갯수
std::map<std::string, unsigned int> g_mIndexByName;

//애니메이션와 관련된 상수 버퍼
ID3D11Buffer *g_pd3dcbBoneMatrix = nullptr;
D3D11_MAPPED_SUBRESOURCE g_d3dMappedResource;
VS_CB_BONE_MATRIX *g_pcbBoneMatrix = nullptr;
*/



class CAniBuffer;
struct Animation
{
	string			strAniName;// 애니매이션의 이름. 즉 사용할 fbx의 확장자를 제외한 파일 이름.
	CAniBuffer*		pAniBuffer;//부모 및 자식노드용 버퍼.

	long long		llAniMaxTime;//애니매이션의 최대시간
								 //float			fAniPlayTimer;
	float			fAniPlaySpeed;//애니매이션의 속도

	XMMATRIX**		ppAniMatrix;//애니매니션의 행렬
	XMMATRIX*		pBaseBoneMatrix;//본의 행렬
	XMMATRIX**		ppResultMatrix;//최종 행렬

	unsigned int	nAniNodeIdxCnt;//노드의 인덱스카운트
	map<string, unsigned int>	mapIndexByName;


	ID3D11Buffer*				pBoneMatrixBuffer;//본의 컨스턴트버퍼
	D3D11_MAPPED_SUBRESOURCE	tMappedResource;//매핑서브리소스
	VS_CB_BONE_MATRIX*			pBoneMatrix;//본의 행렬을 갖고 있는 구조체

	Animation()
		: pAniBuffer(NULL)
		, llAniMaxTime(0)
		//, fAniPlayTimer(0.f)
		, fAniPlaySpeed(100.f)

		, ppAniMatrix(NULL)
		, pBaseBoneMatrix(NULL)
		, ppResultMatrix(NULL)

		, nAniNodeIdxCnt(0)
		, pBoneMatrixBuffer(NULL)
		, pBoneMatrix(NULL)
	{}
};


class CDynamicMesh :public CMesh
{
public:
	CDynamicMesh();
	virtual ~CDynamicMesh();

private:
	WORD					m_wCurrenAniIdx;//현재 애니인덱스
	vector<Animation*>		m_vecAni;//애니메이션 구조채를 담는 벡터
	int						m_iRepeatTime;//애니매이션을 반복할 횟수

public:
	static CDynamicMesh*		Create(const char* _pPath, vector<string> _vecAniName);
	//virtual CResource*		Clone();
	float					m_fAniPlayTimer;//현재 재생하고 있는 애니매이션의 시간
	bool					m_bAniEnd;//애니매이션이 끝낫는지의 여부를 알려주는 Bool값
	bool					Yamae;

protected:
	virtual HRESULT		Load_Model(const char*, vector<string> _vecAniName, FbxManager*, FbxIOSettings*, FbxScene*, FbxImporter*);


public:
	virtual HRESULT		Initialize(const char* _pPath, vector<string> _vecAniName);
public:
	virtual int		Update();

private:
	virtual DWORD Release();
	void Release_Animation();


public:
	void		PlayAnimation(int _iIdx);
	void		BWPlayAnim(int _iIdx);


	// CMesh을(를) 통해 상속됨
	virtual CResources * CloneResource(void) override;

};

