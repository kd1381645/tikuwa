#pragma once

#include <xaudio2.h>
#include <x3daudio.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>


// --- ライブラリ ---
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "xaudio2.lib")

//============================================================
// カテゴリ(=どのボリュームバス/サブミックスに送るか)
//============================================================
enum class SoundCategory
{
	BGM,
	SE,
};

//============================================================
// SoundData : デコード済みPCM。キャッシュ対象。
//============================================================
class SoundData
{
public:
	bool Load(const std::wstring& path);

	const WAVEFORMATEX* GetFormat() const
	{
		return reinterpret_cast<const WAVEFORMATEX*>(m_format.data());
	}
	const BYTE* GetData() const { return m_buffer.data(); }
	UINT32      GetSize() const { return static_cast<UINT32>(m_buffer.size()); }

	UINT32 GetSampleRate() const { return GetFormat()->nSamplesPerSec; }

	float GetDurationSec() const
	{
		const WAVEFORMATEX* w = GetFormat();
		if (w->nAvgBytesPerSec == 0) return 0.0f;
		return static_cast<float>(m_buffer.size()) / static_cast<float>(w->nAvgBytesPerSec);
	}

private:
	std::vector<BYTE> m_format;
	std::vector<BYTE> m_buffer;
};

//============================================================
// VoicePool : ソースボイスの使い回しプール
//  XAudio2のソースボイスはフォーマット固定なので、
//  フォーマット(ch/レート/bit/tag)でキー分けして管理する。
//============================================================
class VoicePool
{
public:
	void Init(IXAudio2* xaudio) { m_xaudio = xaudio; }

	IXAudio2SourceVoice* Acquire(const WAVEFORMATEX* fmt);
	void Release(IXAudio2SourceVoice* voice, const WAVEFORMATEX* fmt);
	void Clear();   // 終了時に全idleボイスを破棄

	int GetCreatedCount() const { return m_created; }

private:
	static uint64_t MakeKey(const WAVEFORMATEX* f)
	{
		uint64_t k = 0;
		k |= static_cast<uint64_t>(f->nSamplesPerSec);
		k |= static_cast<uint64_t>(f->nChannels) << 32;
		k |= static_cast<uint64_t>(f->wBitsPerSample) << 48;
		k |= static_cast<uint64_t>(f->wFormatTag) << 56;
		return k;
	}

	IXAudio2* m_xaudio = nullptr;
	std::unordered_map<uint64_t, std::vector<IXAudio2SourceVoice*>> m_idle;
	int m_created = 0;
};

//============================================================
// SoundInstance : 再生中の1音
//============================================================
class SoundInstance
{
public:
	SoundInstance(VoicePool* pool,
		IXAudio2SubmixVoice* destSubmix,
		std::shared_ptr<SoundData> data,
		SoundCategory category,
		bool loop,
		const std::wstring& key);
	~SoundInstance();

	void Play();
	void Stop();
	void Pause();
	void Resume();

	// --- 音量 ---
	void  SetVolume(float v);
	float GetVolume() const { return m_volume; }

	// --- フェード ---
	void FadeTo(float targetVolume, float seconds);
	void FadeIn(float seconds, float targetVolume = 1.0f);
	void FadeOut(float seconds, bool stopAtEnd = true);

	// --- ピッチ ---
	void  SetPitch(float ratio);
	float GetPitch() const { return m_pitch; }

	// --- ループ ---
	void ExitLoop();
	bool IsLoop() const { return m_loop; }

	// --- 再生時間 ---
	float GetPlayTimeSec() const;
	float GetDurationSec() const;

	// --- 3D ---
	void EnableEmitter(const X3DAUDIO_VECTOR& pos, float distanceScaler);
	void SetPosition(const X3DAUDIO_VECTOR& pos);
	void SetVelocity(const X3DAUDIO_VECTOR& vel);
	bool Is3D() const { return m_is3D; }

	SoundCategory       GetCategory() const { return m_category; }
	const std::wstring& GetKey()      const { return m_key; }
	bool IsPlaying() const;

	// --- AudioManager から毎フレーム呼ばれる ---
	void UpdateFade(float dt);
	void Calculate3D(const X3DAUDIO_HANDLE& engine,
		const X3DAUDIO_LISTENER& listener,
		UINT32 dstChannels,
		std::vector<float>& matrixScratch);

	SoundInstance(const SoundInstance&) = delete;
	SoundInstance& operator=(const SoundInstance&) = delete;

private:
	void ApplyFrequency();

	VoicePool* m_pool = nullptr;   // 解体時にここへ返却
	IXAudio2SourceVoice* m_voice = nullptr;
	IXAudio2SubmixVoice* m_destSubmix = nullptr;
	std::shared_ptr<SoundData> m_data;
	SoundCategory              m_category = SoundCategory::SE;
	std::wstring               m_key;                    // 再生元パス(同時数カウント等に使用)
	bool  m_loop = false;

	float m_volume = 1.0f;
	float m_pitch = 1.0f;

	bool  m_fading = false;
	float m_fadeFrom = 1.0f;
	float m_fadeTo = 1.0f;
	float m_fadeElapsed = 0.0f;
	float m_fadeTime = 0.0f;
	bool  m_stopAfterFade = false;

	bool               m_is3D = false;
	X3DAUDIO_EMITTER   m_emitter = {};
	std::vector<float> m_azimuths;          // 多チャンネル音源用(stereo等)
	float              m_dopplerFreq = 1.0f;
};

//============================================================
// AudioManager
//============================================================
class AudioManager
{
public:
	static AudioManager& Instance()
	{
		static AudioManager inst;
		return inst;
	}

	bool Init();
	void Release();
	void Update(float deltaTime);

	// --- 再生 ---
	std::shared_ptr<SoundInstance> Play(
		const std::wstring& path,
		SoundCategory category = SoundCategory::SE,
		float volume = 1.0f,
		bool  loop = false);

	std::shared_ptr<SoundInstance> Play3D(
		const std::wstring& path,
		const X3DAUDIO_VECTOR& position,
		SoundCategory category = SoundCategory::SE,
		float volume = 1.0f,
		bool  loop = false,
		float distanceScaler = 10.0f);

	// --- サウンドバンク(論理名で再生) ---
	// bank は { "論理名": { "path":..., "category":..., ... }, ... } の形のjsonオブジェクト。
	// 既存のバンクに追記/上書きする(複数ファイルを合成できる)。
	void LoadBank(const nlohmann::json& bank);
	// ファイルから直接読む簡易版(ホットリロード無し)。
	void LoadBankFile(const std::string& path);

	// 論理名で再生(ピッチは pitchMin..pitchMax の乱数)
	std::shared_ptr<SoundInstance> Play(const std::string& name);
	std::shared_ptr<SoundInstance> Play3D(const std::string& name, const X3DAUDIO_VECTOR& position);

	// --- ボリューム(バス) ---
	void  SetMasterVolume(float v);
	void  SetBgmVolume(float v);
	void  SetSeVolume(float v);
	float GetMasterVolume() const { return m_masterVolume; }
	float GetBgmVolume()    const { return m_bgmVolume; }
	float GetSeVolume()     const { return m_seVolume; }

	// --- 停止 ---
	void StopAll();
	void StopAll(SoundCategory category);

	// --- 3Dリスナー ---
	void SetListener(const X3DAUDIO_VECTOR& pos,
		const X3DAUDIO_VECTOR& front,
		const X3DAUDIO_VECTOR& up,
		const X3DAUDIO_VECTOR& velocity = { 0.0f, 0.0f, 0.0f });

	// --- ボイス制限 / クールダウン ---
	void SetMaxVoices(int n);                       // 同時発音数の上限(既定64)
	// サウンドごとのポリシー: 最小再生間隔[秒] と 同一音の最大同時数(0=無制限)
	void SetSoundPolicy(const std::wstring& path, float cooldownSec, int maxInstances);

	// デバッグ用
	int GetActiveVoiceCount() const { return static_cast<int>(m_instances.size()); }
	int GetPooledVoiceCount() const { return m_pool.GetCreatedCount(); }

private:
	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;

	std::shared_ptr<SoundData> GetSoundData(const std::wstring& path);
	IXAudio2SubmixVoice* GetSubmix(SoundCategory c)
	{
		return (c == SoundCategory::BGM) ? m_bgmSubmix : m_seSubmix;
	}

	struct SoundPolicy
	{
		float cooldown = 0.0f;
		int   maxInstances = 0;
	};

	// サウンドバンクの1定義
	struct SoundDef
	{
		std::wstring  path;
		SoundCategory category = SoundCategory::SE;
		float         volume = 1.0f;
		float         pitchMin = 1.0f;
		float         pitchMax = 1.0f;
		bool          loop = false;
		float         cooldown = 0.0f;
		int           maxInstances = 0;
		float         distanceScaler = 10.0f;   // 3D用
	};

	// 再生枠を確保(クールダウン/同時数チェック+必要ならスティール)
	bool TryReserve(const std::wstring& key, const SoundPolicy& policy);
	// 撃ちっぱなしの古い音を1つ止めて枠を空ける
	bool StealOne(const std::wstring* sameKey);

	std::shared_ptr<SoundInstance> PlayDef(const SoundDef& def, bool use3D, const X3DAUDIO_VECTOR& pos);
	float RandRange(float a, float b);

	Microsoft::WRL::ComPtr<IXAudio2> m_xaudio;
	IXAudio2MasteringVoice* m_masterVoice = nullptr;
	IXAudio2SubmixVoice* m_bgmSubmix = nullptr;
	IXAudio2SubmixVoice* m_seSubmix = nullptr;

	VoicePool m_pool;

	float m_masterVolume = 1.0f;
	float m_bgmVolume = 1.0f;
	float m_seVolume = 1.0f;

	std::unordered_map<std::wstring, std::shared_ptr<SoundData>> m_cache;
	std::vector<std::shared_ptr<SoundInstance>>                  m_instances;

	std::unordered_map<std::wstring, SoundPolicy> m_policies;
	std::unordered_map<std::wstring, double>      m_lastPlay;   // 最後に鳴らした時刻
	std::unordered_map<std::string, SoundDef>     m_bank;       // 論理名 -> 定義
	std::mt19937 m_rng;                                          // ランダムピッチ用
	double m_time = 0.0;     // 累積時間(クールダウン判定用)
	int    m_maxVoices = 64;

	X3DAUDIO_HANDLE    m_x3dHandle = {};
	X3DAUDIO_LISTENER  m_listener = {};
	UINT32             m_deviceChannels = 2;
	std::vector<float> m_matrixScratch;

	bool m_coInitialized = false;
};