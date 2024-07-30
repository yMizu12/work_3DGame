//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"


#define LIGHT_DIRECTIONAL_COLOR_R	(0.5f)
#define LIGHT_DIRECTIONAL_COLOR_G	(0.3f)
#define LIGHT_DIRECTIONAL_COLOR_B	(0.7f)
#define LIGHT_DIRECTIONAL_COLOR_A	(1.0f)

#define FOG_START					(200.0f)
#define FOG_END						(550.0f)
#define FOG_COLOR_R					(0.0f)
#define FOG_COLOR_G					(0.0f)
#define FOG_COLOR_B					(0.0f)
#define FOG_COLOR_A					(1.0f)



//
static LIGHT	light[LIGHT_MAX];
static FOG		fog;
static BOOL		fogEnable = FALSE;
static BOOL		lightChange = TRUE;


//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		light[i].Attenuation = 100.0f;		// ��������
		light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &light[i]);
	}

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	light[0].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );		// ���̌���
	light[0].Diffuse   = XMFLOAT4(LIGHT_DIRECTIONAL_COLOR_R, LIGHT_DIRECTIONAL_COLOR_G, 
									LIGHT_DIRECTIONAL_COLOR_B, LIGHT_DIRECTIONAL_COLOR_A);	// ���̐F
	light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// ���s����
	light[0].Enable = TRUE;									// ���̃��C�g��ON
	SetLight(0, &light[0]);									// ����Őݒ肵�Ă���
	

	// �t�H�O�̏������i���̌��ʁj
	fog.FogStart = FOG_START;								// ���_���炱�̋��������ƃt�H�O��������n�߂�
	fog.FogEnd   = FOG_END;									// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	fog.FogColor = XMFLOAT4(FOG_COLOR_R, FOG_COLOR_G, 
							FOG_COLOR_B, FOG_COLOR_A);		// �t�H�O�̐F
	SetFog(&fog);
	SetFogEnable(fogEnable);		// ���̏ꏊ���`�F�b�N���� shadow

}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{

	// �|�C���g���C�g�̃e�X�g
	{

#ifdef _DEBUG
		// ���C�g�̐F�m�F
		if (GetKeyboardTrigger(DIK_F2))
		{
			lightChange = lightChange ? FALSE : TRUE;
		}

		if (lightChange == TRUE)
		{
			light[0].Diffuse = XMFLOAT4(LIGHT_DIRECTIONAL_COLOR_R, LIGHT_DIRECTIONAL_COLOR_G, 
										LIGHT_DIRECTIONAL_COLOR_B, LIGHT_DIRECTIONAL_COLOR_A);
		}
		else
		{
			light[0].Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		}

#endif


		SetLightData(0, &light[0]);
	}

}


//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&light[index]);
}


//=============================================================================
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL GetFogEnable(void)
{
	return(fogEnable);
}


