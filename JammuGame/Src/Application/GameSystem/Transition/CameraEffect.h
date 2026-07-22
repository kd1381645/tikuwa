#pragma once
#include "Const/EffectConst.h"
#include <d3d11.h>

class CameraEffect
{
public:

	void InitBuffer();
	void Update();

	void SetDreamPow(float _pow) { m_dreamPow = _pow; }

	void SetEffect(const std::string& key,
		const std::string& _Transitionkey = "none",
		float          duration = 1.0f);

	// カバー系（fadeblack）: 画面が完全に隠れた時点でシーン切替
	void StartCoverTransition(
		const std::string& transitionKey,
		float                 duration,
		std::function<void()> onSwitch,
		const std::string& newEffect = "none",
		float                 switchPoint = 0.5f);

	// ワイプ系: 旧シーンを凍結してから即座にシーン切替
	void StartWipeTransition(
		const std::string& transitionKey,
		float                 duration,
		std::function<void()> onSwitch,
		const std::string& newEffect = "none");


	const std::string& GetCurrentEffect() const { return m_currentEffect; }
	bool               IsTransitioning()  const { return m_isTransitioning; }

	void RenderFromSRV(ID3D11ShaderResourceView* inputSRV);


	void SetDepthSRV(ID3D11ShaderResourceView* srv) { m_currentDepthSRV = srv; }
	void SetNormalSRV(ID3D11ShaderResourceView* srv) { m_currentNormalSRV = srv; }
	void SetIdSRV(ID3D11ShaderResourceView* srv) { m_currentIdSRV = srv; }

private:

	void RegisterEffect(const std::string& _key, const void* bytecode, SIZE_T bytecodeSize);
	void RegisterTransitionEffect(const std::string& _key, const void* bytecode, SIZE_T bytecodeSize);

	void DrawFullscreenQuad(ID3D11PixelShader* ps,
		ID3D11ShaderResourceView* srv,
		ID3D11RenderTargetView* rtv,
		float                       blendFactor,
		bool                        useAlphaBlend);

	// レンダーターゲット用テクスチャ
	Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_rtTex;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_rtv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_backBufferTex;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRTV;

	// クロスフェード用中間テクスチャ
	Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_rtTexTransition;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_rtvTransition;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srvTransition;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_currentDepthSRV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_currentNormalSRV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_currentIdSRV = nullptr;

	//深度テクスチャ
	Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_depthTex;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_depthCopyTex;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    m_depthDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_depthSRV;

	// 深度キャプチャ用
	Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_depthSnapTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_depthSnapSRV;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_stateCB;

	// シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
	
	// ステート
	Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_sampler;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_raster;
	Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendStateOpaque; 
	Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendStateAlpha;
	Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	//ピクセルシェーダマップ
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> m_psMap;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> m_psTransitionMap;

	// エフェクト管理
	std::string m_currentEffect = "none";
	std::string m_currentTransition = "none";
	std::string m_prevEffect = "none";

	// 遷移管理
	float          m_transitionProgress = 0.0f;
	float          m_transitionDuration = 1.0f;
	bool           m_isTransitioning = false;

	float m_time = 0.0f;
	float m_dreamPow = 0.0f;

	//シーントランジション用
	bool m_sceneSwitched = false;
	std::function<void()> m_onSwitch = nullptr;
	bool                  m_switchFired = false;
	float                 m_switchPoint = 0.5f;
	std::string           m_pendingEffect = "none";
	bool                  m_pendingCapture = false;
	bool                  m_wipeMode = false;
	std::function<void()> m_pendingOnSwitch = nullptr;

	bool isInit = false;

//カメラエフェクトはすべてのシーンで使いたいためシングルトン化

private:

	CameraEffect() = default;
	~CameraEffect() = default;

	CameraEffect(const CameraEffect&) = delete;
	CameraEffect& operator=(const CameraEffect&) = delete;

public:

	static CameraEffect& Instance() 
	{
		static CameraEffect instance;
		return instance;
	}
};
