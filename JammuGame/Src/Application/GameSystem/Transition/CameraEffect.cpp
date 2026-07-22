#include "CameraEffect.h"
#include "../../main.h"
#include "Shader/Camera_VS.shaderInc"

//カメラエフェクトフェード
#include "Shader/Transition/CameraGlitch_PS.shaderInc"
#include "Shader/Transition/CameraOverlay_PS.shaderInc"
#include "Shader/Transition/CameraRipple_PS.shaderInc"
#include "Shader/Transition/CameraScatter_PS.shaderInc"
#include "Shader/Transition/CameraBlind_PS.shaderInc"
#include "Shader/Transition/CameraBurn_PS.shaderInc"

//カメラエフェクト
#include "Shader/EffectType/Camera_PS.shaderInc"
#include "Shader/EffectType/CameraGray_PS.shaderInc"
#include "Shader/EffectType/CameraNone_PS.shaderInc"
#include "Shader/EffectType/CameraAnime_PS.shaderInc"
#include "Shader/EffectType/CameraLetoro_PS.shaderInc"
#include "Shader/EffectType/CameraSumie_PS.shaderInc"

struct Vertex
{
	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT2 uv;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

Vertex quad[] =
{
	{ {-1.0f,  1.0f}, {0.0f, 0.0f} },
	{ { 1.0f,  1.0f}, {1.0f, 0.0f} },
	{ {-1.0f, -1.0f}, {0.0f, 1.0f} },
	{ { 1.0f, -1.0f}, {1.0f, 1.0f} },
};

void CameraEffect::InitBuffer()
{
	if (isInit)return;

	ID3D11Device* dev = KdDirect3D::Instance().WorkDev();

	// バックバッファのフォーマット取得＆キャッシュ
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	{
		KdDirect3D::Instance().WorkSwapChain()->GetBuffer(
			0, IID_PPV_ARGS(&m_backBufferTex));
		D3D11_TEXTURE2D_DESC bbDesc = {};
		m_backBufferTex->GetDesc(&bbDesc);
		backBufferFormat = bbDesc.Format;

		HRESULT hr = dev->CreateRenderTargetView(
			m_backBufferTex.Get(), nullptr, &m_backBufferRTV);
		assert(SUCCEEDED(hr) && "バックバッファRTV作成失敗");
	}

	// RTテクスチャ（エフェクト入力用）
	auto CreateRT = [&](Microsoft::WRL::ComPtr<ID3D11Texture2D>& tex,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
		{
			D3D11_TEXTURE2D_DESC desc = {};
			desc.Width = kScreenWidth;
			desc.Height = kScreenHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = backBufferFormat;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

			HRESULT hr = dev->CreateTexture2D(&desc, nullptr, &tex);
			assert(SUCCEEDED(hr));
			hr = dev->CreateRenderTargetView(tex.Get(), nullptr, &rtv);
			assert(SUCCEEDED(hr));
			hr = dev->CreateShaderResourceView(tex.Get(), nullptr, &srv);
			assert(SUCCEEDED(hr));
		};

	CreateRT(m_rtTex, m_rtv, m_srv);
	CreateRT(m_rtTexTransition, m_rtvTransition, m_srvTransition);

	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = kScreenWidth;
		desc.Height = kScreenHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DSV専用に変更
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;   // SRVフラグ削除

		HRESULT hr = dev->CreateTexture2D(&desc, nullptr, &m_depthTex);
		assert(SUCCEEDED(hr));
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		hr = dev->CreateDepthStencilView(m_depthTex.Get(), &dsvDesc, &m_depthDSV);
		assert(SUCCEEDED(hr));
	}

	// テクスチャ2: 深度コピー（SRV専用）← 新規追加
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = kScreenWidth;
		desc.Height = kScreenHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // DSVフラグなし

		HRESULT hr = dev->CreateTexture2D(&desc, nullptr, &m_depthCopyTex);
		assert(SUCCEEDED(hr));
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		hr = dev->CreateShaderResourceView(m_depthCopyTex.Get(), &srvDesc, &m_depthSRV);
		assert(SUCCEEDED(hr));
	}

	// 頂点シェーダー
	{
		HRESULT hr = dev->CreateVertexShader(
			cameraVS, sizeof(cameraVS), nullptr, &m_vs);
		assert(SUCCEEDED(hr) && "VS作成失敗");
		hr = dev->CreateInputLayout(
			layout, ARRAYSIZE(layout),
			cameraVS, sizeof(cameraVS), &m_inputLayout);
		assert(SUCCEEDED(hr) && "InputLayout作成失敗");
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(quad);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA initData = { quad };
		HRESULT hr = dev->CreateBuffer(&bd, &initData, &m_vertexBuffer);
		assert(SUCCEEDED(hr) && "VB作成失敗");
	}

	// 定数バッファ
	{
		D3D11_BUFFER_DESC cd = {};
		cd.Usage = D3D11_USAGE_DYNAMIC;
		cd.ByteWidth = sizeof(StateParam);
		cd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HRESULT hr = dev->CreateBuffer(&cd, nullptr, &m_stateCB);
		assert(SUCCEEDED(hr) && "CB作成失敗");
	}

	// サンプラー
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		HRESULT hr = dev->CreateSamplerState(&desc, &m_sampler);
		assert(SUCCEEDED(hr) && "サンプラー作成失敗");
	}

	// ラスタライザー
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthClipEnable = TRUE;
		HRESULT hr = dev->CreateRasterizerState(&desc, &m_raster);
		assert(SUCCEEDED(hr) && "ラスタライザー作成失敗");
	}

	// ブレンドステート（不透明）
	{
		D3D11_BLEND_DESC desc = {};
		desc.RenderTarget[0].BlendEnable = FALSE;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT hr = dev->CreateBlendState(&desc, &m_blendStateOpaque);
		assert(SUCCEEDED(hr) && "BlendState作成失敗");
	}

	// ブレンドステート（アルファブレンド）
	{
		D3D11_BLEND_DESC desc = {};
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT hr = dev->CreateBlendState(&desc, &m_blendStateAlpha);
		assert(SUCCEEDED(hr) && "AlphaBlendState作成失敗");
	}

	// デプスステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = FALSE;
		desc.StencilEnable = FALSE;
		HRESULT hr = dev->CreateDepthStencilState(&desc, &m_depthStencilState);
		assert(SUCCEEDED(hr) && "DSS作成失敗");
	}

	// ピクセルシェーダーをバイトコードから登録
	RegisterEffect("none", cameraNonePS, sizeof(cameraNonePS));
	RegisterEffect("map", cameraPS, sizeof(cameraPS));
	RegisterEffect("gray", cameraGrayPS, sizeof(cameraGrayPS));
	RegisterEffect("anime", cameraAnimePS, sizeof(cameraAnimePS));
	RegisterEffect("letoro", cameraLetoroPS, sizeof(cameraLetoroPS));
	RegisterEffect("sumie", cameraSumiePS, sizeof(cameraSumiePS));

	//トランジション用ピクセルシェーダを設定
	RegisterTransitionEffect("overlay", fadeOverlayPS, sizeof(fadeOverlayPS));
	RegisterTransitionEffect("glitch", fadeGlitchPS, sizeof(fadeGlitchPS));
	RegisterTransitionEffect("ripple", fadeRipplePS, sizeof(fadeRipplePS));
	RegisterTransitionEffect("scatter", fadeScatterPS, sizeof(fadeScatterPS));
	RegisterTransitionEffect("blind", fadeBlindPS, sizeof(fadeBlindPS));
	RegisterTransitionEffect("burn", fadeBurnPS, sizeof(fadeBurnPS));

	SetEffect("sumie");

	isInit = true;
}

void CameraEffect::Update(){

	m_time += Application::Instance().GetDeltaTime();

	// 遷移中でなければここで終了
	if (!m_isTransitioning) return;

	m_transitionProgress += 0.016f / m_transitionDuration;
	m_transitionProgress = std::min(m_transitionProgress, 1.0f);

	// ワイプ系: キャプチャ完了後の次フレームでシーン切替
	if (m_wipeMode && !m_pendingCapture && !m_switchFired)
	{
		m_switchFired = true;
		m_currentEffect = m_pendingEffect;
		if (m_pendingOnSwitch)
		{
			m_pendingOnSwitch();
			m_pendingOnSwitch = nullptr;
		}
	}

	// カバー系: switchPointでシーン切替
	if (!m_wipeMode && m_onSwitch && !m_switchFired &&
		m_transitionProgress >= m_switchPoint)
	{
		m_switchFired = true;
		m_sceneSwitched = true;
		m_currentEffect = m_pendingEffect;
		m_onSwitch();
		m_onSwitch = nullptr;
	}

	// トランジション完了
	if (m_transitionProgress >= 1.0f)
	{
		m_transitionProgress = 0.0f;
		m_isTransitioning = false;
		m_currentTransition = "none";
		m_sceneSwitched = false;
		m_switchFired = false;
		m_wipeMode = false;
		m_pendingCapture = false;
		m_pendingOnSwitch = nullptr;
		m_onSwitch = nullptr;
	}

}

void CameraEffect::DrawFullscreenQuad(ID3D11PixelShader* ps, ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv, float blendFactor, bool useAlphaBlend)
{
	ID3D11DeviceContext* context = KdDirect3D::Instance().WorkDevContext();

	// 定数バッファ更新
	StateParam param = {};
	param.time = m_time;
	param.blendFactor = blendFactor;
	param.dreamPow = m_dreamPow;
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(m_stateCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &param, sizeof(param));
	context->Unmap(m_stateCB.Get(), 0);

	// RTV・SRVをセット
	context->OMSetRenderTargets(1, &rtv, nullptr);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->PSSetShaderResources(0, 1, srv ? &srv : &nullSRV);

	ID3D11ShaderResourceView* depthSRV = m_currentDepthSRV.Get();
	context->PSSetShaderResources(1, 1, &depthSRV);

	// t2: 法線
	ID3D11ShaderResourceView* normalSRV = m_currentNormalSRV.Get();
	context->PSSetShaderResources(2, 1, &normalSRV);

	// t3: ID ← 追加
	ID3D11ShaderResourceView* idSRV = m_currentIdSRV.Get();
	context->PSSetShaderResources(3, 1, &idSRV);

	// ブレンドステート
	float bf[4] = { 1, 1, 1, 1 };
	context->OMSetBlendState(
		useAlphaBlend ? m_blendStateAlpha.Get()
		: m_blendStateOpaque.Get(),
		bf, 0xFFFFFFFF);

	context->PSSetShader(ps, nullptr, 0);
	context->Draw(4, 0);

	ID3D11ShaderResourceView* nullSRVs[4] = { nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 4, nullSRVs);
}

void CameraEffect::RenderFromSRV(ID3D11ShaderResourceView* inputSRV)
{
	ID3D11DeviceContext* context = KdDirect3D::Instance().WorkDevContext();

	// GPUステート保存
	Microsoft::WRL::ComPtr<ID3D11VertexShader>      savedVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>       savedPS;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>    savedGS;
	Microsoft::WRL::ComPtr<ID3D11HullShader>        savedHS;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>      savedDS;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>       savedIL;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            savedVB;
	Microsoft::WRL::ComPtr<ID3D11BlendState>        savedBS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> savedDSS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   savedRS;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  savedRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  savedDSV;
	UINT  savedStride = 0, savedOffset = 0, savedStencilRef = 0;
	float savedBF[4] = {};
	UINT  savedSampleMask = 0;

	UINT numVP = 1;
	D3D11_VIEWPORT savedVP = {};
	context->RSGetViewports(&numVP, &savedVP);

	D3D11_PRIMITIVE_TOPOLOGY savedTopology;
	context->IAGetPrimitiveTopology(&savedTopology);

	Microsoft::WRL::ComPtr<ID3D11Buffer> savedPSCB;
	context->PSGetConstantBuffers(0, 1, savedPSCB.GetAddressOf());

	context->VSGetShader(savedVS.GetAddressOf(), nullptr, nullptr);
	context->PSGetShader(savedPS.GetAddressOf(), nullptr, nullptr);
	context->GSGetShader(savedGS.GetAddressOf(), nullptr, nullptr);
	context->HSGetShader(savedHS.GetAddressOf(), nullptr, nullptr);
	context->DSGetShader(savedDS.GetAddressOf(), nullptr, nullptr);
	context->IAGetInputLayout(savedIL.GetAddressOf());
	context->IAGetVertexBuffers(0, 1, savedVB.GetAddressOf(), &savedStride, &savedOffset);
	context->OMGetBlendState(savedBS.GetAddressOf(), savedBF, &savedSampleMask);
	context->OMGetDepthStencilState(savedDSS.GetAddressOf(), &savedStencilRef);
	context->RSGetState(savedRS.GetAddressOf());
	context->OMGetRenderTargets(1, savedRTV.GetAddressOf(), savedDSV.GetAddressOf());

	// savedRTVが現在フレームの正しいバックバッファRTV
	ID3D11RenderTargetView* backRTV = savedRTV.Get();

	// 共通ステート設定
	context->GSSetShader(nullptr, nullptr, 0);
	context->HSSetShader(nullptr, nullptr, 0);
	context->DSSetShader(nullptr, nullptr, 0);

	D3D11_VIEWPORT vp = {};
	vp.Width = (float)kScreenWidth; vp.Height = (float)kScreenHeight;
	vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	UINT stride = sizeof(Vertex), offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(m_inputLayout.Get());
	context->RSSetState(m_raster.Get());
	context->VSSetShader(m_vs.Get(), nullptr, 0);
	context->PSSetConstantBuffers(0, 1, m_stateCB.GetAddressOf());
	context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// SRV・RTVを全部アンバインドしてからCopyResource
	ID3D11ShaderResourceView* nullSRVs[4] = {};
	context->PSSetShaderResources(0, 4, nullSRVs);
	context->VSSetShaderResources(0, 4, nullSRVs);
	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, nullptr);

	// エフェクト描画
	if (!m_isTransitioning)
	{
		auto it = m_psMap.find(m_currentEffect);
		if (it != m_psMap.end())
		{
			DrawFullscreenQuad(it->second.Get(), inputSRV, backRTV, 1.0f, false);
		}
	}
	else
	{
		context->PSSetShaderResources(0, 4, nullSRVs);

		if (!m_sceneSwitched)
		{
			auto itPrev = m_psMap.find(m_prevEffect);
			if (itPrev != m_psMap.end())
			{
				DrawFullscreenQuad(itPrev->second.Get(), inputSRV, m_rtvTransition.Get(), 1.0f, false);
			}

			if (m_pendingCapture)
			{
				m_pendingCapture = false;
				m_sceneSwitched = true;
			}
		}

		auto itCurr = m_psMap.find(m_currentEffect);
		if (itCurr != m_psMap.end())
		{
			DrawFullscreenQuad(itCurr->second.Get(), inputSRV, backRTV, 1.0f, false);
		}

		auto itTrans = m_psTransitionMap.find(m_currentTransition);
		if (itTrans != m_psTransitionMap.end())
		{
			DrawFullscreenQuad(itTrans->second.Get(), m_srvTransition.Get(), backRTV, m_transitionProgress, true);
		}
	}

	// inputSRVの内容をm_rtTexにコピー（次フレーム用）
	{
		ID3D11Resource* res = nullptr;
		inputSRV->GetResource(&res);
		if (res)
		{
			context->PSSetShaderResources(0, 4, nullSRVs);
			context->OMSetRenderTargets(1, &nullRTV, nullptr); // アンバインドしてからコピー
			context->CopyResource(m_rtTex.Get(), res);
			res->Release();
		}
	}

	// GPUステート復元
	context->VSSetShader(savedVS.Get(), nullptr, 0);
	context->PSSetShader(savedPS.Get(), nullptr, 0);
	context->GSSetShader(savedGS.Get(), nullptr, 0);
	context->HSSetShader(savedHS.Get(), nullptr, 0);
	context->DSSetShader(savedDS.Get(), nullptr, 0);
	context->IASetInputLayout(savedIL.Get());
	context->IASetVertexBuffers(0, 1, savedVB.GetAddressOf(), &savedStride, &savedOffset);
	context->OMSetBlendState(savedBS.Get(), savedBF, savedSampleMask);
	context->OMSetDepthStencilState(savedDSS.Get(), savedStencilRef);
	context->RSSetState(savedRS.Get());
	context->RSSetViewports(1, &savedVP);
	context->IASetPrimitiveTopology(savedTopology);
	context->PSSetConstantBuffers(0, 1, savedPSCB.GetAddressOf());

	context->PSSetShaderResources(0, 4, nullSRVs);
	context->VSSetShaderResources(0, 4, nullSRVs);
	context->OMSetRenderTargets(1, &backRTV, savedDSV.Get());
}

void CameraEffect::RegisterEffect(const std::string& _key, const void* bytecode, SIZE_T bytecodeSize)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	HRESULT hr = KdDirect3D::Instance().WorkDev()->CreatePixelShader(
		bytecode, bytecodeSize, nullptr, &ps);
	assert(SUCCEEDED(hr) && "PS作成失敗");
	m_psMap[_key] = ps;
}

void CameraEffect::RegisterTransitionEffect(const std::string& _key, const void* bytecode, SIZE_T bytecodeSize)
{
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
	HRESULT hr = KdDirect3D::Instance().WorkDev()->CreatePixelShader(
		bytecode, bytecodeSize, nullptr, &ps);
	assert(SUCCEEDED(hr) && "Transition PS作成失敗");
	m_psTransitionMap[_key] = ps;
}

void CameraEffect::SetEffect(const std::string& _key, const std::string& _Transitionkey,float duration)
{
	// 未登録のエフェクトキーは無視
	if (m_psMap.find(_key) == m_psMap.end())
	{
		char buf[256];
		sprintf_s(buf, "[CE] SetEffect: 未登録のエフェクト '%s'\n", _key.c_str());
		OutputDebugStringA(buf);
		return;
	}

	// 同じエフェクトへの切り替えは無視
	if (_key == m_currentEffect) return;

	// 遷移中に別のSetEffectが呼ばれた場合は現在の遷移を中断して切り替え
	m_prevEffect = m_currentEffect; // 旧エフェクトを保存
	m_currentEffect = _key;       // 新エフェクトに更新

	// "none" または未登録のトランジションキーは即時切り替え
	if (_Transitionkey == "none" ||
		m_psTransitionMap.find(_Transitionkey) == m_psTransitionMap.end())
	{
		m_isTransitioning = false;
		return;
	}

	// トランジション開始
	m_currentTransition = _Transitionkey;
	m_transitionDuration = duration;
	m_transitionProgress = 0.0f;
	m_isTransitioning = true;
	m_sceneSwitched = false;
}

void CameraEffect::StartCoverTransition(
	const std::string& transitionKey,
	float                 duration,
	std::function<void()> onSwitch,
	const std::string& newEffect,
	float                 switchPoint)
{
	m_prevEffect = m_currentEffect;
	m_pendingEffect = newEffect;
	m_onSwitch = onSwitch;
	m_switchFired = false;
	m_switchPoint = switchPoint;
	m_currentTransition = transitionKey;
	m_transitionDuration = duration;
	m_transitionProgress = 0.0f;
	m_isTransitioning = true;
	m_sceneSwitched = false; // 毎フレーム旧シーンを更新し続ける
}

void CameraEffect::StartWipeTransition(
	const std::string& transitionKey,
	float                 duration,
	std::function<void()> onSwitch,
	const std::string& newEffect)
{
	ID3D11DeviceContext* ctx = KdDirect3D::Instance().WorkDevContext();

	// ① 旧シーンを今すぐ凍結（m_rtTexには直前フレームの画面が入っている）
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ctx->PSSetShaderResources(0, 1, &nullSRV);
	ctx->CopyResource(m_rtTexTransition.Get(), m_rtTex.Get());


	m_prevEffect = m_currentEffect;
	m_pendingEffect = newEffect;
	m_pendingOnSwitch = onSwitch;    // コールバックを保存（まだ呼ばない）
	m_currentTransition = transitionKey;
	m_transitionDuration = duration;
	m_transitionProgress = 0.0f;
	m_isTransitioning = true;
	m_sceneSwitched = false;       // 初回Render()でキャプチャさせる
	m_pendingCapture = true;        // キャプチャ待ちフラグ
	m_wipeMode = true;
	m_switchFired = false;
}