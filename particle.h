//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class PARTICLE
{
private:
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nIdxShadow;		// �eID
	int				nLife;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���

public:
	XMFLOAT3 getPos(void);
	void setPos(float x, float y, float z);
	void setPos(XMFLOAT3 pos);
	void setPosXYZ(char xyz, float value);

	XMFLOAT3 getRot(void);
	void setRot(float x, float y, float z);
	void setRotXYZ(char xyz, float value);

	XMFLOAT3 getScale(void);
	void setScale(float x, float y, float z);

	XMFLOAT3 getMove(void);
	void setMove(float x, float y, float z);
	void setMove(XMFLOAT3 move);
	void setMoveXYZ(char xyz, float value);


	MATERIAL getMaterial(void);
	void setMaterialDiff(float x, float y, float z, float w);
	void setMaterialDiff(XMFLOAT4 col);
	void setMaterialDiffXYZW(char xyz, float value);


	float getfSizeX(void);
	void setfSizeX(float x);

	float getfSizeY(void);
	void setfSizeY(float y);

	int getnIdxShadow(void);
	void setnIdxShadow(int idx);

	int getnLife(void);
	void setnLife(int life);

	BOOL getUse(void);
	void setUse(int use);

};

class ParticleManager
{
public:
	HRESULT InitParticle(void);
	void UninitParticle(void);
	void UpdateParticle(void);
	void DrawParticle(void);

	int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
	void SetColorParticle(int nIdxParticle, XMFLOAT4 col);
	HRESULT MakeVertexParticle(void);

};

