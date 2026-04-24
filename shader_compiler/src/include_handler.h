#ifndef RENDER_ENGINE_INCLUDE_HANDLER_H
#define RENDER_ENGINE_INCLUDE_HANDLER_H

struct IncludeHandler : public IDxcIncludeHandler
{
    IncludeHandler(std::filesystem::path basePath, IDxcUtils* utils) :
        m_BasePath(std::move(basePath)),
        m_Utils(utils),
        m_Ref(1)
    {
    }

    HRESULT STDMETHODCALLTYPE LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource) override
    {
        CComPtr<IDxcBlobEncoding> pEncoding;

        HRESULT result;

        const std::wstring fullPath = (m_BasePath / pFilename).wstring();
        if (m_IncludedFiles.contains(fullPath))
        {
            constexpr char nullStr[] = " ";
            m_Utils->CreateBlobFromPinned(nullStr, sizeof(nullStr), DXC_CP_ACP, &pEncoding);
            result = S_OK;
        }
        else
        {
            m_IncludedFiles.insert(fullPath);
            result = m_Utils->LoadFile(fullPath.c_str(), nullptr, &pEncoding);
        }

        if (SUCCEEDED(result))
            *ppIncludeSource = pEncoding.Detach();

        return result;
    }

    void ExtractIncludePaths(std::unordered_map<std::string, std::filesystem::file_time_type::duration::rep>& relativeIncludePaths)
    {
        for (const std::wstring& path : m_IncludedFiles)
        {
            const std::filesystem::path relativePath = std::filesystem::relative(path, m_BasePath);
            const std::filesystem::file_time_type::duration::rep lastWriteTime = std::filesystem::last_write_time(path).time_since_epoch().count();
            relativeIncludePaths[relativePath.string()] = lastWriteTime;
        }

        m_IncludedFiles.clear();
    }

    ULONG STDMETHODCALLTYPE AddRef()  override
    {
        return ++m_Ref;
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        const ULONG r = --m_Ref;
        if (r == 0)
            delete this;
        return r;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
    {
        if (riid == __uuidof(IDxcIncludeHandler))
        {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

private:
    std::unordered_set<std::wstring> m_IncludedFiles;
    std::filesystem::path m_BasePath;
    IDxcUtils* m_Utils;
    std::atomic<ULONG> m_Ref;
};

inline void WriteIncludeFiles(const std::filesystem::path& outputDirPath, const std::unordered_map<std::string, std::filesystem::file_time_type::duration::rep>& includePaths)
{
    const std::string json = nlohmann::json(includePaths).dump();

    const std::filesystem::path outputPath = std::filesystem::absolute(outputDirPath / "dependencies.json");
    std::filesystem::create_directories(outputPath.parent_path());

    FILE* fp = fopen(outputPath.string().c_str(), "w");
    fwrite(json.c_str(), json.size(), 1, fp);
    fclose(fp);
}

#endif