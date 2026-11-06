# kpiadx.kpi

CRI ADX の音声を再生する KbMedia Player のデコーダプラグイン。

- ADX (v3 / v4、エンコードタイプ 2 / 3 / 4、1〜8ch)
- AAX (ADX のセグメントを @UTF の表でつないだもの)
- AFS アーカイブ (1 ファイル複数曲。ファイル名表があれば曲名と日時を出す)

AHX と暗号化 ADX には対応しない。利用者向けの説明は `kpiadx.txt` (Shift_JIS / CRLF) にある。

## ファイル

- `KpiADXDecoder.*` — デコーダー本体 (ADX のデコード、AAX のセグメント、AFS の曲、ループ、シーク)
- `KpiADXDecoderModule.*`、`kpiadx.cpp`、`kpiadx.def` — KbMedia Player とのやりとり
- `adx.h` — ADX / AFS のヘッダの構造体
- `aax.h` — AAX (@UTF) のパーサー
- `KpiPartialFile.h` — AFS の 1 曲を IKpiFile として見せる
- `kpisdk/` — KbMedia Player プラグイン SDK のヘッダ (v3.17beta3 用、原本のまま)
- `kpi_mingw_uuid.h` — MinGW でビルドするときの `__uuidof` 用シム (MSVC では空)
- `tools/kpi-render.cpp` — 検証用ホスト。ビルドした .kpi をロードして PCM やタグを書き出す
- `tools/fixtures.py` — テスト用の ADX / AAX / AFS を合成し、kpi-render の出力を照合する

## ビルド

リリースビルドは Windows の MSVC (Visual Studio 2026) で行う。

```
cmake -S . -B build-x64 -G "Visual Studio 18 2026" -A x64
cmake --build build-x64 --config Release
cmake -S . -B build-x86 -G "Visual Studio 18 2026" -A Win32
cmake --build build-x86 --config Release
```

`build-*/bin/Release/kpiadx.kpi` ができる。KbMedia Player 本体付属のプラグインと同じく、
VC++ ランタイムは動的リンク (`/MD`) なので、Visual Studio 2026 に対応した VC++ 再頒布可能パッケージが要る。

Visual Studio で `kpiadx.sln` を開いてビルドすることもできる。`kpiadx.vcxproj` と
`tools/kpi-render.vcxproj` は CMakeLists.txt と同じ設定にしてあり、`bin\<Platform>\<Configuration>\` に
`kpiadx.kpi` と `kpi-render.exe` ができる。設定を変えるときは CMakeLists.txt と両方を直すこと。
テストは CMake でビルドしたときだけ使える。

### MinGW (開発中の確認用)

Linux 上で MinGW (gcc または clang) でクロスビルドし、Wine で確かめることもできる。

```
cmake -S . -B build-mingw -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DCMAKE_CROSSCOMPILING_EMULATOR=wine
cmake --build build-mingw
```

clang なら `-DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_COMPILER_TARGET=x86_64-w64-mingw32` とする。

## 検証

```
cd build-x64
ctest -C Release
```

- `make-fixtures` — ADX (v3 / v4、タイプ 2 / 3 / 4、1 / 2 / 6ch、挿入サンプルあり、ループ情報の欄なし)、
  2 セグメントの AAX、名前表つきの AFS を合成する (Python 3 が要る)
- `check-fixtures` — 合成した曲のタグと PCM を、`tools/fixtures.py` の参照デコーダーの結果と照合する
- `seek-check-fixtures` — いろいろな位置にシークしてから半端な長さで Render した結果が、
  先頭から続けて Render した結果と一致するかを確かめる。途中へのシークでは ADX の履歴を 0 から
  始めるので、シーク直後の 4096 フレームまでは違ってよい。先頭と 2 周目以降のループ開始位置は
  最初から一致しなければならない
- `seek-check-data` — `-DKPIADX_TEST_DATA=<ディレクトリ>` を指定したときだけ、その中の
  ADX / AAX / AFS で seek-check を行う

クロスビルドでは、`CMAKE_CROSSCOMPILING_EMULATOR` (wine など) を指定したときだけテストを登録する。

kpi-render は単体でも使える。

```
kpi-render [-k PLUGIN] [-t TRACK] [-n MAXFRAMES] file > out.raw   # int16le、チャンネルはインターリーブ
kpi-render [-k PLUGIN] [-t TRACK] --info file                     # 曲数、KPI_MEDIAINFO、タグ
kpi-render [-k PLUGIN] [--settle FRAMES] --seek-check file...
```

`PLUGIN` の既定値は kpi-render と同じディレクトリの `kpiadx.kpi`。

## 配布

GitHub Actions (`.github/workflows/build.yml`) が x64 / x86 を MSVC でビルドしてテストし、
`kpiadx.kpi`、`kpiadx.txt`、`COPYING` をまとめた zip を作る。`v*` のタグを push すると、
その zip を添付したドラフトのリリースができる。

## ライセンス

MIT ライセンス。`COPYING` を参照。
