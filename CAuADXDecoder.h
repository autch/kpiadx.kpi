
#pragma once

#define ADXK0 0x7298
#define ADXK1 0x3350

#define DEFAULT_MASTER_VOLUME 0x4000
#define BLOCKS_PER_ONCE (1 << 7)

struct SADX3Header
{
  DWORD dwOffsetToSigAndData;
  BYTE  abyReserved[3];
  BYTE  byChannels;
  DWORD dwSamplesPerSec;
  DWORD dwSamples;
  WORD  wReserved;
  BYTE  byVersion;
  BYTE  abyReserved2[9];
  DWORD dwLoopStart;
  WORD  wLoopPointS1;
  WORD  wLoopPointS2;
  DWORD dwLoopEnd;
};

struct SADX4Header
{
  DWORD dwOffsetToSigAndData;
  BYTE  abyReserved[3];
  BYTE  byChannels;
  DWORD dwSamplesPerSec;
  DWORD dwSamples;
  WORD  wReserved;
  BYTE  byVersion;
  BYTE  abyReserved2[9];
  DWORD dwReserved1;
  WORD  wLoopPointS1;
  WORD  wLoopPointS2;
  DWORD dwReserved2;
  DWORD dwLoopStart;
  DWORD dwReserved3;
  DWORD dwLoopEnd;
};

typedef union
{
  SADX3Header adx3;
  SADX4Header adx4;
}SADXHeader;

struct SADXContext
{
  int s1;
  int s2;
};

class CAuADXDecoder
{
private:
  // kpi �Ƃ��ĕK�v�Ȃ���
  HANDLE m_hFile;      // = CreateFile(...)
  SOUNDINFO m_Info;
  SADXHeader m_Header;
  Nokomo::UnitConvs::CAuPcmUnitConverter* m_pPuc;
  Nokomo::SyncObjects::CAuCriticalSection* m_pCs;

  // �s�x�w�b�_������͖̂ʓ|�Ȃ̂ł����ɂ����Ă���
  DWORD m_dwSamples;   // ���T���v����
  DWORD m_dwLoopStart; // �T���v���l
  DWORD m_dwLoopEnd;   // �T���v���l�C���[�v���Ȃ��Ƃ��l�͉�����̂Ŗ�������
  DWORD m_dwDataOffset;// �o�C�g�l�C�f�[�^�J�n�I�t�Z�b�g
  DWORD m_dwSamplesDecoded;
  int m_nMasterVolume;
  LPSTR m_szIniFile;

  // �t�B���^�Ŏg����O�܂ł̃T���v���l
  SADXContext m_Context[2]; // �ő� 2 �`�����l��

  // Big Endian 32bit ���� Little Endian DWORD ��
  DWORD BEtoLE(DWORD dwBE)
  {
    // (;�L�D�M)�c�c
    return ((dwBE & 0xff000000) >> 24)
         | ((dwBE & 0x00ff0000) >>  8)
         | ((dwBE & 0x0000ff00) <<  8)
         | ((dwBE & 0x000000ff) << 24)
         ;
  }
  WORD BEtoLE(WORD wBE)
  {
    return ((wBE & 0xff00) >> 8)
         | ((wBE & 0x00ff) << 8)
         ;
  }
  // BYTE* p ���� Big Endian �� 1word �ǂށD�ǂ񂾌� p �͎����� += 2 ���邱��
  WORD ReadWordBE(BYTE* p)
  {
    return *p << 8 | *(p + 1);
  }

  // m_hFile ����w�b�_��ǂ�ŉ��
  BOOL ReadHeader()
  {
    DWORD dwBytesRead = 0;
    BOOL r;
    r = ReadFile(m_hFile, &m_Header, sizeof(SADX4Header), &dwBytesRead, NULL);
    if(!r || dwBytesRead != sizeof(SADX4Header)) return FALSE;

    // DWORD �ȃ����o�̃G���f�B�A���l�X�ϊ�
#define SWAP_DWORD_MEMBER(name) m_Header.adx4.name = BEtoLE(m_Header.adx4.name)
    SWAP_DWORD_MEMBER(dwOffsetToSigAndData);
    SWAP_DWORD_MEMBER(dwSamplesPerSec);
    SWAP_DWORD_MEMBER(dwSamples);
    SWAP_DWORD_MEMBER(dwLoopStart);
    SWAP_DWORD_MEMBER(dwLoopEnd);
#undef SWAP_DWORD_MEMBER
#define SWAP_WORD_MEMBER(name) m_Header.adx4.name = BEtoLE(m_Header.adx4.name)
    SWAP_WORD_MEMBER(wLoopPointS1);
    SWAP_WORD_MEMBER(wLoopPointS2);
#undef SWAP_WORD_MEMBER

    m_Header.adx3.dwLoopStart = BEtoLE(m_Header.adx3.dwLoopStart);
    m_Header.adx3.dwLoopEnd = BEtoLE(m_Header.adx3.dwLoopEnd);

    ZeroMemory(&m_Info, sizeof(SOUNDINFO));

    DWORD dwSignatureAddress;

    dwSignatureAddress = (m_Header.adx4.dwOffsetToSigAndData & 0x7fffffff) - 2;
    m_dwDataOffset = (m_Header.adx4.dwOffsetToSigAndData & 0x7fffffff) + 4;
    if(m_dwDataOffset >= GetFileSize(m_hFile, NULL)) return FALSE;

    // ���̕ӂ̕ςȒl�� KMP ���R��
    m_Info.dwSamplesPerSec = m_Header.adx4.dwSamplesPerSec;
    m_Info.dwChannels = m_Header.adx4.byChannels;
    m_Info.dwBitsPerSample = 16; // TODO: 16bit ���߂����ł�����ˁH
    m_Info.dwUnitRender = (32 * m_Info.dwChannels * (m_Info.dwBitsPerSample >> 3)) * BLOCKS_PER_ONCE; // 1 �u���b�N 32 �T���v���� 16bit ���`�����l������
    m_Info.dwMultiSongFlag = FALSE;
    m_Info.dwSeekable = TRUE;    // TODO: �����V�[�N�s�ł����Ȃ��H

    // TODO: ���̃o�[�W�������莩�̉��������c�c
    switch(m_Header.adx4.byVersion)
    {
      case 3:
      {
        // TODO: ADX v3 ���ăt�@�C�������ĂȂ��́c�c
        m_dwLoopStart = m_Header.adx3.dwLoopStart;
        m_dwLoopEnd = m_Header.adx3.dwLoopEnd;
        break;
      }
      case 4:
      {
        m_dwLoopStart = m_Header.adx4.dwLoopStart;
        m_dwLoopEnd = m_Header.adx4.dwLoopEnd;
        break;
      }
      default:
        return FALSE;
    }
    // TODO: FIXME: ���[�v�L���̔��肪�ρD
    // �f�[�^�I�t�Z�b�g�̒l���w�b�_�𒴂�������Ȃ�
    // �܂�p�f�B���O���Ȃ��Ƃ������ƂȂ̂Ń��[�v���Ȃ��D
    // �Z�N�^�p�f�B���O���K�v�Ȃ̂̓��[�v������Ƃ��ł���D
    // �������o�[�W�����Ɋ֌W�Ȃ��炵��
    m_dwSamples = m_Header.adx4.dwSamples;
    m_Info.dwLoopFlag = (m_dwDataOffset != (sizeof(SADX3Header)) && m_dwDataOffset != (sizeof(SADX4Header)))
                        && m_dwLoopStart < m_dwLoopEnd && m_dwLoopEnd <= m_dwSamples;

    int nLoopTimes = 2;
    if(m_szIniFile)
      nLoopTimes = GetPrivateProfileInt("kpiadx", "Loop", 2, m_szIniFile);
    if(m_Info.dwLoopFlag)
    {
      // TODO: FIXME: ���[�v��񌈂߂����ł����H
      // �T���v���P�ʂ͊ȒP�ɃI�[�o�[�t���[����̂� 64bit int
      Nokomo::QWORD qwSampleLength = m_dwLoopStart                         // �C���g���i�t�@�C���擪���烋�[�v�J�n�܂Łj
                                   + (m_dwLoopEnd - m_dwLoopStart) * nLoopTimes;    // ���[�v���
      m_Info.dwLength = qwSampleLength * 1000 / m_Info.dwSamplesPerSec;
    }
    else
    {
      Nokomo::QWORD qwSampleLength = m_dwSamples;
      m_Info.dwLength = qwSampleLength * 1000 / m_Info.dwSamplesPerSec;
    }

    if(m_szIniFile)
      m_nMasterVolume = GetPrivateProfileInt("kpiadx", "MasterVolume", DEFAULT_MASTER_VOLUME, m_szIniFile);

    return TRUE;
  }
  // Open() ��C�t�@�C���擪����Đ��ł���悤��������
  // �V�[�N����Ƃ��ɂ��g��
  void Reset()
  {
    SetFilePointer(m_hFile, m_dwDataOffset, NULL, FILE_BEGIN);
    m_dwSamplesDecoded = 0;
    ZeroMemory(m_Context, sizeof(SADXContext) * 2);
  }
  // 1w + 32h ���f�R�[�h����
  void DecodeBlock(BYTE* pSource, short* pDest, SADXContext& context)
  {
    int nScale = ReadWordBE(pSource);
    int d;
    int s0, s1 = context.s1, s2 = context.s2;
    int nMasterVolume = m_nMasterVolume;

    pSource += 2;
    for(int i = 0; i < 16; i++)
    {
      d = *pSource >> 4;
      if(d & 0x08)  d -= 0x10;
      s0 = (((d * nScale) * nMasterVolume) + ADXK0 * s1 - ADXK1 * s2) >> 14;
      if(s0 >  0x7fff)  s0 =  0x7fff;
      if(s0 < -0x7fff)  s0 = -0x7fff;
      *pDest++ = s0;  s2 = s1;  s1 = s0;

      d = *pSource & 0x0f;
      if(d & 0x08)  d -= 0x10;
      s0 = (((d * nScale) * nMasterVolume) + ADXK0 * s1 - ADXK1 * s2) >> 14;
      if(s0 >  0x7fff)  s0 =  0x7fff;
      if(s0 < -0x7fff)  s0 = -0x7fff;
      *pDest++ = s0;  s2 = s1;  s1 = s0;

      pSource++;
    }
    context.s1 = s1;
    context.s2 = s2;
  }
  // dwSize (% dwUnitRender == 0) �̕������f�R�[�h����
  DWORD DecodeBuffer(BYTE* pBuffer, DWORD dwSize)
  {
    DWORD dwBytesDecoded = 0;
    DWORD dwBytesRead = 0;
    DWORD dwSamplesToCopy;
    BOOL r;
    DWORD dwBlocksToRead = BLOCKS_PER_ONCE * m_Info.dwChannels;
    BYTE* abySource = new BYTE[18 * dwBlocksToRead];
    if(m_Info.dwChannels == 1)
    {
      // ���m����
      short sBuffer[32];
      while(dwBytesDecoded < dwSize)
      {
        r = ReadFile(m_hFile, abySource, 18 * dwBlocksToRead, &dwBytesRead, NULL);
        if(!r || dwBytesRead < 18) break;
        BYTE* pSource = abySource;
        while(pSource < abySource + dwBytesRead && dwBytesDecoded < dwSize)
        {
          DecodeBlock(pSource, sBuffer, m_Context[0]);
          pSource += 18;
          // ���T���v������ 32 samples �̔{���Ƃ͌���Ȃ��̂ŁC
          // �R�s�[����T�C�Y�̓T���v�����Ō��߂�
          dwSamplesToCopy = (m_dwSamples - m_dwSamplesDecoded) > 32 ? 32 : m_dwSamples - m_dwSamplesDecoded;
          if(pBuffer)
            CopyMemory(pBuffer, sBuffer, dwSamplesToCopy * 2);

          pBuffer += dwSamplesToCopy * 2;
          dwBytesDecoded += dwSamplesToCopy * 2;
          m_dwSamplesDecoded += dwSamplesToCopy;
          if(dwSamplesToCopy != 32) goto BreakDecoding;
        }
      }
    }
    else
    {
      // �X�e���I
      short sBufferL[32], sBufferR[32];
      while(dwBytesDecoded < dwSize)
      {
        // ��
        r = ReadFile(m_hFile, abySource, 18 * dwBlocksToRead, &dwBytesRead, NULL);
        if(!r || dwBytesRead < 18) break;
        BYTE* pSource = abySource;
        while(pSource < abySource + dwBytesRead && dwBytesDecoded < dwSize)
        {
          DecodeBlock(pSource, sBufferL, m_Context[0]);
          pSource += 18;
          DecodeBlock(pSource, sBufferR, m_Context[1]);
          pSource += 18;

          // ADX �ł͊e�`�����l���̃T���v���� 32 samples ���Ƃ�
          // LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR ...
          // �ƕ���ł���̂ŁC����� WaveAudio �� LRLRLRLR ... �`���ɒ���
          DWORD dwSamplesToCopy = (m_dwSamples - m_dwSamplesDecoded) > 32 ? 32 : m_dwSamples - m_dwSamplesDecoded;

          if(pBuffer)
          {
            short* pStereo = (short*)pBuffer;
            for(DWORD i = 0; i < dwSamplesToCopy; i++)
            {
              *pStereo++ = sBufferL[i];
              *pStereo++ = sBufferR[i];
            }
          }

          pBuffer += dwSamplesToCopy * 2 * 2;
          dwBytesDecoded += dwSamplesToCopy * 2 * 2;
          m_dwSamplesDecoded += dwSamplesToCopy;
          if(dwSamplesToCopy != 32) goto BreakDecoding;
        }
      }
    }
BreakDecoding:
    delete[] abySource;
    return dwBytesDecoded;
  }
public:
  CAuADXDecoder()
  {
    m_hFile = NULL;
    m_pPuc = NULL;
    m_pCs = new Nokomo::SyncObjects::CAuCriticalSection();
    m_nMasterVolume = DEFAULT_MASTER_VOLUME;
    m_szIniFile = NULL;
  }
  ~CAuADXDecoder()
  {
    Close();
    delete m_pPuc;
    delete m_pCs;
  }
  BOOL Open(LPSTR szFileName, SOUNDINFO* pInfo)
  {
    Nokomo::SyncObjects::CAuCriticalSectionEx cs(m_pCs);

    m_hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(m_hFile)
    {
      if(ReadHeader())
      {
        CopyMemory(pInfo, &m_Info, sizeof(SOUNDINFO));
        Reset();
        return TRUE;
      }
      Close();
    }
    return FALSE;
  }
  // �������̃t�@�C�����g��Ȃ��Ƃ��ĂԁD���Ƃ� delete ���邾��
  void Close()
  {
    CloseHandle(m_hFile);
    m_hFile = NULL;
  }
  DWORD SetPosition(DWORD dwPos)
  {
    Nokomo::SyncObjects::CAuCriticalSectionEx cs(m_pCs);
    DWORD dwPosSamples = dwPos / 1000 * m_Info.dwSamplesPerSec;
    if(m_Info.dwLoopFlag && dwPosSamples >= m_dwLoopStart)
    {
      SeekToLoopStartBlock();
      return (Nokomo::QWORD)m_dwLoopStart * 1000 / m_Info.dwSamplesPerSec;
    }
    else
    {
      Reset();
      return 0;
    }
  }
  DWORD Render(BYTE* pBuffer, DWORD dwSize)
  {
    Nokomo::SyncObjects::CAuCriticalSectionEx cs(m_pCs);
    return DecodeBuffer(pBuffer, dwSize);
  }
  VOID SeekToLoopStartBlock()
  {
    // ���[�v�n�_�̃u���b�N�̓Z�N�^�A���C������Ă邾���łȂ�
    // ���[�v�n�_�̓u���b�N�擪�ɃA���C������Ă���D
    DWORD dwBlockOffset = m_dwDataOffset + (m_dwLoopStart >> 5) * 18 * m_Info.dwChannels;
    Reset();
    SetFilePointer(m_hFile, dwBlockOffset, NULL, FILE_BEGIN);
    // ���̓w�b�_�ɓ����Ă���D0 �ŏ��������Ă��̊����Ȃ�
    m_Context[0].s1 = m_Header.adx4.wLoopPointS1;
    m_Context[0].s2 = m_Header.adx4.wLoopPointS2;
    m_Context[1].s1 = m_Header.adx4.wLoopPointS1;
    m_Context[1].s2 = m_Header.adx4.wLoopPointS2;
    m_dwSamplesDecoded = m_dwLoopStart;
  }
  BOOL GetLoopFlag() const { return m_Info.dwLoopFlag; }
  DWORD GetLoopStart() const { return m_dwLoopStart; }
  DWORD GetLoopEnd() const { return m_dwLoopEnd; }
  DWORD GetCurrentSamplesRendered() const { return m_dwSamplesDecoded; }
  VOID SetConfigPath(LPSTR szPath) { m_szIniFile = szPath; }
};

