#!/usr/bin/env python3
"""kpiadx のテスト用の ADX/AAX/AFS を合成し、kpi-render の出力を照合する。

usage: fixtures.py generate DIR
       fixtures.py check DIR --cli KPI_RENDER [--emulator WINE]
       fixtures.py seek-check DIR --cli KPI_RENDER [--emulator WINE]
       fixtures.py seek-check-data DIR --cli KPI_RENDER [--emulator WINE]

generate は DIR に合成した曲と、その期待値 (expected.json、*.raw) を書き出す。
期待値はこのスクリプトの参照デコーダーで求める。参照デコーダーは kpiadx のコードを
使わず、次の決まりで曲を組み立てる。
  - ADX は先頭から履歴 0 でデコードし、先頭の挿入サンプルを捨てる
  - AAX は前のセグメントの最後の 2 サンプルを次のセグメントの履歴にする
  - ループする曲はループ終了位置からループ開始位置に戻り、1 周目と同じ PCM を繰り返す
check は kpi-render --info のタグと kpi-render の PCM を期待値と比べる。
seek-check は kpi-render --seek-check を全曲に対して実行する。
seek-check-data は DIR の下にある ADX/AAX/AFS をすべて探して kpi-render --seek-check にかける。
外部のモジュールは使わない (Python 3.8 以降の標準ライブラリだけ)。
"""

import argparse
import concurrent.futures
import hashlib
import json
import math
import os
import struct
import subprocess
import sys

ADX_TYPE2_COEFF = [(0x0000, 0x0000), (0x0F00, 0x0000), (0x1CC0, -0x0D00), (0x1880, -0x0DC0)]
OPTIONAL_TAGS = ("ADX_InsertedSamples", "ADX_LoopCount", "AAX_Segments", "Title", "AFS_FileDate")
SAMPLES_PER_BLOCK = 32      # 18 バイトのブロック、4 ビット
BLOCK_SIZE = 18
DATA_EXTENSIONS = (".adx", ".aax", ".afs")
# Windows のコマンドラインは 32767 文字までなので、ファイルをこの数ずつに分けて kpi-render に渡す。
# 分けた組は CPU の数だけ並列に動かす
SEEK_CHECK_BATCH = 8


def clip16(value):
    return max(-32768, min(32767, value))


def highpass_coeffs(highpass, rate):
    """kpiadx (と CRI の資料) と同じ式で予測係数を求める"""
    a = math.sqrt(2.0) - math.cos(2.0 * math.pi * (highpass / rate))
    b = math.sqrt(2.0) - 1.0
    c = (a - math.sqrt((a + b) * (a - b))) / b
    return int((c * 2.0) * 4096.0), int(-(c * c) * 4096.0)


# ---------------------------------------------------------------------------
# 合成

def make_signal(channels, frames, rate, seed):
    """チャンネルごとに周波数の違う正弦波とチャープを混ぜた int16 の PCM"""
    out = []
    for ch in range(channels):
        freq = 220.0 * (1.0 + 0.37 * ch + 0.11 * seed)
        samples = []
        for i in range(frames):
            t = i / rate
            value = 9000.0 * math.sin(2.0 * math.pi * freq * t)
            value += 5000.0 * math.sin(2.0 * math.pi * (freq * 3.1 + 400.0 * t) * t)
            value *= 0.6 + 0.4 * math.sin(2.0 * math.pi * 0.7 * t + ch)
            samples.append(clip16(int(value)))
        out.append(samples)
    return out


def predict(coeff, s0, s1):
    return (coeff[0] * s0 + coeff[1] * s1) >> 12


def encode_block(samples, coeff, history, encoding):
    """1 チャンネル分の 32 サンプルを 1 ブロックにする。history は [s0, s1] で更新される"""
    def quantize(coeff_pair, scale, hist):
        s0, s1 = hist
        nibbles = []
        error = 0
        for x in samples:
            pred = predict(coeff_pair, s0, s1)
            d = int(round((x - pred) / scale)) if scale else 0
            d = max(-8, min(7, d))
            s = clip16(d * scale + pred)
            error += (s - x) * (s - x)
            nibbles.append(d & 0x0F)
            s0, s1 = s, s0
        return error, nibbles, [s0, s1]

    def estimate_peak(coeff_pair):
        s0, s1 = history
        peak = 0
        for x in samples:
            peak = max(peak, abs(x - predict(coeff_pair, s0, s1)))
            s0, s1 = x, s0
        return peak

    if encoding == 2:
        best = None
        for index, coeff_pair in enumerate(ADX_TYPE2_COEFF):
            scale = min(0x1FFF, max(1, -(-estimate_peak(coeff_pair) // 7)))
            error, nibbles, hist = quantize(coeff_pair, scale, history)
            if best is None or error < best[0]:
                best = (error, (index << 13) | scale, nibbles, hist)
        _, raw_scale, nibbles, hist = best
    elif encoding == 4:
        peak = estimate_peak(coeff)
        exponent = 12
        while exponent > 0 and (1 << (12 - exponent)) * 7 < peak:
            exponent -= 1
        raw_scale = exponent
        _, nibbles, hist = quantize(coeff, 1 << (12 - exponent), history)
    else:
        raw_scale = min(0x1FFF, max(1, -(-estimate_peak(coeff) // 7)))
        _, nibbles, hist = quantize(coeff, raw_scale, history)

    history[:] = hist
    body = bytes((nibbles[i] << 4) | nibbles[i + 1] for i in range(0, SAMPLES_PER_BLOCK, 2))
    return struct.pack(">H", raw_scale) + body


def make_adx(pcm, rate, version=4, encoding=3, inserted=0, loop=None, loop_info=True, highpass=500):
    """pcm: チャンネルごとの int16 のリスト。loop: (開始, 終了) 挿入サンプルを含まない位置
    loop_info=False ならループ情報の欄がない短いヘッダにする"""
    channels = len(pcm)
    pcm = [[0] * inserted + list(samples) for samples in pcm]
    total = len(pcm[0])
    blocks = -(-total // SAMPLES_PER_BLOCK)
    coeff = highpass_coeffs(highpass, rate)

    if version == 3:
        loop_offset = 0x14
    else:
        loop_offset = 0x18 + max(8, 4 * channels)
    header_end = loop_offset + 0x18 if loop_info else 0x14
    data_offset = (header_end + 6 + 0x1F) & ~0x1F
    copyright_offset = data_offset - 4

    header = bytearray(data_offset)
    struct.pack_into(">HHBBBBIIHBB", header, 0, 0x8000, copyright_offset, encoding, BLOCK_SIZE, 4,
                     channels, rate, total, highpass, version, 0)
    if loop_info:
        looped = loop is not None
        begin = loop[0] + inserted if looped else 0
        end = loop[1] + inserted if looped else 0
        frame_bytes = BLOCK_SIZE * channels
        begin_byte = data_offset + (begin // SAMPLES_PER_BLOCK) * frame_bytes if looped else 0
        end_byte = data_offset + (-(-end // SAMPLES_PER_BLOCK)) * frame_bytes if looped else 0
        struct.pack_into(">HHIIIII", header, loop_offset, inserted, 1 if looped else 0, 1 if looped else 0,
                         begin, begin_byte, end, end_byte)
    header[copyright_offset - 2:copyright_offset + 4] = b"(c)CRI"

    data = bytearray()
    histories = [[0, 0] for _ in range(channels)]
    for block in range(blocks):
        for ch in range(channels):
            chunk = pcm[ch][block * SAMPLES_PER_BLOCK:(block + 1) * SAMPLES_PER_BLOCK]
            chunk += [0] * (SAMPLES_PER_BLOCK - len(chunk))
            data += encode_block(chunk, coeff, histories[ch], encoding)
    # 終端マーカー (デコーダーはサンプル数で終わりを決めるので読まない)
    data += struct.pack(">H", 0x8001) + bytes(BLOCK_SIZE - 2)
    return bytes(header) + bytes(data)


def make_aax(segments):
    """segments: [(adx バイト列, lpflg)] から @UTF の AAX を作る"""
    names = b"AAX\0data\0lpflg\0"
    columns = bytes([0x5B]) + struct.pack(">I", 4) + bytes([0x50]) + struct.pack(">I", 9)
    row_width = 8 + 1
    rows = bytearray()
    data = bytearray()
    for adx, lpflg in segments:
        rows += struct.pack(">IIB", len(data), len(adx), lpflg)
        data += adx
        data += bytes(-len(data) % 0x20)
    rows_offset = 0x18 + len(columns)
    strings_offset = rows_offset + len(rows)
    data_offset = (strings_offset + len(names) + 0x1F) & ~0x1F
    body = bytearray(struct.pack(">HHIIIHHI", 1, rows_offset, strings_offset, data_offset, 0,
                                 len(columns) // 5, row_width, len(segments)))
    body += columns + rows + names
    body += bytes(data_offset - len(body))
    body += data
    return b"@UTF" + struct.pack(">I", len(body)) + bytes(body)


def make_afs(members):
    """members: [(名前 bytes, 日時 (年, 月, 日, 時, 分, 秒), 中身)] から名前表つきの AFS を作る"""
    count = len(members)
    offset = (8 + 8 * (count + 1) + 0x7FF) & ~0x7FF
    items = []
    body = bytearray()
    for _, _, content in members:
        items.append((offset + len(body), len(content)))
        body += content
        body += bytes(-len(body) % 0x800)
    names_offset = offset + len(body)
    names = bytearray()
    for name, date, content in members:
        names += name.ljust(32, b"\0")[:32] + struct.pack("<6HI", *date, len(content))
    out = bytearray(b"AFS\0" + struct.pack("<I", count))
    for item in items:
        out += struct.pack("<II", *item)
    out += struct.pack("<II", names_offset, len(names))
    out += bytes(offset - len(out))
    out += body + names
    return bytes(out)


# ---------------------------------------------------------------------------
# 参照デコーダー

def parse_adx(data):
    sig, copyright_offset, encoding, block_size, bits, channels, rate, total, highpass, version, flags = \
        struct.unpack_from(">HHBBBBIIHBB", data, 0)
    assert sig == 0x8000 and data[copyright_offset - 2:copyright_offset + 4] == b"(c)CRI"
    info = dict(encoding=encoding, block_size=block_size, bits=bits, channels=channels, rate=rate,
                total=total, highpass=highpass, version=version, data_offset=copyright_offset + 4,
                has_loop_info=False, inserted=0, loop_count=0, loop_flag=0, loop_begin=0, loop_end=0)
    loop_offset = {3: 0x14, 4: 0x18 + max(8, 4 * channels)}.get(version, 0)
    if loop_offset and copyright_offset + 4 > loop_offset + 0x18:
        inserted, loop_count, loop_flag, begin, _, end, _ = struct.unpack_from(">HHIIIII", data, loop_offset)
        info.update(has_loop_info=True, inserted=inserted, loop_count=loop_count, loop_flag=loop_flag,
                    loop_begin=begin, loop_end=end)
    return info


def decode_adx(data, history=None):
    """ADX ストリームを全部デコードする。返り値はチャンネルごとのサンプル (挿入サンプルを含む)"""
    info = parse_adx(data)
    channels = info["channels"]
    coeff = highpass_coeffs(info["highpass"], info["rate"])
    spb = (info["block_size"] - 2) * 8 // info["bits"]
    blocks = -(-info["total"] // spb)
    hist = [list(h) for h in history] if history else [[0, 0] for _ in range(channels)]
    out = [[] for _ in range(channels)]
    for block in range(blocks):
        for ch in range(channels):
            pos = info["data_offset"] + (block * channels + ch) * info["block_size"]
            raw_scale = struct.unpack_from(">H", data, pos)[0]
            scale = raw_scale & 0x1FFF
            pair = coeff
            if info["encoding"] == 2:
                pair = ADX_TYPE2_COEFF[(raw_scale >> 13) & 3]
            elif info["encoding"] == 4:
                scale = 1 << (12 - scale)
            s0, s1 = hist[ch]
            for i in range(spb):
                byte = data[pos + 2 + i // 2]
                d = (byte >> 4) if i % 2 == 0 else (byte & 0x0F)
                if d & 8:
                    d -= 16
                s = clip16(d * scale + predict(pair, s0, s1))
                s0, s1 = s, s0
                out[ch].append(s)
            hist[ch] = [s0, s1]
    return [samples[:info["total"]] for samples in out], info


def song_adx(data):
    """ADX 1 本の曲: (チャンネルごとの PCM, ループ (開始, 終了) または None, タグ)"""
    pcm, info = decode_adx(data)
    inserted = info["inserted"]
    pcm = [samples[inserted:] for samples in pcm]
    if info["version"] == 3:
        looped = info["has_loop_info"] and (info["loop_count"] != 0 or info["loop_flag"] != 0)
    elif info["version"] == 4:
        looped = info["has_loop_info"] and info["loop_flag"] != 0
    else:
        looped = False
    loop = (info["loop_begin"] - inserted, info["loop_end"] - inserted) if looped else None
    tags = {"ADX_TotalSamples": str(len(pcm[0])), "ADX_Version": str(info["version"])}
    if info["has_loop_info"]:
        tags["ADX_InsertedSamples"] = str(inserted)
        tags["ADX_LoopCount"] = str(info["loop_count"])
    return pcm, loop, info, tags


def song_aax(segments):
    """segments: [(adx バイト列, lpflg)]"""
    pcm = None
    loop_start = loop_end = None
    position = 0
    info = None
    for adx, lpflg in segments:
        history = None
        if pcm is not None:
            history = [[samples[-1], samples[-2] if len(samples) >= 2 else samples[-1]] for samples in pcm]
        seg_pcm, seg_info = decode_adx(adx, history)
        info = info or seg_info
        if pcm is None:
            pcm = seg_pcm
        else:
            for ch, samples in enumerate(seg_pcm):
                pcm[ch] += samples
        if lpflg:
            if loop_start is None:
                loop_start = position
            loop_end = position + len(seg_pcm[0])
        position += len(seg_pcm[0])
    loop = (loop_start, loop_end) if loop_start is not None else None
    tags = {"ADX_TotalSamples": str(len(pcm[0])), "AAX_Segments": str(len(segments))}
    return pcm, loop, info, tags


def interleave(pcm, frames, loop):
    """ループを展開して frames フレーム分のインターリーブした int16 LE のバイト列にする"""
    channels = len(pcm)
    length = loop[1] if loop else len(pcm[0])
    order = list(range(min(frames, length)))
    if loop:
        while len(order) < frames:
            order += list(range(loop[0], loop[1]))
        order = order[:frames]
    out = bytearray()
    for frame in order:
        for ch in range(channels):
            out += struct.pack("<h", pcm[ch][frame])
    return bytes(out)


# ---------------------------------------------------------------------------

def build_fixtures():
    """(ファイル名, バイト列, [(曲番号, (pcm, loop, info, tags), 追加のタグ)]) を返す"""
    fixtures = []

    def add_adx(name, data):
        fixtures.append((name, data, [(1, song_adx(data), {})]))
        return data

    rate = 32000
    a = add_adx("v4_type3_2ch_loop_inserted.adx",
                make_adx(make_signal(2, 40000, rate, 0), rate, inserted=16, loop=(8000, 36000)))
    b = add_adx("v4_type3_6ch_loop.adx",
                make_adx(make_signal(6, 20000, 48000, 1), 48000, inserted=5, loop=(5115, 19000)))
    c = add_adx("v4_type3_1ch_short_header.adx",
                make_adx(make_signal(1, 12345, 22050, 2), 22050, loop_info=False))
    add_adx("v3_type3_2ch_loop.adx",
            make_adx(make_signal(2, 30000, 44100, 3), 44100, version=3, inserted=7, loop=(4089, 29000)))
    add_adx("v4_type2_2ch_noloop.adx",
            make_adx(make_signal(2, 15000, 24000, 4), 24000, encoding=2))
    add_adx("v4_type4_1ch_loop.adx",
            make_adx(make_signal(1, 18000, 44100, 5), 44100, encoding=4, inserted=31, loop=(1, 17000)))

    # AAX: ループしない前奏とループするセグメント。セグメントの ADX にはループ情報の欄がない
    signal = make_signal(2, 30000, rate, 6)
    intro = make_adx([s[:9000] for s in signal], rate, loop_info=False)
    body = make_adx([s[9000:] for s in signal], rate, loop_info=False)
    aax_segments = [(intro, 0), (body, 1)]
    aax = make_aax(aax_segments)
    fixtures.append(("two_segments.aax", aax, [(1, song_aax(aax_segments), {})]))

    # AFS: 名前表つき。日本語の名前は Shift_JIS、日時が 0 の項目もある
    members = [
        ("テスト曲.adx".encode("cp932"), (2026, 11, 2, 12, 34, 56), a),
        (b"short_header.adx", (0, 0, 0, 0, 0, 0), c),
        (b"multi_channel_loop_6ch_long_name_.adx", (1999, 12, 31, 23, 59, 59), b),   # 32 バイトで切れる
        (b"segments.aax", (2001, 1, 2, 3, 4, 5), aax),
    ]
    afs = make_afs(members)
    tracks = []
    for number, (name, date, content) in enumerate(members, 1):
        song = song_aax(aax_segments) if content is aax else song_adx(content)
        extra = {"Title": name[:32].decode("cp932")}
        if date[0] != 0:
            extra["AFS_FileDate"] = "%04d-%02d-%02d %02d:%02d:%02d" % date
        tracks.append((number, song, extra))
    fixtures.append(("archive.afs", afs, tracks))
    return fixtures


def generate(directory):
    os.makedirs(directory, exist_ok=True)
    expected = []
    for name, data, tracks in build_fixtures():
        with open(os.path.join(directory, name), "wb") as fp:
            fp.write(data)
        for number, (pcm, loop, info, tags), extra in tracks:
            # ループする曲は 2 周半、しない曲は終わりまで (少し多めに頼んで終わることを確かめる)
            if loop:
                frames = loop[1] + (loop[1] - loop[0]) * 5 // 2
            else:
                frames = len(pcm[0])
            raw_name = "%s.%d.raw" % (name, number)
            raw = interleave(pcm, frames, loop)
            with open(os.path.join(directory, raw_name), "wb") as fp:
                fp.write(raw)
            all_tags = {
                "ADX_IsLooped": "True" if loop else "False",
                "ADX_LoopStartSample": str(loop[0] if loop else 0),
                "ADX_LoopEndSample": str(loop[1] if loop else 0),
            }
            all_tags.update(tags)
            all_tags.update(extra)
            expected.append({
                "file": name, "track": number, "count": len(tracks),
                "rate": info["rate"], "channels": info["channels"],
                "looped": bool(loop), "frames": frames, "raw": raw_name,
                "sha256": hashlib.sha256(raw).hexdigest(), "tags": all_tags,
            })
    with open(os.path.join(directory, "expected.json"), "w", encoding="utf-8") as fp:
        json.dump(expected, fp, ensure_ascii=False, indent=1)
    print("generated %d tracks in %d files" % (len(expected), len(set(e["file"] for e in expected))))
    return 0


def run_cli(args, cli_args):
    command = (args.emulator or []) + [args.cli] + cli_args
    return subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)


def check(args):
    with open(os.path.join(args.dir, "expected.json"), encoding="utf-8") as fp:
        expected = json.load(fp)
    fails = 0
    for entry in expected:
        label = "%s #%d" % (entry["file"], entry["track"])
        path = os.path.join(args.dir, entry["file"])
        problems = []

        result = run_cli(args, ["-t", str(entry["track"]), "--info", path])
        if result.returncode != 0:
            problems.append("--info failed: " + result.stderr.decode(errors="replace").strip())
        else:
            info = {}
            for line in result.stdout.decode("utf-8").splitlines():
                key, _, value = line.partition("=")
                info[key] = value
            for key in ("count", "rate", "channels"):
                if info.get(key) != str(entry[key]):
                    problems.append("%s=%s, expected %s" % (key, info.get(key), entry[key]))
            for key, value in entry["tags"].items():
                if info.get("tag." + key) != value:
                    problems.append("tag %s=%r, expected %r" % (key, info.get("tag." + key), value))
            # 曲によって出たり出なかったりするタグは、出ないはずのときに出ていないことも確かめる
            for key in OPTIONAL_TAGS:
                if key not in entry["tags"] and "tag." + key in info:
                    problems.append("tag %s=%r, expected none" % (key, info["tag." + key]))

        # ループしない曲は多めに頼んで、ちょうどで終わることを確かめる
        request = entry["frames"] + (0 if entry["looped"] else 1000)
        result = run_cli(args, ["-t", str(entry["track"]), "-n", str(request), path])
        if result.returncode != 0:
            problems.append("render failed: " + result.stderr.decode(errors="replace").strip())
        else:
            with open(os.path.join(args.dir, entry["raw"]), "rb") as fp:
                raw = fp.read()
            got = result.stdout
            status = result.stderr.decode(errors="replace").strip()
            want_status = "frames=%d finished=%d" % (entry["frames"], 0 if entry["looped"] else 1)
            if status != want_status:
                problems.append("%s, expected %s" % (status, want_status))
            if got != raw:
                frame_bytes = 2 * entry["channels"]
                first = next((i for i in range(min(len(got), len(raw))) if got[i] != raw[i]), min(len(got), len(raw)))
                problems.append("PCM differs from frame %d (%d bytes, expected %d)" % (first // frame_bytes, len(got), len(raw)))

        if problems:
            fails += 1
            print("%s: NG" % label)
            for problem in problems:
                print("  " + problem)
        else:
            print("%s: ok" % label)
    print("%d/%d tracks ok" % (len(expected) - fails, len(expected)))
    return 1 if fails else 0


def run_seek_check(args, paths):
    batches = [paths[start:start + SEEK_CHECK_BATCH] for start in range(0, len(paths), SEEK_CHECK_BATCH)]
    returncode = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=os.cpu_count() or 1) as pool:
        # 結果はファイルの順に出す
        for result in pool.map(lambda batch: run_cli(args, ["--seek-check"] + batch), batches):
            sys.stdout.write(result.stderr.decode(errors="replace"))
            returncode = returncode or result.returncode
    return returncode


def seek_check(args):
    with open(os.path.join(args.dir, "expected.json"), encoding="utf-8") as fp:
        files = sorted(set(entry["file"] for entry in json.load(fp)))
    return run_seek_check(args, [os.path.join(args.dir, name) for name in files])


def seek_check_data(args):
    paths = []
    for root, dirs, files in os.walk(args.dir):
        dirs.sort()
        paths += [os.path.join(root, name) for name in sorted(files) if name.lower().endswith(DATA_EXTENSIONS)]
    if not paths:
        print("no ADX/AAX/AFS in %s" % args.dir)
        return 1
    return run_seek_check(args, paths)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("command", choices=["generate", "check", "seek-check", "seek-check-data"])
    parser.add_argument("dir")
    parser.add_argument("--cli", help="kpi-render の実行ファイル")
    parser.add_argument("--emulator", nargs="+", help="kpi-render を動かすコマンド (wine など)")
    args = parser.parse_args()
    if args.command == "generate":
        return generate(args.dir)
    if not args.cli:
        parser.error("--cli is required")
    commands = {"check": check, "seek-check": seek_check, "seek-check-data": seek_check_data}
    return commands[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
