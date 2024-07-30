//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET		(256)	// �e�ő吔


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class BULLET
{
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			life;			// ����
	int			shadowIdx;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���
	BOOL		load;

public:
	DX11_MODEL	model;

	XMFLOAT4X4	getMtxWorld(void);
	void		setMtxWorld(XMFLOAT4X4 mtx);
	
	XMFLOAT3	getPos(void);
	void		setPos(XMFLOAT3 pos);
	void		setPos(float x, float y, float z);
	void		setPosXYZ(char xyz, float value);

	XMFLOAT3	getRot(void);
	void		setRot(XMFLOAT3 rot);
	void		setRot(float x, float y, float z);

	XMFLOAT3	getScl(void);
	void		setScl(XMFLOAT3 scl);
	void		setScl(float x, float y, float z);

	MATERIAL	getMaterial(void);
	void		setMaterial(MATERIAL matrial);
	void		setMaterialDiffuse(float r, float g, float b, float a);

	float		getSpd(void);
	void		setSpd(float spd);

	float		getfWidth(void);
	void		setfWidth(float w);

	float		getfHeight(void);
	void		setfHeight(float h);

	int			getLife(void);
	void		setLife(int life);

	int			getShadowIdx(void);
	void		setShadowIdx(int idx);

	BOOL		getUse(void);
	void		setUse(BOOL use);

	BOOL		getLoad(void);
	void		setLoad(BOOL load);

	DX11_MODEL	getModel(void);
	void		setModel(DX11_MODEL model);

};


class BulletManager
{
public:
	HRESULT InitBullet(void);
	void UninitBullet(void);
	void UpdateBullet(void);
	void DrawBullet(void);
	int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);
	BULLET* GetBullet(void);

};

