// kpi-render - kpiadx.kpi をロードして曲を raw PCM に書き出す (検証用ホスト)
//
// usage: kpi-render [-k PLUGIN] [-t TRACK] [-n MAXFRAMES] file > out.raw
//        kpi-render [-k PLUGIN] [-t TRACK] --info file
//        kpi-render [-k PLUGIN] [--settle FRAMES] --seek-check file...
//
// 1 つ目は TRACK 番目の曲を先頭から Render() して stdout に書き出す。
//   stdout: int16 little-endian、チャンネルはインターリーブ
//   stderr: "frames=N finished=0|1"
//   ループする曲は終わらないので、MAXFRAMES (既定 10000000) で止める。
// --info は曲数、KPI_MEDIAINFO、Select() で設定されたタグを UTF-8 で stdout に出す。
// --seek-check は全曲について、いろいろな位置にシークしてから半端な長さで Render() した
//   結果が、先頭から続けて Render() した結果と一致するかを確かめる。
//   kpiadx は途中へのシークで ADX の履歴を 0 から始めるので、シーク直後の FRAMES
//   (既定 8192) フレームだけは違ってよい。先頭と 2 周目以降のループ開始位置は
//   1 周目の状態に戻すので、最初から一致しなければならない。
// PLUGIN の既定値は kpi-render.exe と同じディレクトリの kpiadx.kpi。

#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <fcntl.h>
#include <io.h>
#include <map>
#include <string>
#include <vector>

#include "kpi.h"
#include "kpi_decoder.h"
#include "kpi_impl.h"
#include "kpi_mingw_uuid.h"

namespace
{
    class MemoryFile : public KbKpiFileImpl
    {
    public:
        explicit MemoryFile(std::vector<BYTE> data) : data_(std::move(data)) {}

        DWORD WINAPI Read(void* pBuffer, DWORD dwSize) override
        {
            if (pos_ >= data_.size())
                return 0;
            const DWORD bytes = static_cast<DWORD>(std::min<UINT64>(dwSize, data_.size() - pos_));
            std::memcpy(pBuffer, data_.data() + pos_, bytes);
            pos_ += bytes;
            return bytes;
        }
        UINT64 WINAPI Seek(INT64 i64Pos, DWORD dwOrigin) override
        {
            const INT64 base = dwOrigin == FILE_BEGIN ? 0 : dwOrigin == FILE_CURRENT ? static_cast<INT64>(pos_) : static_cast<INT64>(data_.size());
            if (base + i64Pos < 0 || static_cast<UINT64>(base + i64Pos) > data_.size())
                return KPI_FILE_EOF;
            pos_ = static_cast<UINT64>(base + i64Pos);
            return pos_;
        }
        UINT64 WINAPI GetSize() override { return data_.size(); }

    private:
        std::vector<BYTE> data_;
        UINT64 pos_ = 0;
    };

    // Select() で設定されたタグを集める。本体のタグ取得ルーチンは持たない
    class TagInfo : public KbKpiTagInfoImpl
    {
    public:
        std::map<std::wstring, std::wstring> values;

        DWORD WINAPI GetTagInfo(IKpiFile*, IKpiFolder*, DWORD, DWORD) override { return 0; }
        DWORD WINAPI GetValue(const wchar_t* cszName, wchar_t* pszValue, int nSize) override
        {
            const auto found = values.find(cszName);
            if (found == values.end() || nSize <= 0)
                return 0;
            wcsncpy_s(pszValue, static_cast<size_t>(nSize) / sizeof(wchar_t), found->second.c_str(), _TRUNCATE);
            return static_cast<DWORD>(found->second.size());
        }
        void WINAPI SetOverwrite(BOOL) override {}
        void WINAPI SetPicture(DWORD, const wchar_t*, const wchar_t*, const wchar_t*, DWORD, DWORD, const BYTE*, DWORD) override {}

        void WINAPI aSetValueA(const char* cszName, int nNameSize, const char* cszValue, int nValueSize) override
        { set(widen(CP_ACP, cszName, nNameSize), widen(CP_ACP, cszValue, nValueSize)); }
        void WINAPI aSetValueW(const char* cszName, int nNameSize, const wchar_t* cszValue, int nValueSize) override
        { set(widen(CP_ACP, cszName, nNameSize), wide(cszValue, nValueSize)); }
        void WINAPI aSetValueU8(const char* cszName, int nNameSize, const char* cszValue, int nValueSize) override
        { set(widen(CP_ACP, cszName, nNameSize), widen(CP_UTF8, cszValue, nValueSize)); }
        void WINAPI wSetValueA(const wchar_t* cszName, int nNameSize, const char* cszValue, int nValueSize) override
        { set(wide(cszName, nNameSize), widen(CP_ACP, cszValue, nValueSize)); }
        void WINAPI wSetValueW(const wchar_t* cszName, int nNameSize, const wchar_t* cszValue, int nValueSize) override
        { set(wide(cszName, nNameSize), wide(cszValue, nValueSize)); }
        void WINAPI wSetValueU8(const wchar_t* cszName, int nNameSize, const char* cszValue, int nValueSize) override
        { set(wide(cszName, nNameSize), widen(CP_UTF8, cszValue, nValueSize)); }
        void WINAPI u8SetValueA(const char* cszName, int nNameSize, const char* cszValue, int nValueSize) override
        { set(widen(CP_UTF8, cszName, nNameSize), widen(CP_ACP, cszValue, nValueSize)); }
        void WINAPI u8SetValueW(const char* cszName, int nNameSize, const wchar_t* cszValue, int nValueSize) override
        { set(widen(CP_UTF8, cszName, nNameSize), wide(cszValue, nValueSize)); }
        void WINAPI u8SetValueU8(const char* cszName, int nNameSize, const char* cszValue, int nValueSize) override
        { set(widen(CP_UTF8, cszName, nNameSize), widen(CP_UTF8, cszValue, nValueSize)); }

        UINT64 number(const wchar_t* name, UINT64 fallback) const
        {
            const auto found = values.find(name);
            return found == values.end() ? fallback : _wcstoui64(found->second.c_str(), nullptr, 10);
        }

    private:
        void set(std::wstring name, std::wstring value) { values[std::move(name)] = std::move(value); }

        // nSize は -1 なら NUL 終端、そうでなければバイト数
        static std::wstring wide(const wchar_t* text, int nSize)
        {
            if (text == nullptr)
                return {};
            return nSize < 0 ? std::wstring(text) : std::wstring(text, wcsnlen(text, static_cast<size_t>(nSize) / sizeof(wchar_t)));
        }
        static std::wstring widen(UINT codepage, const char* text, int nSize)
        {
            if (text == nullptr)
                return {};
            const int bytes = nSize < 0 ? static_cast<int>(std::strlen(text)) : static_cast<int>(strnlen(text, static_cast<size_t>(nSize)));
            const int length = MultiByteToWideChar(codepage, 0, text, bytes, nullptr, 0);
            std::wstring result(static_cast<size_t>(length), L'\0');
            MultiByteToWideChar(codepage, 0, text, bytes, result.data(), length);
            return result;
        }
    };

    void usage()
    {
        std::fputs("usage: kpi-render [-k PLUGIN] [-t TRACK] [-n MAXFRAMES] file > out.raw\n"
                   "       kpi-render [-k PLUGIN] [-t TRACK] --info file\n"
                   "       kpi-render [-k PLUGIN] [--settle FRAMES] --seek-check file...\n", stderr);
        std::exit(2);
    }

    bool readFile(const wchar_t* path, std::vector<BYTE>& data)
    {
        FILE* fp = _wfopen(path, L"rb");
        if (!fp)
            return false;
        BYTE buf[65536];
        size_t bytes;
        while ((bytes = std::fread(buf, 1, sizeof(buf), fp)) > 0)
            data.insert(data.end(), buf, buf + bytes);
        std::fclose(fp);
        return true;
    }

    // 曲数を返す。失敗したら 0
    DWORD openDecoder(IKpiDecoderModule* module, const wchar_t* path, IKpiDecoder** ppDecoder)
    {
        *ppDecoder = nullptr;
        std::vector<BYTE> data;
        if (!readFile(path, data))
        {
            std::fwprintf(stderr, L"%ls: cannot read\n", path);
            return 0;
        }
        auto* file = new MemoryFile(std::move(data));

        KPI_MEDIAINFO request;
        kpi_InitMediaInfo(&request);
        const DWORD count = module->Open(&request, file, nullptr, ppDecoder);
        file->Release();
        if (count == 0 || *ppDecoder == nullptr)
        {
            std::fwprintf(stderr, L"%ls: cannot open\n", path);
            return 0;
        }
        return count;
    }

    const KPI_MEDIAINFO* selectTrack(IKpiDecoder* decoder, const wchar_t* path, DWORD track, TagInfo* tags)
    {
        const KPI_MEDIAINFO* info = nullptr;
        if (decoder->Select(track, &info, tags, KPI_TAGGET_FLAG_ALL) != track || info == nullptr)
        {
            std::fwprintf(stderr, L"%ls: cannot select track %u\n", path, track);
            return nullptr;
        }
        if (info->nBitsPerSample != 16 || info->dwChannels == 0)
        {
            std::fwprintf(stderr, L"%ls: unexpected format %u ch %d bit\n", path, info->dwChannels, info->nBitsPerSample);
            return nullptr;
        }
        return info;
    }

    // dwFrames フレームまで Render() する。返り値は書けたフレーム数
    DWORD renderFrames(IKpiDecoder* decoder, DWORD channels, std::vector<int16_t>& out, DWORD dwFrames)
    {
        out.resize(static_cast<size_t>(dwFrames) * channels);
        DWORD done = 0;
        while (done < dwFrames)
        {
            const DWORD request = std::min<DWORD>(dwFrames - done, 65536);
            const DWORD rendered = decoder->Render(reinterpret_cast<BYTE*>(out.data() + static_cast<size_t>(done) * channels), request);
            done += rendered;
            if (rendered < request)
                break;
        }
        out.resize(static_cast<size_t>(done) * channels);
        return done;
    }

    void printUtf8(const std::wstring& text)
    {
        const int bytes = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
        std::string utf8(static_cast<size_t>(bytes), '\0');
        WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), utf8.data(), bytes, nullptr, nullptr);
        std::fwrite(utf8.data(), 1, utf8.size(), stdout);
    }

    int info(IKpiDecoderModule* module, const wchar_t* path, DWORD track)
    {
        IKpiDecoder* decoder = nullptr;
        const DWORD count = openDecoder(module, path, &decoder);
        if (count == 0)
            return 1;
        TagInfo tags;
        const KPI_MEDIAINFO* media = selectTrack(decoder, path, track, &tags);
        if (media == nullptr)
        {
            decoder->Release();
            return 1;
        }
        _setmode(_fileno(stdout), _O_BINARY);
        std::printf("count=%lu\nnumber=%lu\nrate=%lu\nchannels=%lu\nbits=%d\nlength=%llu\nloop=%llu\nseekable=%lu\n",
            count, media->dwNumber, media->dwSampleRate, media->dwChannels, media->nBitsPerSample,
            media->qwLength, media->qwLoop, media->dwSeekableFlags);
        for (const auto& [name, value] : tags.values)
            printUtf8(L"tag." + name + L"=" + value + L"\n");
        decoder->Release();
        return 0;
    }

    int render(IKpiDecoderModule* module, const wchar_t* path, DWORD track, DWORD maxframes)
    {
        IKpiDecoder* decoder = nullptr;
        if (openDecoder(module, path, &decoder) == 0)
            return 1;
        const KPI_MEDIAINFO* media = selectTrack(decoder, path, track, nullptr);
        if (media == nullptr)
        {
            decoder->Release();
            return 1;
        }

        _setmode(_fileno(stdout), _O_BINARY);
        const DWORD channels = media->dwChannels;
        std::vector<int16_t> buf;
        DWORD frames = 0;
        bool finished = false;
        while (frames < maxframes)
        {
            const DWORD request = std::min<DWORD>(maxframes - frames, 65536);
            const DWORD rendered = renderFrames(decoder, channels, buf, request);
            std::fwrite(buf.data(), sizeof(int16_t), buf.size(), stdout);  // x86/x64 はリトルエンディアン
            frames += rendered;
            if (rendered < request)
            {
                finished = true;
                break;
            }
        }
        std::fflush(stdout);
        std::fprintf(stderr, "frames=%lu finished=%d\n", frames, finished ? 1 : 0);
        decoder->Release();
        return 0;
    }

    int seekCheckTrack(IKpiDecoder* decoder, const wchar_t* path, DWORD track, DWORD settle)
    {
        TagInfo tags;
        const KPI_MEDIAINFO* media = selectTrack(decoder, path, track, &tags);
        if (media == nullptr)
            return 1;
        const DWORD channels = media->dwChannels;
        const UINT64 total = tags.number(L"ADX_TotalSamples", 0);
        const bool looped = tags.values[L"ADX_IsLooped"] == L"True";
        const UINT64 loop_start = tags.number(L"ADX_LoopStartSample", 0);
        const UINT64 loop_end = tags.number(L"ADX_LoopEndSample", 0);
        const UINT64 loop_length = loop_end - loop_start;
        if (total == 0 || (looped && loop_start >= loop_end))
        {
            std::fwprintf(stderr, L"%ls #%u: no length or loop points in the tags\n", path, track);
            return 1;
        }

        // 基準: 先頭から続けて Render() した結果。ループする曲は 3 周目の途中まで
        const UINT64 ref_frames = looped ? loop_end + loop_length * 2 + 1000 : total + 1000;
        std::vector<int16_t> ref;
        decoder->Seek(0, KPI_MEDIAINFO::SEEK_FLAGS_SAMPLE);
        const DWORD rendered = renderFrames(decoder, channels, ref, static_cast<DWORD>(std::min<UINT64>(ref_frames, MAXDWORD)));
        if (!looped && rendered != total)
        {
            std::fwprintf(stderr, L"%ls #%u: rendered %u frames, expected %llu\n", path, track, rendered, total);
            return 1;
        }
        if (looped && rendered != ref_frames)
        {
            std::fwprintf(stderr, L"%ls #%u: looped song ended at %u frames\n", path, track, rendered);
            return 1;
        }

        std::vector<UINT64> positions = { 0, 1, 31, 32, 33, 1000, total / 2, total - 1, total, total + 500 };
        if (looped)
        {
            // 2 周目以降のループ開始位置 (loop_end, loop_end + loop_length) は 1 周目と同じ状態から始まる
            positions = { 0, 1, 33, 1000, loop_start, loop_start + 1, loop_end - 1, loop_end, loop_end + 7,
                          loop_end + loop_length, loop_end + loop_length + 1, loop_end + loop_length * 2 - 1 };
        }

        int fails = 0;
        for (const UINT64 pos : positions)
        {
            const UINT64 expect = (!looped && pos > total) ? total : pos;
            const UINT64 got = decoder->Seek(pos, KPI_MEDIAINFO::SEEK_FLAGS_SAMPLE);
            if (got != expect)
            {
                std::fwprintf(stderr, L"%ls #%u: Seek(%llu) returned %llu, expected %llu\n", path, track, pos, got, expect);
                fails++;
                continue;
            }

            // 半端な長さで Render() して、基準の続きと比べる
            static constexpr DWORD sizes[] = { 1, 333, 32, 7, 4096, 50 };
            const size_t compare = got < rendered ? static_cast<size_t>(rendered - got) : 0;
            std::vector<int16_t> out;
            std::vector<int16_t> buf;
            for (size_t k = 0; out.size() / channels < compare; k++)
            {
                const DWORD request = sizes[k % std::size(sizes)];
                buf.resize(static_cast<size_t>(request) * channels);
                const DWORD frames = decoder->Render(reinterpret_cast<BYTE*>(buf.data()), request);
                out.insert(out.end(), buf.begin(), buf.begin() + static_cast<size_t>(frames) * channels);
                if (frames < request)
                    break;
            }
            if (out.size() / channels < compare)
            {
                std::fwprintf(stderr, L"%ls #%u: Render after Seek(%llu) ended at %zu frames, expected %zu\n",
                    path, track, pos, out.size() / channels, compare);
                fails++;
                continue;
            }
            // ループしない曲は終わった後に何も出さない
            if (!looped && out.size() / channels > compare)
            {
                std::fwprintf(stderr, L"%ls #%u: Render after Seek(%llu) went beyond the end\n", path, track, pos);
                fails++;
                continue;
            }

            // 先頭と 2 周目以降のループ開始位置は最初から一致する。それ以外は履歴が落ち着くまで待つ
            const bool exact = pos == 0 || (looped && pos >= loop_end && (pos - loop_start) % loop_length == 0);
            size_t last_diff = 0;   // 最後に違ったフレーム + 1
            for (size_t frame = 0; frame < compare; frame++)
            {
                if (std::memcmp(&out[frame * channels], &ref[(static_cast<size_t>(got) + frame) * channels], channels * sizeof(int16_t)) != 0)
                    last_diff = frame + 1;
            }
            if (last_diff > (exact ? 0 : settle))
            {
                std::fwprintf(stderr, L"%ls #%u: PCM after Seek(%llu) differs until frame %zu\n", path, track, pos, last_diff);
                fails++;
            }
        }
        std::fwprintf(stderr, L"%ls #%u: %ls (%llu frames%ls)\n", path, track, fails ? L"NG" : L"ok", total, looped ? L", looped" : L"");
        return fails ? 1 : 0;
    }

    int seekCheck(IKpiDecoderModule* module, const wchar_t* path, DWORD settle)
    {
        IKpiDecoder* decoder = nullptr;
        const DWORD count = openDecoder(module, path, &decoder);
        if (count == 0)
            return 1;
        int ret = 0;
        for (DWORD track = 1; track <= count; track++)
            ret |= seekCheckTrack(decoder, path, track, settle);
        decoder->Release();
        return ret;
    }

    std::wstring defaultPluginPath()
    {
        wchar_t path[MAX_PATH];
        const DWORD len = GetModuleFileNameW(nullptr, path, MAX_PATH);
        std::wstring dir(path, len);
        return dir.substr(0, dir.find_last_of(L"\\/") + 1) + L"kpiadx.kpi";
    }
}

int wmain(int argc, wchar_t** argv)
{
    std::wstring plugin = defaultPluginPath();
    DWORD track = 1;
    DWORD maxframes = 10000000;
    DWORD settle = 8192;
    bool seek_check = false;
    bool show_info = false;
    std::vector<const wchar_t*> files;

    for (int i = 1; i < argc; i++)
    {
        if (!std::wcscmp(argv[i], L"-k") && i + 1 < argc) plugin = argv[++i];
        else if (!std::wcscmp(argv[i], L"-t") && i + 1 < argc) track = static_cast<DWORD>(_wtoi(argv[++i]));
        else if (!std::wcscmp(argv[i], L"-n") && i + 1 < argc) maxframes = static_cast<DWORD>(_wtoi(argv[++i]));
        else if (!std::wcscmp(argv[i], L"--settle") && i + 1 < argc) settle = static_cast<DWORD>(_wtoi(argv[++i]));
        else if (!std::wcscmp(argv[i], L"--seek-check")) seek_check = true;
        else if (!std::wcscmp(argv[i], L"--info")) show_info = true;
        else if (argv[i][0] != L'-') files.push_back(argv[i]);
        else usage();
    }
    if (files.empty() || (!seek_check && files.size() != 1) || (seek_check && show_info) || track == 0)
        usage();

    HMODULE dll = LoadLibraryW(plugin.c_str());
    const auto create = dll ? reinterpret_cast<pfn_kpiCreateInstance>(GetProcAddress(dll, "kpi_CreateInstance")) : nullptr;
    IKpiDecoderModule* module = nullptr;
    if (!create || create(IID_IKpiDecoderModule, reinterpret_cast<void**>(&module), nullptr) != S_OK)
    {
        std::fwprintf(stderr, L"%ls: cannot load plugin\n", plugin.c_str());
        return 1;
    }

    int ret = 0;
    if (seek_check)
    {
        for (const wchar_t* file : files)
            ret |= seekCheck(module, file, settle);
    }
    else if (show_info)
        ret = info(module, files[0], track);
    else
        ret = render(module, files[0], track, maxframes);

    module->Release();
    FreeLibrary(dll);
    return ret;
}
