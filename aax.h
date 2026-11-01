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
    inline uint16_t be16(const BYTE* bytes) { return static_cast<uint16_t>((bytes[0] << 8) | bytes[1]); }
    inline uint32_t be32(const BYTE* bytes) { return (static_cast<uint32_t>(be16(bytes)) << 16) | be16(bytes + 2); }

    constexpr uint8_t UTF_FLAG_NAME = 0x10;
    constexpr uint8_t UTF_FLAG_DEFAULT = 0x20;  // value is stored in the schema
    constexpr uint8_t UTF_FLAG_ROW = 0x40;      // value is stored in each row
    constexpr uint8_t UTF_FLAG_UNDEFINED = 0x80;

    constexpr uint8_t UTF_TYPE_VLDATA = 0x0b;

    // size of a value by column type (0x00: u8 ... 0x0b: vldata), 0 = unknown
    inline uint32_t utf_type_size(uint8_t type)
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
    if (pFile->Read(head, sizeof head) != sizeof head)
    {
        ::OutputDebugStringW(L"parseAAX: cannot read @UTF header\n");
        return false;
    }
    if (memcmp(head, "@UTF", 4) != 0)
    {
        ::OutputDebugStringW(L"parseAAX: signature is not @UTF\n");
        return false;
    }

    const uint32_t table_size = be32(head + 4);
    const BYTE* table_header = head + UTF_BODY_OFFSET;
    const uint32_t rows_offset = be16(table_header + 0x02);
    const uint32_t strings_offset = be32(table_header + 0x04);
    const uint32_t data_offset = be32(table_header + 0x08);
    const uint32_t name_offset = be32(table_header + 0x0c);
    const uint32_t num_columns = be16(table_header + 0x10);
    const uint32_t row_width = be16(table_header + 0x12);
    const uint32_t num_rows = be32(table_header + 0x14);

    if (data_offset > table_size)
    {
        ::OutputDebugStringW(L"parseAAX: data offset is beyond the table\n");
        return false;
    }
    if (data_offset > MAX_SCHEMA_SIZE)
    {
        ::OutputDebugStringW(L"parseAAX: schema is too large\n");
        return false;
    }
    // the table is laid out as: header(0x18) columns, rows, strings, data
    if (rows_offset < 0x18 || rows_offset > strings_offset || strings_offset > data_offset)
    {
        ::OutputDebugStringW(L"parseAAX: offsets of rows/strings/data are out of order\n");
        return false;
    }
    if (num_rows == 0 || num_rows > MAX_ROWS)
    {
        ::OutputDebugStringW(L"parseAAX: number of rows is zero or too large\n");
        return false;
    }
    if (static_cast<uint64_t>(num_rows) * row_width > strings_offset - rows_offset)
    {
        ::OutputDebugStringW(L"parseAAX: rows overflow into the string table\n");
        return false;
    }

    std::vector<BYTE> schema(data_offset);
    pFile->Seek(UTF_BODY_OFFSET, FILE_BEGIN);
    if (pFile->Read(schema.data(), data_offset) != data_offset)
    {
        ::OutputDebugStringW(L"parseAAX: cannot read the whole schema\n");
        return false;
    }

    // returns nullptr if the offset is outside the string table or the string is not terminated
    auto get_string = [&](uint32_t offset) -> const char*
    {
        if (offset >= data_offset - strings_offset)
            return nullptr;
        const auto* begin = reinterpret_cast<const char*>(schema.data() + strings_offset + offset);
        const auto* end = reinterpret_cast<const char*>(schema.data() + data_offset);
        if (std::find(begin, end, '\0') == end)
            return nullptr;
        return begin;
    };

    const char* table_name = get_string(name_offset);
    if (table_name == nullptr)
    {
        ::OutputDebugStringW(L"parseAAX: invalid table name\n");
        return false;
    }
    if (strcmp(table_name, "AAX") != 0)
    {
        ::OutputDebugStringW(L"parseAAX: table name is not AAX\n");
        return false;
    }

    struct Column
    {
        uint8_t flags;
        uint8_t type;
        const char* name;
        uint32_t default_pos;   // position of the value in the schema, if UTF_FLAG_DEFAULT
        uint32_t row_pos;       // position of the value in a row, if UTF_FLAG_ROW
    };
    std::vector<Column> columns;
    uint32_t pos = 0x18;
    uint32_t row_pos = 0;
    for (uint32_t i = 0; i < num_columns; i++)
    {
        // 1 byte flags/type + 4 bytes name offset
        if (pos + 5 > rows_offset)
        {
            ::OutputDebugStringW(L"parseAAX: columns overflow into the rows\n");
            return false;
        }
        Column column{};
        column.flags = schema[pos] & 0xf0;
        column.type = schema[pos] & 0x0f;
        const uint32_t size = utf_type_size(column.type);
        if (size == 0)
        {
            ::OutputDebugStringW(L"parseAAX: unknown column type\n");
            return false;
        }
        if (column.flags & UTF_FLAG_UNDEFINED)
        {
            ::OutputDebugStringW(L"parseAAX: unsupported column flag 0x80\n");
            return false;
        }
        // an invalid name leaves the column unnamed; it is ignored below
        column.name = (column.flags & UTF_FLAG_NAME) ? get_string(be32(&schema[pos + 1])) : nullptr;
        pos += 5;
        if (column.flags & UTF_FLAG_DEFAULT)
        {
            column.default_pos = pos;
            pos += size;
            if (pos > rows_offset)
            {
                ::OutputDebugStringW(L"parseAAX: default value overflows into the rows\n");
                return false;
            }
        }
        if (column.flags & UTF_FLAG_ROW)
        {
            column.row_pos = row_pos;
            row_pos += size;
        }
        columns.push_back(column);
    }
    if (row_pos > row_width)
    {
        ::OutputDebugStringW(L"parseAAX: columns do not fit in the row width\n");
        return false;
    }

    const Column* col_data = nullptr;
    const Column* col_lpflg = nullptr;
    for (const auto& column : columns)
    {
        if (column.name == nullptr)
            continue;
        if (strcmp(column.name, "data") == 0) col_data = &column;
        if (strcmp(column.name, "lpflg") == 0) col_lpflg = &column;
    }
    if (col_data == nullptr)
    {
        ::OutputDebugStringW(L"parseAAX: no data column\n");
        return false;
    }
    if (col_data->type != UTF_TYPE_VLDATA)
    {
        ::OutputDebugStringW(L"parseAAX: data column is not vldata\n");
        return false;
    }
    // lpflg is optional, but must be an integer up to 32bit (type 0x00-0x05)
    if (col_lpflg != nullptr && col_lpflg->type > 0x05)
    {
        ::OutputDebugStringW(L"parseAAX: lpflg column is not an integer up to 32bit\n");
        return false;
    }

    // returns the position of the value of the column at the row,
    // or nullptr if the column has no value (that means zero)
    auto value_at = [&](const Column& column, uint32_t row) -> const BYTE*
    {
        if (column.flags & UTF_FLAG_ROW)
            return &schema[rows_offset + row * row_width + column.row_pos];
        if (column.flags & UTF_FLAG_DEFAULT)
            return &schema[column.default_pos];
        return nullptr;
    };

    const UINT64 file_size = pFile->GetSize();
    for (uint32_t row = 0; row < num_rows; row++)
    {
        const BYTE* data_value = value_at(*col_data, row);
        if (data_value == nullptr)
        {
            ::OutputDebugStringW(L"parseAAX: data column has no value\n");
            return false;
        }
        AAXsegment seg{};
        seg.offset = static_cast<uint64_t>(UTF_BODY_OFFSET) + data_offset + be32(data_value);
        seg.size = be32(data_value + 4);
        if (file_size != KPI_FILE_EOF && seg.offset + seg.size > file_size)
        {
            ::OutputDebugStringW(L"parseAAX: segment is beyond the end of file\n");
            return false;
        }

        if (col_lpflg != nullptr)
        {
            if (const BYTE* lpflg_value = value_at(*col_lpflg, row))
            {
                uint32_t lpflg = 0;
                for (uint32_t i = 0; i < utf_type_size(col_lpflg->type); i++)
                    lpflg = (lpflg << 8) | lpflg_value[i];
                seg.loop = lpflg != 0;
            }
        }
        segments.push_back(seg);
    }
    return true;
}
