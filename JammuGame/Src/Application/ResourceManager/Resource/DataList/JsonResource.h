#pragma once
namespace fs = std::filesystem;

class JsonResource 
{
public:

    using json = nlohmann::json; 
    using onReloadCallBack = std::function<void(const json&)>;
    
    //生成時即ロード
    explicit JsonResource(std::string _filePath) :
        m_filePath(std::move(_filePath)),
        m_isDirty(false)
    {
        Reload();
    };

    JsonResource(const JsonResource&) = delete;
    JsonResource& operator=(const JsonResource&) = delete;
    
    void Poll()
    {
        try 
        {
            auto newTime = fs::last_write_time(m_filePath);
            if (newTime == m_lastWriteTime)return;

            Reload();
            m_isDirty.store(true);

            if (m_onReload) 
            {
                std::lock_guard lock(m_mutex);
                m_onReload(m_data);
            }
        }
        catch (...) {}
    }

    // JSON 全体を取得（コピーで返すためスレッドセーフ）
    json GetAll() const
    {
        std::lock_guard lock(m_mutex);
        return m_data;
    }

    // 指定キーの値を取得。存在しなければ空の json{} を返す
    json Get(const std::string& _key) const
    {
        std::lock_guard lock(m_mutex);
        auto it = m_data.find(_key);
        return it != m_data.end() ? *it : json{};
    }

    bool IsDirty() const {return m_isDirty.load(); }
    void ClearDirty() { m_isDirty.store(false); };

    //コールバック
    void SetOnReload(onReloadCallBack _cb) { m_onReload = std::move(_cb); }

    //メタ情報
    const std::string& GetFilePath() const { return m_filePath; }

private:
    
    //リロード
    void Reload() 
    {
        std::ifstream file(m_filePath);
        if (!file.is_open()) return;

        json tmp;
        try
        {
            file >> tmp; 
        }
        catch (const std::exception&)
        {
            return; 
        }

        std::lock_guard lock(m_mutex);
        m_data = std::move(tmp);
        m_lastWriteTime = std::filesystem::last_write_time(m_filePath);
    }

    std::string         m_filePath;
    json                m_data;
    mutable std::mutex  m_mutex;
    fs::file_time_type  m_lastWriteTime;
    std::atomic<bool>   m_isDirty;
    onReloadCallBack    m_onReload;
};