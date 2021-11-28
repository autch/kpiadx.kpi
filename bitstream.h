#pragma once

#include <cstdint>
#include <climits>

struct bitstream
{
private:
    uint8_t* p;
    uint8_t* pe;
    size_t size;

    uint8_t value;

    int bits_avail;

    int fill_next()
    {
        if (p == pe) return -1;
        if (bits_avail != 0) return 0;

        p++;
        if (p == pe) return -1;

        value = *p;
        bits_avail = CHAR_BIT;

        return 1;
    }

public:
    bitstream(void* p, size_t size) : p(static_cast<uint8_t*>(p)), pe(static_cast<uint8_t*>(p) + size), size(size), value(*static_cast<uint8_t*>(p)), bits_avail(CHAR_BIT)
    {
        reset(p, size);
    }

    void reset(void* p, size_t size)
    {
        this->p = this->pe = static_cast<uint8_t*>(p);
        this->size = size;
        pe += size;
        value = *this->p;
        bits_avail = CHAR_BIT;
    }

    ~bitstream() = default;
    bitstream(bitstream& other) = default;
    bitstream(bitstream&& other) = default;
    bitstream& operator=(bitstream const& other) = default;
    bitstream& operator=(bitstream&& other) = default;

    template<int N>
    int get()
    {
        const int ret = (value >> N) & ((1 << N) - 1);
        value <<= N;
        bits_avail -= N;
        if(bits_avail == 0)
            fill_next();
        return ret;
    }

    int get(int bits)
    {
        int ret = 0;

        while (bits > 0)
        {
            int bits_to_use;

            if (bits < bits_avail)
            {
                bits_to_use = bits;
                bits = 0;
            }
            else
            {
                bits_to_use = bits_avail;
                bits -= bits_avail;
            }
            const int mask = (1 << bits_to_use) - 1;
            const int v = (value >> (CHAR_BIT - bits_to_use)) & mask;
            ret <<= bits_to_use;
            ret |= v;
            value <<= bits_to_use;
            bits_avail -= bits_to_use;
            if (bits_avail == 0 && fill_next() < 0)
                break;
        }
        return ret;
    }

    operator uint8_t* () const
    {
        return p;
    }
};
