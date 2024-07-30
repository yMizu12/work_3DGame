//=============================================================================
//
// レンダリング処理 [renderer.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"

//デバッグ用画面テキスト出力を有効にする
#define DEBUG_DISP_TEXTOUT
//シェーダーデバッグ設定を有効にする
//#define DEBUG_SHADER


//
static D3D_FEATURE_LEVEL       featureLevel = D3D_FEATURE_LEVEL_11_0;

static ID3D11Device*           D3DDevice = NULL;
static ID3D11DeviceContext*    immediateContext = NULL;
static IDXGISwapChain*         swapChain = NULL;
static ID3D11RenderTargetView* renderTargetView = NULL;
static ID3D11DepthStencilView* depthStencilView = NULL;

static ID3D11VertexShader*		vertexShader = NULL;
static ID3D11PixelShader*		pixelShader = NULL;
static ID3D11InputLayout*		vertexLayout = NULL;
static ID3D11Buffer*			worldBuffer = NULL;
static ID3D11Buffer*			viewBuffer = NULL;
static ID3D11Buffer*			projectionBuffer = NULL;
static ID3D11Buffer*			materialBuffer = NULL;
static ID3D11Buffer*			lightBuffer = NULL;
static ID3D11Buffer*			fogBuffer = NULL;
static ID3D11Buffer*			dissolveBuffer = NULL;	// ディゾルブ
static ID3D11Buffer*			fuchiBuffer = NULL;
static ID3D11Buffer*			cameraBuffer = NULL;

static ID3D11DepthStencilState* depthStateEnable;
static ID3D11DepthStencilState* depthStateDisable;

static ID3D11BlendState*		blendStateNone;
static ID3D11BlendState*		blendStateAlphaBlend;
static ID3D11BlendState*		blendStateAdd;
static ID3D11BlendState*		blendStateSubtract;
static BLEND_MODE				blendStateParam;

static ID3D11RasterizerState*	rasterStateCullOff;
static ID3D11RasterizerState*	rasterStateCullCW;
static ID3D11RasterizerState*	rasterStateCullCCW;

static ID3D11Texture2D* depthTexture;
static ID3D11SamplerState* samplerState;
static ID3DBlob* pErrorBlob;
static ID3D11ShaderResourceView* fractalnoise;	// テクスチャ情報

static MATERIAL_CBUFFER	material;
static LIGHT_CBUFFER	light;
static FOG_CBUFFER		fog;
static DISSOLVE_CBUFFER dissolve;		// ディゾルブ	// シェーダーに渡す前にこれに格納

static FUCHI			fuchi;

static float clearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };	// 背景色


ID3D11Device* GetDevice( void )
{
	return D3DDevice;
}


ID3D11DeviceContext* GetDeviceContext( void )
{
	return immediateContext;
}


void SetDepthEnable( BOOL Enable )
{
	if( Enable )
		immediateContext->OMSetDepthStencilState( depthStateEnable, NULL );
	else
		immediateContext->OMSetDepthStencilState( depthStateDisable, NULL );

}


void SetBlendState(BLEND_MODE bm)
{
	blendStateParam = bm;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	switch (blendStateParam)
	{
	case BLEND_MODE_NONE:
		immediateContext->OMSetBlendState(blendStateNone, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ALPHABLEND:
		immediateContext->OMSetBlendState(blendStateAlphaBlend, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ADD:
		immediateContext->OMSetBlendState(blendStateAdd, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_SUBTRACT:
		immediateContext->OMSetBlendState(blendStateSubtract, blendFactor, 0xffffffff);
		break;
	}
}

void SetCullingMode(CULL_MODE cm)
{
	switch (cm)
	{
	case CULL_MODE_NONE:
		immediateContext->RSSetState(rasterStateCullOff);
		break;
	case CULL_MODE_FRONT:
		immediateContext->RSSetState(rasterStateCullCW);
		break;
	case CULL_MODE_BACK:
		immediateContext->RSSetState(rasterStateCullCCW);
		break;
	}
}

void SetAlphaTestEnable(BOOL flag)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	if (flag)
		blendDesc.AlphaToCoverageEnable = TRUE;
	else
		blendDesc.AlphaToCoverageEnable = FALSE;

	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	switch (blendStateParam)
	{
	case BLEND_MODE_NONE:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ALPHABLEND:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ADD:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_SUBTRACT:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = NULL;
	D3DDevice->CreateBlendState(&blendDesc, &blendState);
	immediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);

	if (blendState != NULL)
		blendState->Release();
}


void SetWorldViewProjection2D( void )
{
	XMMATRIX world;
	world = XMMatrixTranspose(XMMatrixIdentity());
	GetDeviceContext()->UpdateSubresource(worldBuffer, 0, NULL, &world, 0, 0);

	XMMATRIX view;
	view = XMMatrixTranspose(XMMatrixIdentity());
	GetDeviceContext()->UpdateSubresource(viewBuffer, 0, NULL, &view, 0, 0);

	XMMATRIX worldViewProjection;
	worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	worldViewProjection = XMMatrixTranspose(worldViewProjection);
	GetDeviceContext()->UpdateSubresource(projectionBuffer, 0, NULL, &worldViewProjection, 0, 0);
}


void SetWorldMatrix( XMMATRIX *WorldMatrix )
{
	XMMATRIX world;
	world = *WorldMatrix;
	world = XMMatrixTranspose(world);

	GetDeviceContext()->UpdateSubresource(worldBuffer, 0, NULL, &world, 0, 0);
}

void SetViewMatrix(XMMATRIX *ViewMatrix )
{
	XMMATRIX view;
	view = *ViewMatrix;
	view = XMMatrixTranspose(view);

	GetDeviceContext()->UpdateSubresource(viewBuffer, 0, NULL, &view, 0, 0);
}

void SetProjectionMatrix( XMMATRIX *ProjectionMatrix )
{
	XMMATRIX projection;
	projection = *ProjectionMatrix;
	projection = XMMatrixTranspose(projection);

	GetDeviceContext()->UpdateSubresource(projectionBuffer, 0, NULL, &projection, 0, 0);
}

void SetMaterial( MATERIAL material )
{
	material.Diffuse = material.Diffuse;
	material.Ambient = material.Ambient;
	material.Specular = material.Specular;
	material.Emission = material.Emission;
	material.Shininess = material.Shininess;
	material.noTexSampling = material.noTexSampling;

	GetDeviceContext()->UpdateSubresource( materialBuffer, 0, NULL, &material, 0, 0 );
}

void SetLightBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(lightBuffer, 0, NULL, &light, 0, 0);
}

void SetLightEnable(BOOL flag)
{
	// フラグを更新する
	light.Enable = flag;

	SetLightBuffer();
}

void SetLight(int index, LIGHT* pLight)
{
	light.Position[index] = XMFLOAT4(pLight->Position.x, pLight->Position.y, pLight->Position.z, 0.0f);
	light.Direction[index] = XMFLOAT4(pLight->Direction.x, pLight->Direction.y, pLight->Direction.z, 0.0f);
	light.Diffuse[index] = pLight->Diffuse;
	light.Ambient[index] = pLight->Ambient;
	light.Flags[index].Type = pLight->Type;
	light.Flags[index].OnOff = pLight->Enable;
	light.Attenuation[index].x = pLight->Attenuation;

	SetLightBuffer();
}

void SetFogBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(fogBuffer, 0, NULL, &fog, 0, 0);
}

void SetFogEnable(BOOL flag)
{
	// フラグを更新する
	fog.Enable = flag;

	SetFogBuffer();
}

void SetFog(FOG* pFog)
{
	fog.Fog.x = pFog->FogStart;
	fog.Fog.y = pFog->FogEnd;
	fog.FogColor = pFog->FogColor;

	SetFogBuffer();
}

void SetFuchi(int flag)
{
	fuchi.fuchi = flag;
	GetDeviceContext()->UpdateSubresource(fuchiBuffer, 0, NULL, &fuchi, 0, 0);
}


void SetShaderCamera(XMFLOAT3 pos)
{
	XMFLOAT4 tmp = XMFLOAT4( pos.x, pos.y, pos.z, 0.0f );

	GetDeviceContext()->UpdateSubresource(cameraBuffer, 0, NULL, &tmp, 0, 0);
}

//=========================================================================
// ディゾルブ
//=========================================================================
void SetDissolveBuffer(void)	// ★ここでシェーダーに渡す
{
	// シェーダーのグローバル変数を書き換える
	GetDeviceContext()->UpdateSubresource(dissolveBuffer, 0, NULL, &dissolve, 0, 0);
}

void SetDissolveEnable(BOOL flag)
{
	// フラグを更新する
	dissolve.Enable = flag;

	SetDissolveBuffer();
}

void SetDissolve(DISSOLVE* pDissolve)
{

	dissolve.Enable = pDissolve->Enable;
	dissolve.threshold = pDissolve->threshold;

	SetDissolveBuffer();
}
// ここまで

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン、コンテキスト生成
	DWORD deviceFlags = 0;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = bWindow;

	//デバッグ文字出力用設定
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
	deviceFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

	hr = D3D11CreateDeviceAndSwapChain( NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										deviceFlags,
										NULL,
										0,
										D3D11_SDK_VERSION,
										&sd,
										&swapChain,
										&D3DDevice,
										&featureLevel,
										&immediateContext );
	if( FAILED( hr ) )
		return hr;

	//デバッグ文字出力用設定
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	hr = swapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE); // N.B. the GDI compatible flag
	if (FAILED(hr))
		return hr;
#endif

	// レンダーターゲットビュー生成、設定
	ID3D11Texture2D* pBackBuffer = NULL;
	swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &renderTargetView );
	pBackBuffer->Release();



	//ステンシル用テクスチャー作成
	depthTexture = NULL;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory( &td, sizeof(td) );
	td.Width			= sd.BufferDesc.Width;
	td.Height			= sd.BufferDesc.Height;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc		= sd.SampleDesc;
	td.Usage			= D3D11_USAGE_DEFAULT;
	td.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags	= 0;
    td.MiscFlags		= 0;
	D3DDevice->CreateTexture2D( &td, NULL, &depthTexture );

	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	D3DDevice->CreateDepthStencilView( depthTexture, &dsvd, &depthStencilView );


	immediateContext->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );


	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediateContext->RSSetViewports( 1, &vp );



	// ラスタライザステート作成
	D3D11_RASTERIZER_DESC rd; 
	ZeroMemory( &rd, sizeof( rd ) );
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE; 
	rd.DepthClipEnable = TRUE; 
	rd.MultisampleEnable = FALSE; 
	D3DDevice->CreateRasterizerState( &rd, &rasterStateCullOff);

	rd.CullMode = D3D11_CULL_FRONT;
	D3DDevice->CreateRasterizerState(&rd, &rasterStateCullCW);

	rd.CullMode = D3D11_CULL_BACK;
	D3DDevice->CreateRasterizerState(&rd, &rasterStateCullCCW);

	// カリングモード設定（CCW）
	SetCullingMode(CULL_MODE_BACK);



	// ブレンドステートの作成
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof( blendDesc ) );
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	D3DDevice->CreateBlendState( &blendDesc, &blendStateAlphaBlend );

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	D3DDevice->CreateBlendState(&blendDesc, &blendStateNone);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	D3DDevice->CreateBlendState(&blendDesc, &blendStateAdd);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	D3DDevice->CreateBlendState(&blendDesc, &blendStateSubtract);

	// アルファブレンド設定
	SetBlendState(BLEND_MODE_ALPHABLEND);




	// 深度ステンシルステート作成
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	D3DDevice->CreateDepthStencilState( &depthStencilDesc, &depthStateEnable );//深度有効ステート

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	D3DDevice->CreateDepthStencilState( &depthStencilDesc, &depthStateDisable );//深度無効ステート

	// 深度ステンシルステート設定
	SetDepthEnable(TRUE);



	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerState = NULL;
	D3DDevice->CreateSamplerState( &samplerDesc, &samplerState );

	immediateContext->PSSetSamplers( 0, 1, &samplerState );



	// 頂点シェーダコンパイル・生成
	pErrorBlob;
	ID3DBlob* pVSBlob = NULL;
	DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) && defined(DEBUG_SHADER)
	shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DX11CompileFromFile( "shader.hlsl", NULL, NULL, "VertexShaderPolygon", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		MessageBox( NULL , (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR );
	}

	D3DDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &vertexShader );

	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE( layout );

	D3DDevice->CreateInputLayout( layout,
		numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&vertexLayout );

	pVSBlob->Release();


	// ピクセルシェーダコンパイル・生成
	ID3DBlob* pPSBlob = NULL;
	hr = D3DX11CompileFromFile( "shader.hlsl", NULL, NULL, "PixelShaderPolygon", "ps_4_0", shFlag, 0, NULL, &pPSBlob, &pErrorBlob, NULL );
	if( FAILED(hr) )
	{
		MessageBox( NULL , (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR );
	}

	D3DDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pixelShader );
	
	pPSBlob->Release();


	//=========================================================================
	// ディゾルブ
	//=========================================================================
	{
		fractalnoise;	// テクスチャ情報

		// 貼るテクスチャを設定
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			"data/TEXTURE/fractalnoise.jpg",
			NULL,
			NULL,
			&fractalnoise,
			NULL);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(1, 1, &fractalnoise);
	}


	// 定数バッファ生成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(XMMATRIX);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	//ワールドマトリクス
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &worldBuffer);
	immediateContext->VSSetConstantBuffers(0, 1, &worldBuffer);
	immediateContext->PSSetConstantBuffers(0, 1, &worldBuffer);

	//ビューマトリクス
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &viewBuffer);
	immediateContext->VSSetConstantBuffers(1, 1, &viewBuffer);
	immediateContext->PSSetConstantBuffers(1, 1, &viewBuffer);

	//プロジェクションマトリクス
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &projectionBuffer);
	immediateContext->VSSetConstantBuffers(2, 1, &projectionBuffer);
	immediateContext->PSSetConstantBuffers(2, 1, &projectionBuffer);

	//マテリアル情報
	hBufferDesc.ByteWidth = sizeof(MATERIAL_CBUFFER);
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &materialBuffer);
	immediateContext->VSSetConstantBuffers(3, 1, &materialBuffer);
	immediateContext->PSSetConstantBuffers(3, 1, &materialBuffer);

	//ライト情報
	hBufferDesc.ByteWidth = sizeof(LIGHT_CBUFFER);
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &lightBuffer);
	immediateContext->VSSetConstantBuffers(4, 1, &lightBuffer);
	immediateContext->PSSetConstantBuffers(4, 1, &lightBuffer);

	//フォグ情報
	hBufferDesc.ByteWidth = sizeof(FOG_CBUFFER);
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &fogBuffer);
	immediateContext->VSSetConstantBuffers(5, 1, &fogBuffer);
	immediateContext->PSSetConstantBuffers(5, 1, &fogBuffer);

	//縁取り
	hBufferDesc.ByteWidth = sizeof(FUCHI);
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &fuchiBuffer);
	immediateContext->VSSetConstantBuffers(6, 1, &fuchiBuffer);
	immediateContext->PSSetConstantBuffers(6, 1, &fuchiBuffer);

	//=========================================================================
	// ディゾルヴ情報
	//=========================================================================
	hBufferDesc.ByteWidth = sizeof(DISSOLVE_CBUFFER);
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &dissolveBuffer);
	immediateContext->VSSetConstantBuffers(8, 1, &dissolveBuffer);	//VS 頂点シェーダーにセット
	immediateContext->PSSetConstantBuffers(8, 1, &dissolveBuffer);	//PS ピクセルシェーダにセット

	//カメラ
	hBufferDesc.ByteWidth = sizeof(XMFLOAT4);
	D3DDevice->CreateBuffer(&hBufferDesc, NULL, &cameraBuffer);
	immediateContext->VSSetConstantBuffers(7, 1, &cameraBuffer);
	immediateContext->PSSetConstantBuffers(7, 1, &cameraBuffer);


	// 入力レイアウト設定
	immediateContext->IASetInputLayout( vertexLayout );

	// シェーダ設定
	immediateContext->VSSetShader( vertexShader, NULL, 0 );
	immediateContext->PSSetShader( pixelShader, NULL, 0 );

	//ライト初期化
	ZeroMemory(&light, sizeof(LIGHT_CBUFFER));
	light.Direction[0] = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	light.Diffuse[0] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	light.Ambient[0] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.Flags[0].Type = LIGHT_TYPE_DIRECTIONAL;
	SetLightBuffer();


	//マテリアル初期化
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	//=========================================================================
	// ディゾルブ初期化
	//=========================================================================
	DISSOLVE dissolve;
	ZeroMemory(&dissolve, sizeof(dissolve));
	dissolve.Enable = FALSE;
	dissolve.threshold = 0.0f;
	SetDissolve(&dissolve);

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitRenderer(void)
{
	// オブジェクト解放
	if (depthStateEnable)		depthStateEnable->Release();
	if (depthStateDisable)	depthStateDisable->Release();
	if (blendStateNone)		blendStateNone->Release();
	if (blendStateAlphaBlend)	blendStateAlphaBlend->Release();
	if (blendStateAdd)		blendStateAdd->Release();
	if (blendStateSubtract)	blendStateSubtract->Release();
	if (rasterStateCullOff)	rasterStateCullOff->Release();
	if (rasterStateCullCW)	rasterStateCullCW->Release();
	if (rasterStateCullCCW)	rasterStateCullCCW->Release();

	if (worldBuffer)			worldBuffer->Release();
	if (viewBuffer)			viewBuffer->Release();
	if (projectionBuffer)		projectionBuffer->Release();
	if (materialBuffer)		materialBuffer->Release();
	if (lightBuffer)			lightBuffer->Release();
	if (dissolveBuffer)		dissolveBuffer->Release();	//ディゾルブ
	if (fogBuffer)			fogBuffer->Release();

	if (vertexLayout)			vertexLayout->Release();
	if (vertexShader)			vertexShader->Release();
	if (pixelShader)			pixelShader->Release();

	if (depthStencilView) depthStencilView->Release();
	if (fuchiBuffer) fuchiBuffer->Release();
	if (cameraBuffer) cameraBuffer->Release();
	if (depthTexture) depthTexture->Release();
	if (samplerState) samplerState->Release();
	if (pErrorBlob) pErrorBlob->Release();
	if (fractalnoise) fractalnoise->Release();

	if (immediateContext)		immediateContext->ClearState();
	if (renderTargetView)		renderTargetView->Release();
	if (swapChain)			swapChain->Release();
	if (immediateContext)		immediateContext->Release();
	if (D3DDevice)			D3DDevice->Release();
}



//=============================================================================
// バックバッファクリア
//=============================================================================
void Clear(void)
{
	// バックバッファクリア
	immediateContext->ClearRenderTargetView( renderTargetView, clearColor );
	immediateContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void SetClearColor(float* color4)
{
	clearColor[0] = color4[0];
	clearColor[1] = color4[1];
	clearColor[2] = color4[2];
	clearColor[3] = color4[3];
}


//=============================================================================
// プレゼント
//=============================================================================
void Present(void)
{
	swapChain->Present( 0, 0 );

}


//=============================================================================
// デバッグ用テキスト出力
//=============================================================================
void DebugTextOut(char* text, int x, int y)
{
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	HRESULT hr;

	//バックバッファからサーフェスを取得する
	IDXGISurface1* pBackSurface = NULL;
	hr = swapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackSurface);

	if (SUCCEEDED(hr))
	{
		//取得したサーフェスからデバイスコンテキストを取得する
		HDC hdc;
		hr = pBackSurface->GetDC(FALSE, &hdc);

		if (SUCCEEDED(hr))
		{
			//文字色を白に変更
			SetTextColor(hdc, RGB(255, 255, 255));
			//背景を透明に変更
			SetBkMode(hdc, TRANSPARENT);

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = SCREEN_WIDTH;
			rect.bottom = SCREEN_HEIGHT;

			//テキスト出力
			DrawText(hdc, text, (int)strlen(text), &rect, DT_LEFT);

			//デバイスコンテキストを解放する
			pBackSurface->ReleaseDC(NULL);
		}
		//サーフェスを解放する
		pBackSurface->Release();

		//レンダリングターゲットがリセットされるのでセットしなおす
		immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	}
#endif
}
