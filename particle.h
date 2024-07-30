//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once


//*****************************************************************************
// 構造体定義
//*****************************************************************************
class PARTICLE
{
private:
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nIdxShadow;		// 影ID
	int				nLife;			// 寿命
	BOOL			use;			// 使用しているかどうか

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

