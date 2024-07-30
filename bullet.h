//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BULLET		(256)	// 弾最大数


//*****************************************************************************
// 構造体定義
//*****************************************************************************
class BULLET
{
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			life;			// 寿命
	int			shadowIdx;		// 影ID
	BOOL		use;			// 使用しているかどうか
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

