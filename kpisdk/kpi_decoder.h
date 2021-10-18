#pragma once
#ifndef kpi_decoderH
#define kpi_decoderH

#include "kpi.h"
#pragma pack(push, 1) //�\���̂̃A���C�������g

///////////////////////////////////////////////////////////////////////////////
/*�f�R�[�_�v���O�C��
  �I�[�f�B�I�f�[�^�� PCM �Ƀf�R�[�h

  ���d�l(kmp_pi.h)�ŏo���Ȃ��������[�v�Ɋւ���ׂ��Ȑ���A1�t�@�C�������Ȃ�
  ������ƑΉ��B

  IKpiFolder/IKpiTagInfo/IKpiConfig/IKpiUnkProvider �͖���
  IKpiFile::GetRealFileW �Ŏ��t�@�C�����擾

  �ł�����x�̂��͍̂쐬�\�B�Ƃ肠�����������̂��A�������x�ł���΁A
  kpi.h �͂�������Ƃ͓ǂ܂Ȃ��Ă� OK (��������ƓǂޕK�v�͂��邪)

  ���t�@�C��������t�@�C�����J���ꍇ�A�J�����t�@�C�������܂� IKpiFile ��
  Release ���Ă͂����Ȃ����Ƃɒ��ӁB

  �f�R�[�_�v���O�C�����G�N�X�|�[�g����֐�
  typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,      //IKpiXXXModule �� IID
                                                void **ppvObject, //IKpiXXXModule �̖߂�l
                                                IKpiUnknown *pUnknown);
  HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
  riid �� IID_IKpiDecoderModule ���n�����B
  �K�v�ł���� pUnknown ���� IKpiConfig �����擾���邱�ƁB
  kpi_CreateConfig ���̃w���p�[�֐����Ăԏꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ�����B

  *ppvObject �ɃC���X�^���X��n���ꍇ�́A�K�؂Ȍ^�ɃL���X�g���邱�ƁB
  �Ⴆ�΁A

    if(IsEqualIID(riid, IID_IKpiDecoderModule){
        CKpiXXXDecoderModule *pModule = new CKpiXXXDecoderModule;
        // *ppvObject = pModule; //NG
        *ppvObject = (IKpiDecoderModule*)pModule; //OK
    }

  �̂悤�ɂ��邱�ƁBCKpiXXXDecoderModule �������̃N���X����h�����Ă���ꍇ�A
  �K�؂ȃ|�C���^���n����Ȃ��\��������B
*/
///////////////////////////////////////////////////////////////////////////////
#define KPI_DECODER_MODULE_VERSION 200 

class __declspec(uuid("{EAEF6826-DE5A-4D10-A170-296DB8C6FD53}")) IKpiDecoder;
class __declspec(uuid("{2F02A4B1-F3FA-4725-AE31-54EA8A1EB095}")) IKpiDecoderModule;

#define IID_IKpiDecoder       __uuidof(IKpiDecoder)
#define IID_IKpiDecoderModule __uuidof(IKpiDecoderModule)

///////////////////////////////////////////////////////////////////////////////
struct KPI_DECODER_MODULEINFO
{
    enum{
        TAG_TITLE  = 0x01,   //�^�C�g���擾�Ή�(Media �N�X�v���[���ł̕\���p���)
        TAG_EXTRA  = 0x01<<1,//�ڍ�(�f�R�[�h�p�ɊJ�����Ƃ������擾�\�ȏ��)�Ή�
        TAG_DECODER_EXCLUSIVE = 0x01<<2,//�f�R�[�h���Ƀ^�O�擾�s��(������)
            //dwMultipleInstance �� KPI_MULTINST_ZERO ������ȊO���ł��Ⴂ������(��q)
    };
    DWORD cb;             //=sizeof(KPI_DECODER_MODULEINFO)
    DWORD dwModuleVersion;//=KPI_DECODER_MODULE_VERSION
    DWORD dwPluginVersion;//�v���O�C���̃o�[�W����
    DWORD dwMultipleInstance;//KPI_MULTINST_INFINITE/ZERO/ONE/UNIQUE
    GUID  guid;           //�v���O�C�����ʎq
    const wchar_t *cszDescription;  //�v���O�C���̖���
    const wchar_t *cszCopyright;    //���쌠(����������ꍇ�͉��s������)
    const wchar_t *cszSupportExts;  //�Ή��g���q('/' �ŋ�؂�)(��F".wav/.mp3", ".aaa/.bbb")(�g���q���܂߂�)
    const wchar_t *cszMultiSongExts;//�Ή��g���q�̂���1�̃t�@�C���ɕ����Ȋ܂ތ`��
    void  *pvReserved1;//�\��(must be NULL)
    void  *pvReserved2;//�\��(must be NULL)
    DWORD dwSupportTagInfo; //�^�O�擾�Ή�(TAG_TITLE/EXTRA/DECODER_EXCLUSIVE �̑g�ݍ��킹)
    DWORD dwSupportConfig;  //IKpiConfig �ɂ��ݒ�̓ǂݏ����ɖ��Ή��Ȃ� 0
    DWORD dwReserved[4];    //must be 0
/*
    dwMultipleInstance: �f�R�[�_�C���X�^���X�𓯎��ɐ����o���鐔
                        (���d�l�� dwReentrant �ɑ���)
      KPI_MULTINST_INFINITE: (����������������)�f�R�[�_�C���X�^���X�𓯎��ɉ��ł��쐬�\(���d�l��dwReentrant==1)
      KPI_MULTINST_ZERO:     ��ɕʃv���Z�X�N��(���d�l��dwReentrant==0xFFFFFFFF)
      KPI_MULTINST_ONE:      �f�R�[�_�C���X�^���X�𓯎���1�܂ō쐬�\
                             (2�ڈȍ~�͕ʃv���Z�X�N��)(���d�l�� dwRrentrant == 0)
      KPI_MULTINST_ZERO + 2: �f�R�[�_�C���X�^���X�𓯎���2�܂ō쐬�\
                             (3�ڈȍ~�͕ʃv���Z�X�N��)
      KPI_MULTINST_ZERO + N: �f�R�[�_�C���X�^���X�𓯎���N�܂ō쐬�\
                             ((N+1)�ڈȍ~�͕ʃv���Z�X�N��)
      KPI_MULTINST_UNIQUE:   �v���Z�X���܂����ł��f�R�[�_�C���X�^���X�͏��1�����쐬�s��

    �ʏ�� KPI_MULTINST_INFINITE/ZERO/ONE �̂����ꂩ
    �Ȃ�ׂ� dwMultipleInstance=KPI_MULTINST_INFINITE �ƂȂ邱�Ƃ��]�܂���
    dwMultipleInstance=KPI_MULTINST_ZERO �Ƃ���ׂ��Ȃ͈̂ȉ��̂悤�ȏꍇ
      �E�v���O�C�����ˑ����郉�C�u������ DLL �������C���X�^���X�����쐬�ɔ�Ή�
      �E���� DLL ���ʂ̃v���O�C��������g�p�����\��������
        (���̉\�����قڂȂ��ꍇ�� KPI_MULTINST_ONE �ŗǂ�)
      �E�v���O�C����ˑ����C�u�����̓��삪�s����ŁA�v���O�C���������ŋN����
        �����I����n���O�A�b�v�ɖ{�̂��������܂�Ȃ��悤�ɂ������ꍇ
        (kbrunkpi.exe ���������܂�邪�Akbrunkpi.exe ���n���O�A�b�v���Ă��{�̂�
        ��������m����)
      �E�f�o�b�O������ɂȂ�̂ŁA�f�o�b�O���[�h�ł� KPI_MULTINST_ONE �ɂ���Ɨǂ�
    dwSupportTagInfo :TAG_TITLE/EXTRA �e�t���O�̑g�ݍ��킹
      0:         �^�O�擾��Ή�
      TAG_TITLE: �^�C�g���擾�Ή�(Media �N�X�v���[���ł̕\���p���)
         �^�O�擾��p���[�h�ł̏��擾�ɑΉ����Ă���΃Z�b�g����B
         Open �̑�1����(cpRequest) �� Select �̑�2����(ppMediaInfo) �� NULL
         �̂Ƃ����^�O�擾��p�ŊJ��
         �Z�b�g����Ă��Ȃ��Ƃ��� Media �G�N�X�v���[���ŏ��\������ Open ��
         �Ă΂�Ȃ��B
      TAG_EXTRA:  �ڍ�(�f�R�[�h�p�ɊJ�����Ƃ������擾�\�ȏ��)�Ή�
         �f�R�[�h�p�ɊJ�����������擾�\�ȏ�񂪂���ꍇ�ɃZ�b�g�B
         Open �̑�1����(cpRequest) �� Select �̑�2����(ppMediaInfo) �����ɔ�NULL
         �̏ꍇ���f�R�[�h�p�ŊJ��
      TAG_DECODER_EXCLUSIVE: �f�R�[�h���͕ʃv���Z�X�o�R�Ń^�O�擾
         ���C�u�����̎d�l��f�R�[�h���Ƀ^�O�擾�o���Ȃ��ꍇ�̓Z�b�g����B
         �Z�b�g����Ă���Ƃ��͕ʃv���Z�X�o�R�Ń^�O�擾���s��

      �^�C�g��/�A�[�e�B�X�g�̎擾�͔�Ή�(�{�̂ɔC����)�������̑��̒ǉ����̎擾
      �ɂ͑Ή�����ꍇ�� TAG_TITLE �̓Z�b�g���Ȃ��� TAG_EXTRA �������Z�b�g����
      (�f�R�[�h�p�ȊO�ł� Open ���Ă΂Ȃ�)

      TAG_DECODER_EXCLUSIVE ���Z�b�g����Ă���ꍇ�A�Đ��p�Ƀf�R�[�_�� Open ����
      ����ꍇ�͕ʃv���Z�X�o�R�Ń^�O�擾���s��(�ő��ɂȂ��Ǝv�����A�Đ����̋Ȃ���
      �^�O�擾�o�����A�����C���X�^���X�𓯎��ɍ쐬�o���Ȃ��悤�ȃ��C�u�����ɑΉ�
      ���邽�߂̂���)

      TAG_DECODER_EXCLUSIVE ���Z�b�g����Ă��� dwMultipleInstance=KPI_MULTINST_ZERO
      �̏ꍇ�̓^�O�擾��(�f�R�[�h���łȂ��Ă�)��ɕʃv���Z�X�o�R�ōs���B

      TAG_DECODER_EXCLUSIVE ���Z�b�g����Ă��Ȃ��ꍇ�́A�����쐬�\�ȃC���X�^���X����
      �����������Ă��A�^�O�擾��(�f�R�[�h���ł����Ă�)��ɖ{�̃v���Z�X����s���B

      dwSupportTagInfo=0 �̂Ƃ��� Media �G�N�X�v���[���ł̃^�C�g���擾�Ƀv���O�C����
      �g�p����Ȃ�(Open ���Ă΂�Ȃ�)
      (dwSupportTagInfo&TAG_TITLE) !=0 �̂Ƃ��� Media �G�N�X�v���[���ł̃^�C�g��
      �擾����
      �EOpen �̑�1����(cpRequest)�� NULL
      �ESelect �̑�2����(ppMediaInfo)�� NULL�A��4������ dwTagGetFlags �ɕK�v�ȏ��
        �������Z�b�g(���݂̎����ł� KPI_TAGGET_FLAG_TITLE|KPI_TAGGET_FLAG_ARTIST)

      ��n���ČĂяo�����B���̂Ƃ��ASelect �̑�4����(dwTagGetFlags) �Ŏw�肵��
      ���Ȃ����͂Ȃ�ׂ��ʒm���Ȃ�����(�ʒm���Ă��\��Ȃ������������)

    cszSupportExts: �Ή��g���q
      '/' �ŋ�؂�A�g���q . ���܂߂�
      ".mp1/.mp2/.mp3" �̂悤�ɂ���
    cszMultiSongExts: �Ή��g���q�̂��� 1�̃t�@�C���ɕ����̋Ȃ��܂މ\�����������
      cszSupportExts �̕��ɂ��܂߂Ă��Ȃ��ƔF������Ȃ�
      cszSupportExts = ".aaa/.bbb;
      cszMultiSongExts = ".aaa/.ccc";
      �Ⴆ�΁��̂悤�ȏꍇ�A.aaa �� .bbb ���Ή��g���q�ƌ��Ȃ���A���̂����g���q .aaa ��
      1�̃t�@�C���ɕ����Ȋ܂ތ`���Ƃ��ĔF������邪�A�g���q .ccc �͖��������
      ���̃v���O�C�����g���q .ccc �ɑΉ����Ă��Ă��e�����Ȃ�
*/
};
///////////////////////////////////////////////////////////////////////////////
struct KPI_MEDIAINFO
{
    enum{
        SEEK_DISABLE        = 0,    //�V�[�N�s��
        SEEK_FLAGS_SAMPLE   = 0x01, //�T���v���P�ʂ̍����x�ȃV�[�N�ɑΉ�
        SEEK_FLAGS_ACCURATE = 0x02, //�̎��Ƃ̓����Ɏx�Ⴊ�Ȃ����x�ɐ��m�ȃV�[�N�Ή�
        SEEK_FLAGS_ROUGH    = 0x04, //���x�͈������V�[�N�Ή�
        FORMAT_PCM = 0,    //�ʏ�� PCM
        FORMAT_DOP = 1,    //�ŏI�o�͒i�܂Ńf�[�^�̉��H�����Ȃ�(���v���C�Q�C����t�F�[�h�A�E�g�̏��������Ȃ�)
        FORMAT_PLAYER = 2  //���t��p(IKpiPlayer �p)
    };
    DWORD  cb;             //=sizeof(KPI_MEDIAINFO)
    DWORD  dwNumber;       //�Ȕԍ�(1�x�[�X)�A�Ȕԍ�0�͑��݂��Ȃ�(�G���[�Ƃ��Ĉ�����)
                           //Select �őI�Ȃ����ԍ��ɂ��邱��
    DWORD  dwCount;        //�Ȃ̐��A�Ȑ� 0 �͑��݂��Ȃ�(�G���[�Ƃ��Ĉ�����)
    DWORD  dwFormatType;   //FORMAT_PCM or FORMAT_DOP(DOP �̏ꍇ�A�ŏI�o�͒i�܂Ńf�[�^�̉��H�����Ȃ����Ƃ��ۏ؂����)
    DWORD  dwSampleRate;   //�Đ����g��(44100, 48000 etc...)
    INT32  nBitsPerSample; //�ʎq���r�b�g��(8bit, 16bit, 24bit, 32bit, -32bit, -64bit)
                           //float �̏ꍇ�A�l�𕉂ɂ���(32bit float => -32, 64bit float => -64)
    DWORD  dwChannels;     //�`���l����(1 or 2)
    DWORD  dwSpeakerConfig;//0(�}���`�`�����l���Ή��̂��߂ɗ\��)
    UINT64 qwLength;       //�P���[�v��(�C���g���{���[�v�P��)�̋Ȃ̒���(�P�ʂ�100�i�m�b)
    UINT64 qwLoop;         //�Q���[�v�ڈȍ~�̂P���[�v�̒���(�P�ʂ�100�i�m�b)
    UINT64 qwFadeOut;      //�t�F�[�h�A�E�g����(�P�ʂ�100�i�m�b)(�t�F�[�h�A�E�g���Ȃ��ꍇ�� 0, �{�̂̐ݒ�ɏ]���ꍇ��-1)
                           //(0 �ł� -1 �ł��Ȃ��ꍇ�̓f�[�^�̐����l)
    DWORD  dwLoopCount;    //���[�v��(���[�v�ȂłȂ� or �{�̂̐ݒ�ɏ]���ꍇ��0�A0 �ȊO�̏ꍇ�̓f�[�^�̐����l)
    DWORD  dwUnitSample;   //Render �̑�2����(dwSizeSample) �ɓn���ׂ���(�����ł��ǂ��ꍇ�� 0)
                           //�Ȃ�ׂ� 0 �ɂ��邱��(�������ʓ|�Ȃ� 0 �ȊO�ł� OK)
                           //16384 ���傫�Ȓl�ɂ��Ă͂����Ȃ�(���̏ꍇ�͊撣���Ď������邱��)
    DWORD  dwSeekableFlags;//Seek �ւ̑Ή��t���O
           //�V�[�N�ɑΉ����Ă��Ȃ��ꍇ�� 0
           //�T���v���P�ʂ̐��m�ȃV�[�N�ɑΉ�����ꍇ�� SEEK_FLAGS_SAMPLE ���Z�b�g
           //�̎������\�Ȑ��x�̃V�[�N�ɑΉ�����ꍇ�� SEEK_FLAGS_ACCURATE ���Z�b�g
           //���x�͈������V�[�N�ɑΉ�����ꍇ�� SEEK_FLAGS_ROUGH ���Z�b�g
           //�V�[�N�̐��x: SAMPLE > ACCURATE > ROUGH
           //�V�[�N�̑��x: ROUGH > ACCURATE > SAMPLE
           //(�S���Ή�����ꍇ�� dwSeekableFlag = SEEK_FLAGS_SAMPLE | SEEK_FLAGS_ACCURATE | SEEK_FLAGS_ROUGH �ƂȂ�)
    DWORD  dwVideoWidth;   //�f���̉���(100%��)(�f�����������Ȃ��ꍇ�� 0)
    DWORD  dwVideoHeight;  //�f���̍���(100%��)(�f�����������Ȃ��ꍇ�� 0)
    DWORD  dwReserved[6];  //�\��(0 �ɂ��邱��)
/*
    �E�Đ����g���A�r�b�g���A�`���l�����A���[�v�񐔁A�t�F�[�h�A�E�g���ԗv��
    IKpiDecoderModule::Open �� cpRequest �̊e�l�� 0 �ȊO�̏ꍇ�́A�\�Ȃ�
    ���̌`���ŁA�s�\�Ȃ炻��ɍł��߂��`���Ńt�@�C�����J�����ƁB
    �l�� 0 �̏ꍇ�̓v���O�C���̃f�t�H���g�l�ŊJ������

    qwLength, qwLoop, qwFadeOut �̎��Ԃ̒P�ʂ� 100�i�m�b=(1/10000�~���b)
    �Ⴆ�� qwLength �� 1�b �Ȃ� 1�b=1000ms=1000*10000 �ƂȂ�

    ���[�v�P�񂠂���̉��t���Ԃ��킩�炸�A�������[�v����ꍇ(Render �̖߂�l��
    ���܂Ōo���Ă� dwUnitSample ��菬�����Ȃ�Ȃ��\��������ꍇ)��
    qwLoop == (UINT64)-1

    ���t���Ԃ��킩���Ă��ă��[�v���Ȃ��Ȃ̏ꍇ(�ł���ʓI�ȃP�[�X):
        qwLength == �Ȃ̒���, qwLoop == 0
        ���̂Ƃ� qwFadeOut �� dwLoopCount �̒l�͖��������

    ���t����(�C���g��+���[�v1��)�ƃ��[�v���Ԃ��Ƃ��ɕ�����Ȃ̏ꍇ�F
        ���ۂ̉��t���� = qwLength + (dwLoopCount-1)*qwLoop + qwFadeOut
        �������A
            dwLoopCount == 0 �̏ꍇ�AdwLoopCount �͖{�̂̐ݒ�l�ɏ]��
            qwFadeOut == -1 �̏ꍇ�AqwFadeOut �͖{�̂̐ݒ�l�ɏ]��

    ���t���Ԃ͂킩�邪�A���[�v���Ԃ�������Ȃ�(�������[�v���邩������Ȃ�)�ꍇ
        qwLength == ���t����(=�������o���J�n���鎞��),
        qwLoop == -1
        qwFadeOut == 0 or -1 or (�f�[�^���瓾���l)
        dwLoopCount == ����
        ���̂Ƃ��A
        �E�v���C���[�̐ݒ肪�u�P�ȃ��s�[�g�v�ȊO�̍Đ����[�h�̏ꍇ��
          qwLength ���Ԃ����Đ���AqwFadeOut �����t�F�[�h�A�E�g���ĉ��t��ł��؂�
          ���ۂ̉��t���� = qwLength + qwFadeOut
        �E���t���[�h���u�P�ȃ��s�[�g�v�̏ꍇ�� qwLength ���Ԃ����Đ���A
          ���������o�����܂Ŗ������[�v�Đ�����

    ���t���Ԃ͂킩��Ȃ����A�����͉��t�I������ꍇ
    (�������o�ɂ�鉉�t�̑ł��؂肪�s�v�ȏꍇ):
        qwLength == -1, qwLoop == 0, qwFadeOut == 0(����)
        ���̏ꍇ�A�v���C���[���\������Ȃ̒����� ???? �ƂȂ�

    ���t���Ԃ��킩�炸�A�����t�I�����邩���킩��Ȃ��ꍇ�F
    (�������o�ɂ�鉉�t�ł��؂肪�K�v�ȏꍇ):
        qwLength == -1, qwLoop == -1
        ���̂Ƃ��A
        �E�v���C���[���\������Ȃ̒�����
          �f�t�H���g�̋Ȃ̒���(DefaultLength) + �f�t�H���g�̃t�F�[�h�A�E�g����(DefaultFade)�ɂȂ�
        �E�v���C���[�̉��t���[�h���u�P�ȃ��s�[�g�v�łȂ��ꍇ�� DefaultLength ����
          �Đ���ADefaultFade ���Ԃ����ăt�F�[�h�A�E�g���ĉ��t��ł��؂�
        �E���t���[�h���u�P�ȃ��s�[�g�v�̏ꍇ�� DefaultLength �����Đ���A
          ���������o�����܂Ŗ������[�v�Đ�����

    �ŏI�I�ȋȂ̒���(qwTotalLength)��
    1. qwTotalLength = qwLength + (dwLoopCount-1)*qwLoop + qwFadeOut (100ns�P��)
    2. �����T���v���P�ʂɕϊ�(�����_�ȉ��؂�̂�)
       qwTotalSample = kpi_100nsToSample(qwTotalLength, dwSampleRate);
    3. ����100ns �P�ʂɕϊ�(�����_�ȉ��؂�̂�)
       qwTotalLength = kpi_SampleTo100ns(qwTotalSample, dwSampleRate);
       ���̎��_�� 1. �Ƃ͈قȂ����l�ɂȂ邪�A�{���̉��t���ԂƓ����ɂȂ�
    4. �\���́����~���b�P�ʂɕϊ�(�����_�ȉ��l�̌ܓ�)
       qwTotalLengthMs = (qwTotalLength + 5000) / 10000;
    �̂悤�Ȏ菇�ŎZ�o����B

    ���t���Ԃ̃v���C���X�g�ւ̕ۑ������̎菇�Ń~���b�P�ʂŎZ�o�����l���L�^����B
    ���������āA���C�u�������Ԃ��Ȃ̒������T���v���P�ʂ̏ꍇ qwLength �̎Z�o����
    �����_�ȉ��؂�̂�/�l�̌ܓ��̂ǂ����I��ł��Ȃ̒����͓����ƂȂ�B

    ���[�v��(qwLoop != 0) �̉��t�I���̏����͖{�̂��s�����߁A���[�v�Ȃ̏ꍇ��
    Render �̖߂�l����ɑ�2����(dwSizeSample)�Ɠ����ƂȂ�悤�ɂ��邱�ƁB
    �f�R�[�h���C�u�����̎d�l�セ�ꂪ�s�\�ȏꍇ�́A
    IKpiDecoderModule::Open �̑�1����(cpRequest)�� dwLoopCount/qwFadeOut
    �̒l�ɏ]�������������Đ���A�v���O�C�����ŉ��t�I���̏������s������
    ���̏ꍇ�AqwLoop �̒l�� 0 �ɂ��Ȃ���΂Ȃ�Ȃ�(0�ȊO���Ɩ{�̂����[�v�Ȃ�
    ���f���ĉ��t�I���������s�����Ƃ��邽��)
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiDecoder : public IKpiUnknown
{//�I�[�f�B�I�f�[�^�� PCM �Ƀf�R�[�h
 //�v���O�C�������̕��S���y�����邽�߁A�������̃��\�b�h�͌Ăяo���̏�������
 //���܂��Ă���B�uXXX �O(��)�� XXX �������ʂ͖���`�v�Ə����Ă���ꍇ�A����
 //�悤�ȌĂяo��������Ȃ����Ƃ��v���O�C���͑z�肵�ėǂ��B
 //(Select ���Ȃ��� Render �� Seek ���Ă΂�铙)
public:
    virtual DWORD  WINAPI Select(DWORD dwNumber,                    //�Ȕԍ�
                                 const KPI_MEDIAINFO **ppMediaInfo, //may be NULL
                                 IKpiTagInfo *pTagInfo,             //may be NULL
                                 DWORD dwTagGetFlags)=0;//�ǂ̃t�B�[���h�̃^�O���擾���邩�̃t���O
    virtual DWORD  WINAPI Render(BYTE *pBuffer, DWORD dwSizeSample)=0;
    virtual UINT64 WINAPI Seek(UINT64 qwPosSample, DWORD dwFlag)=0;
    virtual DWORD  WINAPI UpdateConfig(void *pvReserved)=0; //�ݒ肪�ύX���ꂽ��Ă΂��
/*
    DWORD WINAPI Select(DWORD dwNumber,
                        const KPI_MEDIAINFO **ppMediaInfo,//may be NULL
                        IKpiTagInfo *pTagInfo,            //may be NULL
                        DWORD dwTagGetFlags)=0;//�ǂ̃t�B�[���h�̃^�O���擾���邩�̃t���O
      �I�Ȃ���(dwNumber=�Ȕԍ� �� 1 �x�[�X�A�Ȕԍ�0 �͑��݂��Ȃ�)
      KbMedia Player �� Render/Seek �̑O�ɕK�� Select ���Ăяo���B

      1�t�@�C��1�Ȃ̏ꍇ�� dwNumber �͏�� 1 ���n�����B
      1�t�@�C�������Ȃ̏ꍇ�� dwNumber �� 0 ���n����邱�Ƃ�����B���̏ꍇ��
      �I�[�v�����Ă���f�[�^�S�̂�1�̋Ȃ܂��̓A���o���ƌ��Ȃ��Ẵ^�O�擾��
      �s���BMedia �G�N�X�v���[���̃��X�g�����Ɂu�t�@�C����::�Ȕԍ��v�̂悤��
      �\�����ꂸ�A�P�Ɂu�t�@�C�����v���\������Ă���Ƃ��̏��ɑ�������B
      (�u�^�C�g���v���A���o����Q�[�����ɂȂ�悤�ȃC���[�W)

      dwNumber=0 �̏ꍇ�̓f�R�[�h�͍s��Ȃ����߁ARender/Seek �͌Ă΂�Ȃ����Ƃ�
      �z�肵�ėǂ��B

      �^�O�擾�ɑΉ�����ꍇ�͂����Ń^�O�擾���s���BID3v1/v2 �� ApeTag ���A
      �{�̂����m�̃^�O�`���ł���΂��̃��[�`���𗘗p���邱�Ƃ��o����B
      IKpiTagInfo(kpi.h) �����킹�ĎQ�Ƃ��邱�ƁB

      IKpiDecoderModule::Open �� cpRequest �� NULL ���n���ꂽ�ꍇ�A�^�O�擾�p
      �ɊJ�����Ƃ��Ӗ����AppMediaInfo �� NULL ���n�����B
      ppMediaInfo==NULL �̂Ƃ��A�^�O��񂾂���Ԃ�(������ NULL �`�F�b�N���K�v)
      ���̏ꍇ�A���ۂɋȂ��؂�ւ�邩�ǂ����͖���`(Render/Seek ����Ȃ�����
      ��z�肵�ėǂ�)

      ��� Media �G�N�X�v���[����ł̃^�C�g���擾���� NULL ���n�����B

      dwTagGetFlags �͎擾����^�O�̃t�B�[���h�BKPI_TAGGET_FLAG_TITLE ����
      �Z�b�g�����B�Z�b�g����Ȃ����͏ȗ����ėǂ��B(�ʒm���Ă��\��Ȃ����A
      �{�̂ɂ���Ė��������)

      �f�R�[�h�p�ɊJ��(cpRequest!=NULL)�ꍇ�� dwTagGetFlags=KPI_TAGGET_FLAG_ALL
      �ƂȂ�A�^�O�擾�p�ɊJ��(cpRequest==NULL)�ꍇ�� dwTagGetFlags �ɍŏ�����
      �t���O�������^������B

      pTagInfo == NULL �̂Ƃ��A�^�O���̎擾���ȗ����邱�ƁB
      �K�� pTagInfo �� NULL �`�F�b�N���s�����ƁB
      �ő��ɂȂ��ċC�Â��ɂ������ߒ��ӂ��邱�ƁB
      CueSheet �ɓo�^���ꂽ�Ȃ̍Đ����� pTagInfo==NULL ���n�����B

      ppMediaInfo!=NULL �̂Ƃ��A*ppMediaInfo �ɋȂ̒�����Đ����g�����̏���
      �����B*ppMediaInfo �̎����͎��� IKpiDecoder �̃��\�b�h���Ă΂��܂ŁB
      �؂�ւ�����̍Đ��ʒu�͖���`(�{�̂� Render �O�ɕK�� Seek ���Ăяo��)
      �؂�ւ��ɐ���������؂�ւ���̋Ȕԍ����A���s������ 0 ��Ԃ��A*ppMediaInfo
      �� NULL ������B

      Select �Ɏ��s��� Render/Seek �������ʂ͖���`�B(�Ă΂�Ȃ����Ƃ�z�肵�ėǂ�)
      ���s���Ă��A���̋Ȕԍ��ł� Select �͉\�Ȃ琬�����邱�ƁB

    DWORD WINAPI Render(BYTE *pBuffer, DWORD dwSizeSample);
      PCM �Ƀf�R�[�h�������ʂ� pBuffer �ɓ����B
      ���ۂɃf�R�[�h�����T���v������Ԃ��B
      ���t���I�������ꍇ�� dwSizeSample ��菬�����l��Ԃ��B
      dwSizeSample �� pBuffer �̃T�C�Y(�T���v���P��)
      dwSizeSample �̓T���v���P�ʂȂ̂ŁA���ۂ̃o�b�t�@�T�C�Y(dwBufferSize)��
      dwBufferSize = dwSizeSample * (dwChannels * ((abs(nBitsPerSample)/8)) �ƂȂ邱�Ƃɒ��ӁB
      KPI_MEDIAINFO::dwUnitSample != 0 �̏ꍇ�́AdwSizeSample �ɂ� KPI_MEDIAINFO::dwUnitSample ���n�����
      KPI_MEDIAINFO::dwUnitSample == 0 �̏ꍇ�́AdwSizeSample �ɂ͔C�ӂ̒l���n�����
      Select �̌Ăяo���O or ���s��� Render �̌��ʂ͖���`�B
      Select ����� Seek �O�� Render �̌��ʂ͖���`�B

    UINT64 WINAPI Seek(UINT64 qwPosSample, DWORD dwFlag);
      qwPosSample �̓V�[�N��ʒu�ŁA�P�ʂ̓T���v���P�ʁB
      ���ۂ̃V�[�N��ʒu(�T���v���P��)��Ԃ�
      ���ۂ̃V�[�N��ʒu�� qwPosSample �����S�Ɉ�v����K�v�͂Ȃ����A
      qwPosSample ��������ɃV�[�N����邱�Ƃ͐������Ȃ��B
      (CueSheet ���Đ����Ɏx����������\�������邽��)
      dwFlag �̓V�[�N�̐��x�ŁA���x�ɂ���ăV�[�N�̏������Ԃ��������ω�����ꍇ��
      dwFlag �̒l�ɂ���ď�������ʂ��邱�ƁB
        0: �v���O�C���̃f�t�H���g�̐��x�ŃV�[�N
        SEEK_FLAGS_SAMPLE�F  �T���v���P�ʂ̐��x�ŃV�[�N(���Ή��̏ꍇ�͐擪�ɖ߂�)
        SEEK_FLAGS_ACCURATE�F�̎������Ɏx�Ⴊ�Ȃ����x�ŃV�[�N(SAMPLE���x���܂߂Ė��Ή��̏ꍇ�͐擪�ɖ߂�)
        SEEK_FLAGS_ROUGH�F   ���x�D��ŃV�[�N(ROUGH ��荂�����x���܂߂Ė��Ή��̏ꍇ�͐擪�ɖ߂�)

      �V�[�N�ɑΉ����Ȃ��ꍇ�͐擪�ɖ߂��� 0 ��Ԃ��B
      �̎����Ȃ��Ȃ��Đ����� SEEK_FLAGS_ROUGH
      �̎������݂���Ȃ��Đ����� SEEK_FLAGS_ACCURATE
      CueSheet �Đ����� SEEK_FLAGS_SAMPLE ���w�肵�ăV�[�N����B
      �w�肵���ȏ�̐��x�ŃV�[�N���邱�ƁB(�s�\�Ȃ�擪�ɖ߂�����)
      (ROUGH �w�莞�� SAMPLE ���x�� OK �����ASAMPLE �w�莞�� ROUGH ���x�� NG)

      Select �̌Ăяo���O or ���s��� Seek �̌��ʂ͖���`�E

      dwFlag �͎��͖{�̂������ɖ�����(���G
      ��� SEEK_FLAGS_SAMPLE �����n���Ă��炸�A���J���̃v���O�C���̂قڑS�Ă�
      dwFlag �����Ă��Ȃ��B

    DWORD WINAPI UpdateConfig(void *pvReserved);
      IKpiDecoderModule::ApplyConfig �̌Ăяo���� KPI_CFGRET_NOTIFY ���Ԃ��ꂽ
      �ꍇ�̂݁A�{�̂ɂ���ăf�R�[�_�̃C���X�^���X���ƂɌĂ΂��B
      �Ă΂ꂽ���_�Őݒ�͊��ɕύX�ς݂ł���A�ǂ̐ݒ肪�ύX���ꂽ����m�邱��
      �͏o���Ȃ��B
      pvReserved �͏����̗\��(NULL ���n�����)
      �߂�l�̈Ӗ��͏����̗\�� => 0 ��Ԃ����ƁB
      IKpiDecoderModule::ApplyConfig �����킹�ĎQ�Ƃ��邱�ƁB
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiDecoderModule : public IKpiUnknown
{
public:
    virtual void  WINAPI GetModuleInfo(const KPI_DECODER_MODULEINFO **ppInfo)=0;
    virtual DWORD WINAPI Open(const KPI_MEDIAINFO *cpRequest,//�Đ����g�����̗v���l���܂܂ꂽ KPI_MEDIAINFO
                              IKpiFile     *pFile,   //���y�f�[�^
                              IKpiFolder   *pFolder, //���y�f�[�^������t�H���_
                              IKpiDecoder **ppDecoder)=0;
    virtual BOOL  WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator)=0;
    virtual DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//�Z�N�V������
                                     const wchar_t *cszKey,    //�L�[��
                                     INT64  nValue, //�ύX��̒l(BOOL/INT �̏ꍇ)
                                     double dValue, //�ύX��̒l(FLOAT �̏ꍇ)
                                     const wchar_t *cszValue)=0;//�ύX��̒l(������)
/*
    void  WINAPI GetModuleInfo(const KPI_DECODER_MODULEINFO **ppInfo);
      KPI_DECODER_MODULEINFO �� *ppInfo �ɐݒ肵�ĕԂ�
      KPI_DECODER_MODULEINFO �͋��v���O�C���� KMPMODULE �ɑ�������
      ���� IKpiDecoderModule �̃��\�b�h((GetModuleInfo/Open/EnumConfig/ApplyConfig �̂����ꂩ)
      ���Ăяo���ꂽ���_�� *ppInfo �̓��e���j������ėǂ�
      �Ⴆ�� ApplyConfig �Ăяo����ɑΉ��g���q���ω������ꍇ�A�ω��������_��
      *ppInfo �̓��e�������ς���ėǂ�
      GetModuleInfo �� ApplyConfig �Ăяo����ɖ{�̂���ēx�Ă΂��

      IKpiConfig �ɑΉ�����ꍇ�Akpi_CreateConfig �ɂ�� IKpiConfig �̃C���X�^���X
      �擾�́A�x���Ƃ� kpi_CreateInstance ���Ŏ��s���邱�ƁB
      kpi_CreateInstance ����߂����i�K�Ŗ��� kpi_CreateConfig ���Ă΂�Ă��Ȃ�
      �ꍇ�A�ݒ�̕ۑ���{�̂ɊǗ������邱�Ƃ͏o���Ȃ��B
      (EnumConfig/ApplyConfig �͗L�������o�C�i���ȊO�̌^�͈����Ȃ�)

    DWORD WINAPI Open(const KPI_MEDIAINFO *cpRequest,//�Đ����g�����̗v���l���܂܂ꂽ KPI_MEDIAINFO
                              IKpiFile     *pFile,   //���y�f�[�^
                              IKpiFolder   *pFolder, //���y�f�[�^������t�H���_
                              IKpiDecoder **ppDecoder);
      pFile/pFolder ����Ȃ��J���B
      ���������� IKpiDecoder �̃C���X�^���X�� *ppDecoder �ɐݒ肵�A�܂܂�Ă���
      �Ȃ̐���Ԃ��B(�G���[�̏ꍇ�� *ppDecoder �� NULL ��ݒ肵�� 0 ��Ԃ�)

      ���t�@�C��������J�����\�b�h�͒񋟂���Ȃ��B
      ���t�@�C��������J�������ꍇ�� IKpiFile::GetRealFileW/A ���Ăяo���B

      cpRequest : �ȉ��̊e�l�� 0 �ȊO�������ꍇ�͗v���l�B�\�Ȃ�v���l�ɍł�
                  �߂��`�ŉ�����B(�v���𖳎����Ă��\��Ȃ�)
      �EdwSampleRate   : �Đ����g��
      �EnBitsPerSample : �r�b�g��(�ʏ�� -64 ���n�����)
      �EqwFadeOut      : �t�F�[�h�A�E�g����
      �EdwLoopCount    : ���[�v��
      dwSampleRate �͖������ĉ�����K�v�͂Ȃ��B
      nBitsPerSample �̓f�R�[�_���Ή����钆�ōł��������Z���x��Ԃ����ƁB
      16bit �̐��x�����Ȃ��̂� -64 �ɑΉ�����K�v�͂Ȃ��B
      ���[�v�ȂɑΉ����邪�A�f�R�[�h���C�u�����̎d�l�㉉�t�I���̏�����{�̂�
      ���䂳���邱�Ƃ��o���Ȃ��ꍇ�� dwLoopCount/qwFadeOut �̒l���牉�t���Ԃ�
      �Z�o���A�v���O�C�����ŉ��t�I���̏������s�����ƁB

      cpRequest == NULL �̏ꍇ�̓^�O�擾�p�ɊJ���B
      (��� Media �G�N�X�v���[���ł̃^�C�g���\���p)
      IKpiDecoder::Select �� ppMediaInfo �ɂ� NULL ���n�����悤�ɂȂ�B
      ���̏ꍇ�̓t�@�C���I�[�v���̏������ŏ����ɗ}���邱�ƁB
      (�f�[�^�̗L�����̃`�F�b�N���ȗ����Ă��ǂ�)
      IKpiTagInfo �ւ̏��ʒm���^�C�g���ƃA�[�e�B�X�g�����ŗǂ��B
      (�^�C�g��/�A�[�e�B�X�g�ȊO�̏���ʒm���Ă��\��Ȃ������������)

      KPI_DECODER_MODULEINFO::dwSupportTagInfo==0 �̏ꍇ�� Media �G�N�X�v���[��
      �ł̏��擾���� Open ���Ă΂Ȃ��B�^�C�g���擾�͖{�̂ɔC���Ēǉ���񂾂���
      �擾����ꍇ�� dwSupportTagInfo �� 0 �ɂ��邱�Ƃ� Media �G�N�X�v���[����
      �p�t�H�[�}���X�ቺ��}���邱�Ƃ��o����B

    BOOL WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator);
      �ݒ荀�ڂ�񋓂���B
      �񋓂��ꂽ�ݒ荀�ڂ����ɖ{�̂��ݒ��ʂ�������������B
      �ݒ荀�ڂ��Ȃ��ꍇ�� FALSE ���A����ꍇ�� TRUE ��Ԃ��B
      KPI_DECODER_MODULEINFO::dwSupportConfig=0 �̏ꍇ�͌Ăяo����Ȃ��B
      KPI_CFG_SECTION/KEY �\���̋y�� IKpiConfigEnumerator �����킹�ĎQ�ƁB
      kpi_CreateInstance ����߂�܂ł� kpi_CreateConfig ���Ă΂�Ȃ��ꍇ�A
      �񋓂��Ă��ݒ肪�ۑ�����Ȃ��B(�o�C�i���̌^�͗�O)

    DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//�Z�N�V������
                             const wchar_t *cszKey,    //�L�[��
                             INT64  nValue, //�ύX��̒l(BOOL/INT �̏ꍇ)
                             double dValue, //�ύX��̒l(FLOAT �̏ꍇ)
                             const wchar_t *cszValue)=0;//�ύX��̒l(������)
      cszSection �� cszKey �ɊY������ݒ�̕ύX���u�K�p�v���钼�O�ɌĂ΂��B
      �Ă΂ꂽ���_�ł͂܂� m_pConfig �͕ύX����Ă��Ȃ��B
      ������ m_pConfig �Ƃ� kpi_CreateConfig �Ŏ擾���� IKpiConfig ��
      �C���X�^���X�Ƃ���B

      kpi_CreateInstance ����߂�܂ł� kpi_CreateConfig ���Ă΂�Ȃ��ꍇ�A
      �o�C�i���ȊO�̌^�ł� ApplyConfig ���Ă΂�Ȃ��B(�ݒ�̕ۑ��ꏊ�����肷��
      ���Ƃ��o���Ȃ����߁BApplyConfig ����kpi_CreateConfig ���ĂׂΗǂ��ƍl����
      ��������Ȃ����A�ݒ��ʏ�Ō��݂̐ݒ�l���m�F���邱�Ƃ��o���Ȃ�)

      �ݒ���u�K�p�v����̂̓v���O�C�����g�ł���BSetXXX ���Ȃ��܂� ApplyConfig
      ����߂����ꍇ�A�ݒ�͉����ς��Ȃ����Ƃɒ��ӁB

      nValue/dValue/cszValue �ɓn�����l�� EnumConfig �ŗ񋓂��ꂽ��������
      �����͈͓��Ɏ��܂��Ă��邱�Ƃ��ۏ؂����B
      EnumConfig �ł͕\���o���Ȃ��悤�Ȕ͈͂����ꍇ�͂����� nValue ��������
      ���A�C���̕K�v������΃v���O�C������ SetXXX ���Ăяo���B

      �{�̑��ł͈̔̓`�F�b�N�ŏ\���ŁAnValue/dValue/cszValue �̌��؂��s�v��
      �ꍇ�́A�n���ꂽ cszValue �����̂܂� SetStr ����Ηǂ��B

      m_pConfig->SetStr(cszSection, cszKey, cszValue);//cszValue �ɕύX���Ȃ��ꍇ

      cszKey �ɋ󕶎���(��q)���n���ꂽ�ꍇ�����̂܂� SetStr ���Ė��Ȃ��B
      (�{�̂ɂ���Ė��������)

      �n���ꂽ�l�̏C�����K�v�ɂȂ�̂́A�Ⴆ�Έȉ��̂悤�ȏꍇ�ł���B

      �E������n�ő啶�����������ɕϊ�������
      �E�ŏ��l�`�ő�l�͈͓̔��ł����Ă��s���ƂȂ�悤�Ȓl�����݂���
      �E�l�̐������͕����̃L�[�̑g�ݍ��킹�Ō��܂�

      ���؂̌��ʁA�l�ɕύX������ꍇ�́A�Y���L�[�̌^�ɓK������ Set �n���\�b�h��
      �Ăяo���B

      m_pConfig->SetInt(cszSection, cszKey, nValue);//BOOL/INT
      m_pConfig->SetFloat(cszSection, cszKey, dValue);//FLOAT
      m_pConfig->SetStr(cszSection, cszKey, cszValue);//BOOL/INT/FLOAT/BIN �ȊO

      �K�v�ł���΂��̊֐����ŊY���Z�N�V����/�L�[�Ɋւ���v���O�C���Ǝ���
      �ݒ��ʂ��o�����Ƃ��o����(�ʏ�� KPI_CFG_TYPE_BIN �ŕK�v)

      KPI_CFG_TYPE_BIN �Ŗ{�̂ɐݒ�̕ۑ����Ǘ�������ꍇ�ɂ͓Ǝ��̐ݒ��ʂ�
      �u�K�p�v�����Ƃ��� SetBin ���Ăяo���Ηǂ��B
      �Ǝ��̐ݒ��ʂ�\�����钼�O�� GetBin ����΁A�����_�Ŗ{�̂��ێ����Ă���
      �ݒ�f�[�^�̃o�C�i���l���擾���邱�Ƃ��o����B

      ApplyConfig �̓Z�N�V�����P�ʂŌĂ΂��B���ۂɌĂяo���̂́A�ݒ��ʏ��
      �l���ύX���ꂽ�L�[�݂̂ł��邽�߁A�Ăяo���ꂽ��l���ς���Ă���ƍl����
      �����x���Ȃ��B

      �{�͓̂���Z�N�V�������ŁA�l���ύX���ꂽ�L�[�݂̂� EnumConfig �ŗ񋓂��ꂽ
      ���ɌĂяo���B�ύX���ꂽ�S�ẴL�[�ɑ΂��� ApplyConfig ���Ăяo������A
      �Ō�� cszKey �ɋ󕶎��񂪓n�����B

      �Ⴆ�΁AcszSection1 �� cszKey1, cszKey2, cszKey3 �����̏��� EnumConfig
      ����Ă���Ƃ��� cszKey1 �� cszKey3 ���ύX���ꂽ��ԂŁu�K�p�v�{�^����
      �����ꂽ�Ƃ����

      ApplyConfig(cszSection1, cszKey1, ...);
      //ApplyConfig(cszSection1, cszKey2, ...);//�ύX����Ă��Ȃ��̂ŌĂ΂�Ȃ�
      ApplyConfig(cszSection1, cszKey3, ...);
      ApplyConfig(cszSection1, L"", ...);//��3�����ȍ~�͖������邱�ƁB
      //ApplyConfitg(cszSection2, ...);//�ݒ�̓K�p�̓Z�N�V�����P�ʂȂ̂ł����ł͌Ă΂�Ȃ�

      �̂悤�ȏ��ŌĂяo���B

      ApplyConfig ���ł́AcszSection/cszKey �ȊO�̔C�ӂ̃Z�N�V�����E�L�[
      (EnumConfig �ŗ񋓂���Ȃ��Z�N�V����/�L�[���܂�)�ɑ΂��� SetXXX/GetXXX
      ���邱�Ƃ��o����B

      ��������Z�N�V�������̕����̃L�[�̑g�ݍ��킹�ōŏI�I�Ȑ������l���m�肷��
      �悤�ȏꍇ�́AcszKey �ɋ󕶎��񂪓n���ꂽ���_�ő��̃L�[�ƍ��킹�� SetXXX
      ������Ηǂ��B�l�̈�ѐ������ɂȂ�ꍇ�� EnumConfig �ł͗񋓂���Ȃ���
      �̃L�[�ɕۑ�����Ƃ������@������B�Ⴆ�ΐݒ��ʏ�ł� 3 ��̃L�[�� BOOL
      �̂悤�Ɏg�������̕ۑ��l�͕ʂ̃L�[(�ݒ��ʏ�ɂ͔�\��)�ɂ���ȂǁB

      SetXXX ���Ă΂�Ȃ��ꍇ�A�ݒ�̕ύX�͓K�p����Ȃ����Ƃɒ���(�v���O�C����
      �Ǝ��ɐݒ�̕ۑ��Ǘ����s���ꍇ�͏���)

      �߂�l�͐ݒ�̕ύX�����f�����^�C�~���O�ł���B�ȉ��̒l��Ԃ�����

      KPI_CFGRET_OK: �ݒ�͑����ɔ��f�����(IKpiXXX::UpdateConfig() �͌Ă΂�Ȃ�)
      KPI_CFGRET_NOTIFY: �e��C���X�^���X�ւ̒ʒm���K�v
                         (IKpiXXX::UpdateConfig()���Ă΂��)
      KPI_CFGRET_RELOAD_DATA: �f�[�^�̍ă��[�h���K�v
                              (���� IKpiXXXModule::Open �ォ�甽�f�����)
      KPI_CFGRET_RELOAD_KPI:  �v���O�C���̍ă��[�h���K�v
      KPI_CFGRET_REBOOT_PLAYER:�v���C���[�̍ċN�����K�v(���ʂȎ���Ȃ�����
                               RELOAD_KPI �ő���锤����)
      KPI_CFGRET_REBOOT_OS:    OS �̍ċN�����K�v
      KPI_CFGRET_UNKNOWN:   �s��(�O���v���O�������Ăяo�����A�ݒ�֌W�̈ˑ�
                            �R�[�h�̏ڍׂ�������Ȃ���)

      KPI_CFGRET_NOTIFY ��Ԃ����ꍇ�́AIKpiXXXModule::Open �Ŋl������
      �e�C���X�^���X(IKpiXXX)�ɑ΂��Ė{�̂� UpdateConfig ���Ăяo���B
      ���m�ɂ� ApplyConfig �̓s�x�ł͂Ȃ��A�Ō�� cszKey �ɋ󕶎����n����
      �Ăяo������ɌĂяo���BcszKey �ɋ󕶎����n�����Ƃ��̖߂�l�������画�f
      ����̂ł͂Ȃ��A�Ō�� ApplyConfig �̌Ăяo���܂łɈ�x�ł� CFGRET_NOTIFY
      ���߂�ΌĂяo���B

      UpdateConfig �ɑΉ����邱�ƂŁA�f�R�[�h���Ƀ��A���^�C���ɐݒ�ύX�𔽉f
      �����邱�Ƃ��o���邪�A���f�����܂łɂ����鎞�Ԃ͖{�̂́u��ǂ݃o�b�t�@�v
      �̐ݒ�ɂ��B(����l�� 5 �b�̂��߁A5 �b���x�������Ă��܂�)

      ApplyConfig ���Ă΂��̂Ƃ͕ʃv���Z�X�� IKpiXXX �����쒆�̏ꍇ�A
      ������� IKpiXXX �ɑ΂��Ă� UpdateConfig ���Ăяo�����B

      ���̏ꍇ�A�Ăяo���ꂽ IKpiXXX ���ł� ApplyConfig ���Ă΂�Ȃ����ƂɂȂ�
      ���Akpi_CreateConfig �Ŏ擾���� IKpiConfig �͑S�Ẵv���Z�X�ԂŐݒ肪���L
      ����Ă���B

      ���̂��� IKpiXXX::UpdateConfig �ɑΉ�����ꍇ�� IKpiXXX �̊e�C���X�^���X
      �ɑ΂��Ă� m_pConfig ��n���Ă����A�ݒ�̎擾���K�v�ȏꍇ�� m_pConfig ����
      �擾����悤�ɂ��Ȃ���΂Ȃ�Ȃ��B

      ApplyConfig ���Ƀv���O�C�������Őݒ��ύX�A���̐ݒ�l�� IKpiXXX ���Q��
      ����悤�ȕ����Ŏ�������ƁAApplyConfig ���Ăяo�����̂Ƃ͕ʃv���Z�X��
      IKpiXXX �����삵�Ă����ꍇ�ɔ��f����Ȃ����ƂɂȂ�B

    IKpiDecoderModule �̃C���X�^���X�쐬�Ɣj���̓v���O�C�������[�h����Ă����
    ��x�����s�Ȃ���B

    �K�v�ȏ������̓C���X�^���X�쐬���A��n���̓C���X�^���X�j�����ɍs�����ƁB

    �{�̂̓v���O�C�������[�h�� Open �܂ōs�킸�A�Œ���̏�񂾂��擾��ɃA�����[�h
    ���邱�Ƃ�����B�Ⴆ�΋N�����Ƀv���O�C�������߂ĔF�������ꍇ�� GetModuleInfo
    �����Ăяo����ɃA�����[�h����B�ݒ��ʂ̕\�����A�v���O�C�����g�p���łȂ����

    GetModuleInfo/EnumConfig/ApplyConfig

    �����Ă΂��A�ݒ��ʂ�����Ƃ���ŃA�����[�h����B

    �����v���O�C���̏������Ɏ��Ԃ�������ꍇ�� Open ���ɏ�������1�x�������s����ȂǁA
    �v���O�C�����œƎ��ɍH�v���邱�ƁB���̍ۂ͖��������̂܂� IKpiDecoderModule
    ��j�����ꂽ��A�ǂ̃��\�b�h���ŏ��ɌĂ΂�Ă�����ɖ�肪�N����Ȃ��悤��
    ���ӂ��邱�ƁB
*/
};
///////////////////////////////////////////////////////////////////////////////
//�f�R�[�_�v���O�C�����ʂ̃f�R�[�_�v���O�C�����g�p���邽�߂̃w���p�[�֐�
//
//  cpRequest �� KPI_MEDIAINFO::nBitsPerSample �� 0 �ȊO������ƃf�R�[�_��
//  �Ԃ��r�b�g�����v���ʂ�ɂȂ�(�{�̂������ŗv�����ꂽ�r�b�g���ɕϊ�����)
//
//  kbrunkpi.exe �o�R�̏ꍇ�͖�����(�K�����s����)
//
///////////////////////////////////////////////////////////////////////////////
extern DWORD WINAPI kpi_CreateDecoder(IKpiUnkProvider *pProvider,
                                      const KPI_MEDIAINFO *cpRequest,//�Đ����g�����̗v���l���܂܂ꂽ KPI_MEDIAINFO
                                      IKpiFile     *pFile,   //���y�f�[�^
                                      IKpiFolder   *pFolder, //���y�f�[�^������t�H���_
                                      IKpiDecoder **ppDecoder);
extern DWORD WINAPI kpi_CreateDecoder(IKpiUnknown *pUnknown, //kpi_CreateInstance �̑�3�������璼�ڎ擾
                                      const KPI_MEDIAINFO *cpRequest,//�Đ����g�����̗v���l���܂܂ꂽ KPI_MEDIAINFO
                                      IKpiFile     *pFile,   //���y�f�[�^
                                      IKpiFolder   *pFolder, //���y�f�[�^������t�H���_
                                      IKpiDecoder **ppDecoder);
//pProvider ���� IKpiDecoder �̃C���X�^���X���擾
//pProvider->CreateInstance(IID_IKpiDecoder, (void*)cpRequest, pFile, pFolder, NULL, (void**)ppDecoder);
//�Ɠ���
//kpi_CreateInstance �̑�3�������璼�� IKpiDecoder ���擾���邱�Ƃ��o����
//
///////////////////////////////////////////////////////////////////////////////
//�f�R�[�_�v���O�C���g�p���p�̃w���p�[�֐�(�f�R�[�_�v���O�C���쐬�ɂ͕s�v)
//�v���O�C���g�p��(�ʏ�� KbMedia Player)���Ăяo���ꍇ�� kpi_decoder.cpp ��
//�v���W�F�N�g�ɒǉ�����
///////////////////////////////////////////////////////////////////////////////
extern HRESULT WINAPI kpi_CreateDecoderModule(pfn_kpiCreateInstance fnCreateInstance,
                                              IKpiDecoderModule **ppModule, IKpiUnknown *pUnknown);
///////////////////////////////////////////////////////////////////////////////
//�v���O�C���g�p��/�쐬���̃w���p�[�֐�
//inline �֐��Ƃ��Ē�`���Ă���̂� kpi_decoder.cpp ���v���W�F�N�g�ɒǉ�����K�v�͂Ȃ�
///////////////////////////////////////////////////////////////////////////////
inline UINT64 kpi_100nsToSample(UINT64 qw100ns, DWORD dwSampleRate)
{//100�i�m�b�P�� => �T���v���P��(�����_�ȉ��؂�̂�)
    return qw100ns * dwSampleRate / (1000*10000);
}
inline UINT64 kpi_SampleTo100ns(UINT64 qwSample, DWORD dwSampleRate)
{//�T���v���P�� => 100�i�m�b�P��(�����_�ȉ��؂�̂�)(0 �ŏ��Z�̃`�F�b�N�Ȃ�)
    return qwSample * (1000*10000) / dwSampleRate;
}
inline UINT64 kpi_MsTo100ns(UINT64 qwMs)
{//�~���b�P�� => 100�i�m�b�P��
 //DWORD �ŉ��Z����� 7��������������������ŃI�[�o�[�t���[
    return qwMs * 10000;
}
inline void kpi_InitMediaInfo(KPI_MEDIAINFO *pMediaInfo)
{
    ZeroMemory(pMediaInfo, sizeof(KPI_MEDIAINFO));
    pMediaInfo->cb = sizeof(KPI_MEDIAINFO);
}
///////////////////////////////////////////////////////////////////////////////
#pragma pack(pop) //�\���̂̃A���C�������g
#endif
