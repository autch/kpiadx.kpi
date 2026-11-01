#pragma once

// CRI AAX: an @UTF table named "AAX" whose rows hold ADX segments.
//   column "data"  (vldata): ADX stream of the segment
//   column "lpflg" (integer): non-zero if the segment is part of the loop
// note: @UTF is in BIG ENDIAN

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>

#include "kpi.h"

struct AAXsegment
{
    uint64_t offset;    // offset of the ADX stream in the AAX file
    uint64_t size;
    bool loop;
};

namespace aax_detail
{
    inline uint16_t be16(const BYTE* p) { return static_cast<uint16_t>((p[0] << 8) | p[1]); }
    inline uint32_t be32(const BYTE* p) { return (static_cast<uint32_t>(be16(p)) << 16) | be16(p + 2); }

    constexpr BYTE UTF_FLAG_NAME = 0x10;
    constexpr BYTE UTF_FLAG_DEFAULT = 0x20;     // value is stored in the schema
    constexpr BYTE UTF_FLAG_ROW = 0x40;         // value is stored in each row
    constexpr BYTE UTF_FLAG_UNDEFINED = 0x80;

    constexpr BYTE UTF_TYPE_VLDATA = 0x0b;

    // size of a value by column type (0x00: u8 ... 0x0b: vldata), 0 = unknown
    inline uint32_t utf_type_size(BYTE type)
    {
        static constexpr uint32_t sizes[] = { 1, 1, 2, 2, 4, 4, 8, 8, 4, 8, 4, 8 };
        return type < std::size(sizes) ? sizes[type] : 0;
    }
}

// Returns false if pFile is not an AAX (or is malformed).
inline bool parseAAX(IKpiFile* pFile, std::vector<AAXsegment>& segments)
{
    using namespace aax_detail;

    constexpr uint32_t MAX_SCHEMA_SIZE = 0x10000;   // header+columns+rows+strings, data excluded
    constexpr uint32_t MAX_ROWS = 4096;
    constexpr uint32_t UTF_BODY_OFFSET = 8;         // offsets in the table are relative to here

    segments.clear();

    BYTE head[UTF_BODY_OFFSET + 0x18];
    pFile->Seek(0, FILE_BEGIN);
    if (pFile->Read(head, sizeof head) != sizeof head) return false;
    if (memcmp(head, "@UTF", 4) != 0) return false;

    const uint32_t table_size = be32(head + 4);
    const BYTE* h = head + UTF_BODY_OFFSET;
    const uint32_t rows_offset = be16(h + 0x02);
    const uint32_t strings_offset = be32(h + 0x04);
    const uint32_t data_offset = be32(h + 0x08);
    const uint32_t name_offset = be32(h + 0x0c);
    const uint32_t num_columns = be16(h + 0x10);
    const uint32_t row_width = be16(h + 0x12);
    const uint32_t num_rows = be32(h + 0x14);

    if (data_offset > table_size || data_offset > MAX_SCHEMA_SIZE) return false;
    if (strings_offset > data_offset || rows_offset > strings_offset || rows_offset < 0x18) return false;
    if (num_rows == 0 || num_rows > MAX_ROWS) return false;
    if (static_cast<uint64_t>(num_rows) * row_width > strings_offset - rows_offset) return false;

    std::vector<BYTE> t(data_offset);
    pFile->Seek(UTF_BODY_OFFSET, FILE_BEGIN);
    if (pFile->Read(t.data(), data_offset) != data_offset) return false;

    auto get_string = [&](uint32_t offset) -> const char*
    {
        if (offset >= data_offset - strings_offset) return nullptr;
        const auto* begin = reinterpret_cast<const char*>(t.data() + strings_offset + offset);
        const auto* end = reinterpret_cast<const char*>(t.data() + data_offset);
        return std::find(begin, end, '\0') != end ? begin : nullptr;
    };

    const char* table_name = get_string(name_offset);
    if (table_name == nullptr || strcmp(table_name, "AAX") != 0) return false;

    struct Column
    {
        BYTE flags;
        BYTE type;
        const char* name;
        uint32_t default_pos;   // position of the value in the schema, if UTF_FLAG_DEFAULT
        uint32_t row_pos;       // position of the value in a row, if UTF_FLAG_ROW
    };
    std::vector<Column> columns;
    uint32_t pos = 0x18;
    uint32_t row_pos = 0;
    for (uint32_t i = 0; i < num_columns; i++)
    {
        if (pos + 5 > rows_offset) return false;
        Column c{};
        c.flags = t[pos] & 0xf0;
        c.type = t[pos] & 0x0f;
        const uint32_t size = utf_type_size(c.type);
        if (size == 0 || (c.flags & UTF_FLAG_UNDEFINED)) return false;
        c.name = (c.flags & UTF_FLAG_NAME) ? get_string(be32(&t[pos + 1])) : nullptr;
        pos += 5;
        if (c.flags & UTF_FLAG_DEFAULT)
        {
            c.default_pos = pos;
            pos += size;
            if (pos > rows_offset) return false;
        }
        if (c.flags & UTF_FLAG_ROW)
        {
            c.row_pos = row_pos;
            row_pos += size;
        }
        columns.push_back(c);
    }
    if (row_pos > row_width) return false;

    const Column* col_data = nullptr;
    const Column* col_lpflg = nullptr;
    for (const auto& c : columns)
    {
        if (c.name == nullptr) continue;
        if (strcmp(c.name, "data") == 0) col_data = &c;
        if (strcmp(c.name, "lpflg") == 0) col_lpflg = &c;
    }
    if (col_data == nullptr || col_data->type != UTF_TYPE_VLDATA) return false;
    if (col_lpflg != nullptr && col_lpflg->type > 0x05) return false;    // must be an integer up to 32bit

    // returns the position of the value of the column at the row, or nullptr if the value is zero
    auto value_at = [&](const Column& c, uint32_t row) -> const BYTE*
    {
        if (c.flags & UTF_FLAG_ROW) return &t[rows_offset + row * row_width + c.row_pos];
        if (c.flags & UTF_FLAG_DEFAULT) return &t[c.default_pos];
        return nullptr;
    };

    const UINT64 file_size = pFile->GetSize();
    for (uint32_t row = 0; row < num_rows; row++)
    {
        const BYTE* d = value_at(*col_data, row);
        if (d == nullptr) return false;
        AAXsegment seg{};
        seg.offset = static_cast<uint64_t>(UTF_BODY_OFFSET) + data_offset + be32(d);
        seg.size = be32(d + 4);
        if (file_size != KPI_FILE_EOF && seg.offset + seg.size > file_size) return false;

        if (col_lpflg != nullptr)
        {
            if (const BYTE* v = value_at(*col_lpflg, row))
            {
                uint32_t lpflg = 0;
                for (uint32_t i = 0; i < utf_type_size(col_lpflg->type); i++)
                    lpflg = (lpflg << 8) | v[i];
                seg.loop = lpflg != 0;
            }
        }
        segments.push_back(seg);
    }
    return true;
}
