#pragma once
#ifndef kpiH
#define kpiH

#pragma pack(push, 1) //�\���̂̃A���C�������g
/*

  KbMedia Player �v���O�C�� SDK (2017/03/06��)(�{��Ver 3.02 �p)

  �Ekmp_pi.h: ���d�l�̃v���O�C�����쐬�E�g�p���ɕK�v
              IKpiTagInfo �̒�`�ς� cszName ���g�������ꍇ���K�v
              ���d�l�̃v���O�C���� 64bit �ł��܂߂ĐV�d�l�̐��������[�X���
              ���������Ή����邪�A���񂪑������ߐV�d�l�ւ̈ڍs�𐄏�
              (���d�l�̕������̂͊ȒP����)
  �Ekpi.h: �S�Ẵv���O�C���ŕK�v(���d�l������)
  �Ekpi_decoder.h: �f�R�[�_�v���O�C���쐬�E�g�p���ɕK�v
                   ���t�v���O�C���쐬�E�g�p���ɂ��K�v
  �Ekpi_player.h:  ���t�v���O�C���쐬�E�g�p���ɕK�v
  �Ekpi_unarc.h;   �A�[�J�C�u�v���O�C���쐬�E�g�p���ɕK�v
  �Ekpi_device.h:  �f�o�C�X�v���O�C���쐬�E�g�p���ɕK�v
  �Ekpi_impl.h:    IKpiUnknown �̔h���N���X�� QueryInterface/AddRef/Release
                   �̎������ʓ|�ȏꍇ�ɕK�v(�g�p�͕K�{�ł͂Ȃ�)
  �E�ȉ��͊e��v���O�C���֌W�̃w���p�[�֐��g�p���Ƀv���W�F�N�g�ւ̒ǉ���
    �K�v�ɂȂ邱�Ƃ�����(�K�{�ł͂Ȃ�)
    �Ekpi.cpp: kpi_CreateConfig ���̃w���p�[�֐��g�p��(IKpiConfig �ɑΉ�����
               �ꍇ�͂قڕK�{)
    �Ekpi_decoder.cpp: �f�R�[�_�v���O�C���֌W�̃w���p�[�֐��g�p��
    �Ekpi_player.cpp:  ���t�v���O�C���֌W�̃w���p�[�֐��g�p��
    �Ekpi_unarc.cpp:   �A�[�J�C�u�v���O�C���֌W�̃w���p�[�֐��g�p��
    �Ekpi_device.cpp:  �f�o�C�X�v���O�C���֌W�̃w���p�[�֐��g�p��

  �Ƃ肠�����V�����d�l�̃v���O�C��������Ă݂�ꍇ�́A

  IKpiFolder/IKpiTagInfo/IKpiConfig/IKpiUnkProvider �͖���
  IKpiFile::GetRealFileW �Ŏ��t�@�C�����擾

  �ł�����x�̂��͍̂쐬�\�Bkpi.h �͏n�Ǖs�v�Bkpi_decoder.h ���ŏ���
  �ǂ񂾕����������₷�������B

  ���t�@�C��������t�@�C�����J���ꍇ�A�J�����t�@�C�������܂� IKpiFile ��
  Release ���Ă͂����Ȃ����Ƃɒ��ӁB


  �X�V����

  2017/03/06:
    �Ekpi_unarc.h(�A�[�J�C�u�v���O�C���p)���C��
    �E�C���^�[�t�F�[�X�͕ύX�Ȃ�(GUID ���ύX�ɂȂ��Ă�̂� v3.01�ȑO�ł͎g�p�s��)
    �Ekpi_CreateLocalFile/kpi_CreateMemoryFile ��ǉ�
    �EIKpiUnArcModule::Open �� cszOutputPath �ȉ��ɓW�J���ʂ����t�@�C���o�͂����ꍇ�A
      �A�[�J�C�u�v���O�C�����g�p���鑤(�ʏ�� KbMedia Player)���K�v�ɉ����č폜����悤�ɂ���
  2017/01/16:
    �E���ɂȂ�(�����Ń����[�X�ɍ��킹�ĒǋL��������)
  2017/01/09:
    �Ekpi_decoder.h/cpp: kpi_CreateDecoder ��{�̑��Ŏ���
    �E���̃r�b�g���v���Ɋւ���L�q��ǉ�(�{�̂ɂ���ėv���r�b�g���ɕϊ������)
    �Ekpi_impl.h: KbKpiNullFolder(�����������Ȃ� IKpiFolder) ��ǉ�
    �Ekpi_device.h: IKpiOutDevice::Output �ɖ߂�l��ǉ�
  2016/10/23:
    �Ekpi_CreateConfig �Ńv���O�C���̌Ăяo����(�ʏ�� KbMedia Player �{��)��
      IKpiConfig ���Ή��̏ꍇ�͏�Ƀf�t�H���g�l��Ԃ� IKpiConfig ��Ԃ��悤��
      ����(kpi.cpp)
    �EIKpiConfig ���Ȃ��Ă����삷��悤�ɍ쐬����̂��ʓ|�ȏꍇ�ɑΉ�
  2016/10/10:
    �Ekpi_device.h::KPI_DEVICE_FORMAT::dwFormatType ��ǉ�(DoP �Ή��p)
    �E�f�R�[�_�v���O�C���͕ύX�Ȃ�
  2016/10/02:
    �EKPI_DECODER_MODULEINFO::cszSupportCodecs/cszSupportSupportContainers ��p�~
      (pvReserved1/pvReserved2 �ɕύX)
    �EKPI_DECODER_MODULE_VERSION �� 200 �ɕύX
    �E�f�R�[�_�v���O�C���d�l�͂���Ŋm��(�̗\��)
  2016/09/12:
    �EIKpiTagInfo::SetPicture �̎d�l��ύX
    �EIKpiTagInfo::GetTagInfo �̑�3����(dwType)���C��
      �EID3v1 �� ID3v2 �� Lyrics3Tag �����
  2016/08/22:
    �EIKpiDecoder/IKpiDecoderModule �� kpi_decoder.h �Ɉړ�
    �EIKpiTagInfo �Ƀ��\�b�h�ǉ�
        DWORD WINAPI GetTagInfo(IKpiFile *pFile, IKpiFolder *pFolder,
                                DWORD dwType, DWORD dwTagGetFlags);//�{�̂������Ă���^�O�擾���[�`�����Ăяo��
        DWORD WINAPI GetValue(const wchar_t *cszName, wchar_t *pszValue, int nSize);
        void  WINAPI SetOverwrite(BOOL bOverwrite);//�㏑�����[�h�̐؂�ւ�
        void  WINAPI SetPicture(const wchar_t *cszName,   //"front", "back", etc...
                                const wchar_t *cszFormat, //"jpg", "png", "gif", "bmp", etc
                                const BYTE *pPictureData, DWORD dwSize);
    �EIKpiDecoder::Select/IKpiPlayer::Select �̑�4�����Ƀ^�O�擾����t�B�[���h
      ���w�肷��t���O��ǉ�
        DWORD  WINAPI Select(DWORD dwNumber,                    //�Ȕԍ�
                             const KPI_MEDIAINFO **ppMediaInfo, //may be NULL
                             IKpiTagInfo *pTagInfo,             //may be NULL
                             DWORD dwTagGetFlags);//�ǂ̃t�B�[���h�̃^�O���擾���邩�̃t���O
  2016/08/16:
    �E�Z�N�V�����̕\�����ƃw���v�ɑΉ�
      �EKPI_CFG_SECTION �ǉ�
      �EKPI_CFG_INFO => KPI_CFG_KEY �ɕύX
      �EIKpiConfigEnumerator �̃��\�b�h�ǉ�
      �EIKpiConfig �� uuid(IID_IKpiConfig) ��ύX(�݊������Ȃ��Ȃ�������)
    �Ekpi_CreateConfig �Ăяo���̗L��������ǉ�
      �Ekpi_CreateInstance ����߂�܂łƂ���
  2016/08/08:
    �EIKpiConfig(�ݒ�̉�ʐ����ƕۑ��Ǘ�)��啝�C��������
    �E���ɔ������\�b�h(IKpiDecoder::UpdateConfig)�̒ǉ�
    �E�A�[�J�C�u/�f�o�C�X/���t(��ɓ���p)�֌W�̃v���O�C����
    �E�Ή��g���q�����C��(KPI_DECODER_MODULEINFO::cszSupportExts ��)
    �EIKpiFile::GetBuffer �̑�2�����̌^�� DWORD => size_t �ɕύX
      (64bit �ł� 4GB �̐������Ȃ�������)
    �E�萔(IKpiFile::GET_BUFFER_MAXSIZE) ��ǉ�
      (IKpiFile::GetBuffer ��{�̂��G�~�����[�g�����Ƃ��̃T�C�Y�̐���)
    �E�G�N�X�|�[�g����֐��̖��O��ύX
      kpi_CreateDecoderModule => kpi_CreateInstance
    �E���̐����ł܂łɂ�肽�����Ƃ� 80% ���炢�͎����o��������
      (�c��� 20% �ɑS�̂�80%(���܂ł�4�{)�̎��Ԃ��₵�����ȋC�����邪)
    �E�^�O�擾���肪�C�ɓ���Ȃ��c�BOpen/Select ���̈����̈Ӗ��Ƃ������ꂵ���B

  2015/12/28: pfnCreateDecoderModule �̖߂�l���C��(BOOL* => BOOL)


  ���v���O�C���d�l(kmp_pi.h)�̌��_
    �E���[�v�Ȃɖ��Ή�
    �E�P�t�@�C�������Ȃւ̑Ή����s���S
      �E�{�̂��m���Ă���g���q�ɂ����Ή��o���Ȃ�
      �E�Ȃ̒������擾�o���Ȃ�(���ׂĖ������[�v����ȂƂ��Ĉ�����)
      �E�V�[�N�ɑΉ��o���Ȃ�
    �E�O���t�@�C�����Q�Ƃ���`���̃A�[�J�C�u�Ή����s���S
      �E�Q�ƃt�@�C���̎擾���@��{�̂��m���Ă���K�v������
      �E�{�̂��m��Ȃ��g���q�̃v���O�C���ɑ΂��ẮA�Q�ƃt�@�C���̎擾��
        �s�v�ł��A�{�͎̂Q�ƃt�@�C����(�����ȕ��@��)�擾���悤�Ƃ��邽�߁A
        ������I�ł���
    �E�t�@�C������̓ǂݍ��݂ɂ����Ή��o���Ȃ����߁A�A�[�J�C�u���̃f�[�^���A
      �t�@�C���Ƃ��đ��݂��Ȃ��f�[�^���Đ�����̂ɂ��K���ꎞ�t�@�C�����쐬
      ���Ȃ���΂Ȃ�Ȃ�
    �E�}���`�`�����l�����Ή�
  �V�v���O�C���d�l����̖ڕW
    �E��L���_������
    �E�A�[�J�C�u�֌W�̃v���O�C����
    �E�o�̓f�o�C�X�֌W�̃v���O�C����
    �E����֌W�̃v���O�C����
    �E�o���邾���p�t�H�[�}���X���ቺ���Ȃ��悤��
    �E�o���邾���ȒP�Ƀv���O�C�����J���o����悤��(�c�O�Ȃ��狌�d�l�Ɣ�ׂ�ƕ��G�œ��)
    �E�o���邾���ȒP�Ƀv���O�C�����g�p�o����悤��(���d�l�Ɣ�ׂ�Ɛ��i���)
    �EC++ �ȊO�̌���ł��J���o����悤��
      �E���(C++ => Delphi/C �w�b�_�ւ̃R���o�[�^�����݂���Ή����H)
    �E�g���̗]�n���c��
    �E�K�v�ȏ�ɗ~����߂��Ȃ�
    �E�ȉ�������Ă݂��������
      �E�G�t�F�N�g�֌W�̃v���O�C����
      �EWAVE/AVI/OGG/Matrosuka ���A�R���e�i�Ή��̃v���O�C����
      �EMIDI �֌W�ɂ��v���O�C���őΉ�
      �E�t�@�C���R���o�[�^�֌W�̃v���O�C����(MML=>MID��)
      �E����̉f���������܂߂��v���O�C����

*/
/*
    �C���^�[�t�F�[�X(class IKpiXXX)

    IKpiUnknown : �S�ẴC���^�[�t�F�[�X�� IKpiUnknown ����h������
                  IUnknown �� typedef �Ȃ̂� IUnknown �Ɠ���
                  QuearyInterface �Ŋg���̗]�n���c��
                  Release ���Ă΂ꂽ�璼���ɔj������̂ł͂Ȃ��A������
                  �Q�ƃJ�E���g���������ăJ�E���^�� 0 �ɂȂ�����j������
                  �������ʓ|�Ȃ� kpi_impl.h �Œ�`���Ă���N���X����h��
                  �����Ă��ǂ�(�K�{�ł͂Ȃ�)

    ��ɖ{�̑��Ŏ���(IKpiFile �̂݃A�[�J�C�u�v���O�C���ł�����)
    IKpiFile    : �X�g���[������̃t�@�C���̓ǂݍ���(�������݂͂Ȃ�)
    IKpiFolder  : �t�H���_�ɑ���
    IKpiTagInfo : �^�O���擾�p
    IKpiConfig  : �ݒ�ǂݏ����p
    IKpiUnkProvider : IKpiConfig �擾�A�v���O�C������v���O�C�����g���ꍇ���Ɏg�p
    IKpiXXXEnumerator: �񋓊֌W(IKpiTagInfo ������Ӗ��񋓌n)

    �����̃C���^�[�t�F�[�X�� KbMedia Player ���Œ񋟂���̂Ńv���O�C�������
    ����������K�v�͂Ȃ��B
    �������A�A�[�J�C�u�v���O�C�������ꍇ�ɂ� IKpiFile �̎������K�v�B

    �v���O�C�����Ŏ���
    IKpiDecoderModule : �f�R�[�_�v���O�C��(�I�[�f�B�I�f�[�^�� PCM �Ƀf�R�[�h)
    IKpiDecoder       : �������ۂɏ�������
    IKpiUnArcModule   : �A�[�J�C�u�W�J�v���O�C��
    IKpiUnArc         : �������ۂɏ�������
    IKpiPlayerModule  : ���t�v���O�C��(��ɓ��擙)(PCM ���擾�o���Ȃ��������������)
    IKpiPlayer        ; �������ۂɏ�������
    IKpiOutDeviceModule :�f�o�C�X�o�̓v���O�C��
    IkpiOutDevice       :�������ۂɏ�������

    �����̃C���^�[�t�F�[�X�̓v���O�C�����Ŏ������K�v�B
    QueryInterface/AddRef/Release �̎������ʓ|�Ȃ� kpi_impl.h ��
    ��`����Ă���N���X����h�������Ă��ǂ�(�K�{�ł͂Ȃ�)

    �\����(struct KPI_XXX)

    KPI_DECODER_MODULEINFO :�f�R�[�_�v���O�C���̏��(���d�l�� KMPMODULE �̏�񕔕��ɑ���)
    KPI_UNARC_MODULEINFO   :�A�[�J�C�u�v���O�C���̏��(���̃A�[�J�C�u��)
    KPI_PLAYER_MODULEINFO  :���t�v���O�C���̏��(���ۂ� KPI_DECODER_MODULEINFO �� typedef)
    KPI_OUTDEVICE_MODULEINFO: �f�o�C�X�o�̓v���O�C���̏��
    KPI_MEDIAINFO          :�f�R�[�h����Ȃ̏��(���d�l�� SOUNDINFO �ɑ���)
    KPI_ARCHIVEINFO        :�A�[�J�C�u�t�@�C�����̂̏��
    KPI_FILEINFO           :�A�[�J�C�u�t�@�C�����̊i�[�t�@�C���̏��

    �قƂ�ǂ̍\���̂� sizeof �l���i�[���郁���o(cb) �����B
    �\���̂̊g���̗]�n���c���B

    IKpiXXX �� Release ���Y��ɏ\�����ӂ��邱��
    ���������[�N���邾���łȂ��AKbMedia Player ���������A�N�Z�X�ᔽ�ňُ�I��
    ���邱�Ƃ�����B

    AddRef/Release �͐������ɒ��ӂ��K�v�B�������Ă���Ǝv�����O�̂��ߏ�����
    �������Ƃɂ���Ɓc�B

    �EIKpiDecoderModule::Open �� IKpiFile/IKpiFolder ���A���\�b�h�̈����Ƃ���
      �n���ꂽ�C���^�[�t�F�[�X�́A�n���ꂽ���_�ł̃v���O�C�����Ǘ����ׂ��Q��
      �J�E���g�� 0 �Ƃ��Đ�����B
      �EAddRef �� Release �̉񐔂𓯂��ɂ���Ƃ������ƁB
      �E���\�b�h����߂�������g�p����ꍇ�� AddRef ���Ȃ���΂Ȃ�Ȃ��B
        (�s�v�ɂȂ����� AddRef �����̂Ɠ����񐔂��� Release ���Ȃ���΂Ȃ�Ȃ�)
      �E���\�b�h����߂�����Ɏg�p���Ȃ��ꍇ�� Release �s�v�BRelease ���Ă͂����Ȃ�
      �E�ꕔ�̃C���^�[�t�F�[�X�̓��\�b�h����߂�����̎g�p��F�߂Ă��Ȃ�(��ɗ񋓌n)
        (IKpiDecoder::Select ���� IKpiTagInfo �� IKpiXXXEnumerator �����Y��)
    �EQueryInterface �� IKpiFile::CreateClone, IKpiFolder::OpenFile ���Ŋl������
      �C���X�^���X�́A�l�����_�ł̃v���O�C�����Ǘ����ׂ��Q�ƃJ�E���g�� 1 �Ƃ���
      ������B
      �EAddRef ��� Release �̉񐔂̕��� 1 �񂾂������Ƃ������ƁB

    �Ȃ��A�{�̂���񋟂����C���^�[�t�F�[�X�Ɍ���A

    �E�C���X�^���X�̍쐬�����A�l�������C���X�^���X����� Release ���Ă��ǂ�

    �Ⴆ�� IKpiFolder::OpenFile �� IKpiFile ���l����A�l������ IKpiFile ���
    ��� IKpiFolder �� Release ���ėǂ��Ƃ������Ƃł���B
    IKpiUnkProvider �ɂ�� IKpiConfig �l�������l�B

    ����̓v���O�C���쐬�����猩���Ƃ������ŁA�v���O�C���g�p���ɂ͓��Ă͂܂�Ȃ��B
    �Ⴆ�Ζ{�̂� IKpiDecoder ������� IKpiDecoderModule �� Release ���邱�Ƃ�
    �Ȃ��A�����Ȃ�\����z�肵�ăv���O�C�����쐬����K�v�͂Ȃ��B

    ��O�͈ȉ��ł���B

    �EIKpiFile::GetRealFileW/A �œ������t�@�C�������\�b�h����߂�������g�p����
      �ꍇ�� AddRef ���Ă����A���t�@�C���������� Release ���Ȃ���΂Ȃ�Ȃ��B
    �EIKpiFile::GetBuffer �œ����������o�b�t�@�����\�b�h����߂�������g�p����
      �ꍇ�� AddRef ���Ă����A�������o�b�t�@���s�v�ɂȂ����� Release ���Ȃ���΂Ȃ�Ȃ��B

    GetRealFileW/A/GetBuffer �������ʂ������ォ�� Release �����i���Ȃ����߁B

    Release ���Y���{�̑��Ō��o���邱�Ƃ͏o�����A�{�̂̓��삪�s����ɂȂ邱��
    �����邽�߁AAddRef/Release �̊Ǘ��͏\���ɋC���������ƁB


    extern DWORD WINAPI kpi_CreateConfig(IKpiUnknown *pUnknown,
                                         const GUID *pGUID,
                                         DWORD *pdwPlatform,
                                         IKpiConfig **ppConfig);
    ���Aextern kpi_XXX

    �Ő錾�����w���p�[�֐����g�p����ꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ�����
    (�����J�G���[���)

    IKpiXXXModule �͈ȉ��̋��ʂ̃��\�b�h������
     �EGetModuleInfo: �v���O�C���̏����擾(�����̍\����(KPI_XXX_MODULEINFO)�̌^�̓v���O�C���̎�ނɂ��قȂ�)
     �EOpen :         IKpiXXX �̃C���X�^���X���擾����(�����̓v���O�C���̎�ނɂ��قȂ�)
     �EEnumConfig:    �ݒ荀�ڂ�񋓂���
     �EApplyConfig:   �ݒ���u�K�p���悤�Ƃ������v�ɌĂ΂��B�Ă΂ꂽ���_�ł�
                      �܂��ݒ�͕ύX����Ă��Ȃ��B

    IKpiXXX �͈ȉ��̋��ʂ̃��\�b�h������
     �EUpdateConfig: IKpiXXXModule::ApplyConfig ���Ăяo���Đݒ���u�K�p������Ɂv�Ă΂��B
                     IKpiXXX �̃C���X�^���X���ƂɌĂ΂��B
*/

typedef IUnknown IKpiUnknown; //IKpiUnknown = IUnknown
class __declspec(uuid("{C3BCE8C9-3F3E-49D0-8278-6014D9667274}")) IKpiFile;
class __declspec(uuid("{E521AD43-9156-4923-880A-7B8A3D1CF97B}")) IKpiFolder;
class __declspec(uuid("{130903A6-D381-4136-832E-688689AEF030}")) IKpiTagInfo;
class __declspec(uuid("{EFE8AC3E-A5D4-403A-BBB3-BD09A1B292F4}")) IKpiConfigEnumerator;
class __declspec(uuid("{D01D08E8-9823-48D9-85E9-4F37DCC09EEF}")) IKpiConfig;
class __declspec(uuid("{C5CF866A-4A9B-46E1-9CA8-3025D20774E9}")) IKpiUnkProvider;

#define IID_IKpiUnknown     __uuidof(IKpiUnknown)
#define IID_IKpiFile        __uuidof(IKpiFile)
#define IID_IKpiFolder      __uuidof(IKpiFolder)
#define IID_IKpiTagInfo     __uuidof(IKpiTagInfo)
#define IID_IKpiConfigEnumerator __uuidof(IKpiConfigEnumerator)
#define IID_IKpiConfig      __uuidof(IKpiConfig)
#define IID_IKpiUnkProvider __uuidof(IKpiUnkProvider)

#define KPI_MULTINST_INFINITE 0//IKpiXXXModule::Open �ŃC���X�^���X�𓯎��ɉ��ł��쐬�\
#define KPI_MULTINST_ZERO     1//���͏�ɕʃv���Z�X�o�R�Ŏ擾
#define KPI_MULTINST_ONE      2//���͓�����1�܂ō쐬�\(2�ڈȍ~�͕ʃv���Z�X�o�R)
//(KPI_MULTIINST_ZERO + N )    //���͓�����N�܂ō쐬�\((N+1)�ڈȍ~�͕ʃv���Z�X�o�R)
#define KPI_MULTINST_UNIQUE (DWORD)-1 //���̓V�X�e����(�v���Z�X���܂����ł�)1�����쐬�s��

//IKpiFile/IKpiFolder ���̕�����擾�E�񋓌n���\�b�h
//
//  GetXXXName(..., wchar_t *pszName, DWORD dwSize, ...)
//  EnumXXX(DWORD dwIndex, wchar_t *pszName, DWORD dwSize, ...)
//
//�̎d�l
//
//pszName �Ɋi�[���ׂ��o�b�t�@�AdwSize �Ƀo�b�t�@�̃T�C�Y(�������ł͂Ȃ�)��n���B
//�i�[���ׂ������񂪑��݂���(������)�̏ꍇ�͕K�v�ȃo�b�t�@�̃T�C�Y(�I�[�� 0 ���܂�)��Ԃ��B
//�o�b�t�@�̃T�C�Y������Ă���ꍇ�� pszName �ɕ�������i�[����B
//(�߂�l�͕K�v�ȃo�b�t�@�̃T�C�Y==�i�[�����o�b�t�@�̃T�C�Y)
//�o�b�t�@�̃T�C�Y������Ȃ��ꍇ�� pszName �ɋ󕶎�����i�[����B
//�i�[���ׂ������񂪂Ȃ��ꍇ�� pszName �ɋ󕶎�����i�[���� 0 ��Ԃ��B
//
//�񋓌n(EnumXXX)�ł� 0 ��Ԃ����Ƃ��ɗ񋓏I���B���� dwIndex �ɑ΂��� 0 ��Ԃ����ꍇ�A
//���̒l�ȏ�� dwIndex �ɑ΂��Ă͕K�� 0 ��Ԃ��B
//
//pszName �� NULL �� dwSize �� 2 ��菬���Ȓl��n���Ă͂����Ȃ��BGet/Enum �n�� 0 ��Ԃ��ꍇ��
//�o�b�t�@�̃T�C�Y������Ȃ��ꍇ�ApszName �ɋ󕶎��񂪓���B�r�؂ꂽ������ł͂Ȃ��A�󕶎���
//�����邱�Ƃɒ��ӁB�n�����o�b�t�@�̃T�C�Y�����傫�Ȓl��Ԃ�����擾�Ɏ��s���Ă���B
//
///////////////////////////////////////////////////////////////////////////////
#define KPI_FILE_EOF (UINT64)-1 //�T�C�Y���s������ GetSize() �̖߂�l
                                //���s���� Seek() �̖߂�l
class IKpiFile : public IKpiUnknown
{
/*
    C�����^�C�����C�u������ FILE �\���́AWin32API �� HANDLE hFile �ɑ�������B
    �ʏ�� Read/Seek/GetSize �����g��Ȃ��ėǂ��͂��B
    GetRealFile �� GetBuffer ���g�����ꍇ�́A�擾�����t�@�C���⃁�����o�b�t�@��
    ����(IKpiFile �� Release ����܂�)�ɏ\���C���������ƁB
*/
public:
    enum{
        GET_BUFFER_MAXSIZE = 128*1024*1024 //GetBuffer ���Ԃ��ő�T�C�Y(128MB)
        //�{�̂� GetBuffer ���G�~�����[�g���鎞�̂��̂ŁA���ۂ̃T�C�Y�̐�����
        //�A�[�J�C�u�v���O�C����{�̂̎����ɂ��
        //�����`����������傫�����Ƃ����蓾��ꍇ�AGetBuffer ���ĂԂ��Ƃ�
        //�������Ȃ�
    };
    virtual DWORD  WINAPI Read(void *pBuffer, DWORD dwSize)=0;
    virtual UINT64 WINAPI Seek(INT64 i64Pos, DWORD dwOrigin)=0;
    virtual UINT64 WINAPI GetSize(void)=0;
    virtual BOOL   WINAPI CreateClone(IKpiFile **ppFile)=0;
    virtual DWORD  WINAPI GetFileName(wchar_t *pszName, DWORD dwSize)=0;//dwSize�̒P�ʂ̓o�b�t�@�T�C�Y(�������ł͂Ȃ�)
    virtual BOOL   WINAPI GetRealFileW(const wchar_t **ppszFileNameW)=0;
    virtual BOOL   WINAPI GetRealFileA(const char **ppszFileNameA)=0;
    virtual BOOL   WINAPI GetBuffer(const BYTE **ppBuffer, size_t *pstSize)=0;//32bit �ł� 64bit �łŊm�ۉ\�ȃo�b�t�@�T�C�Y�̏�����قȂ�
    virtual BOOL   WINAPI Abort(void)=0;
/*
    DWORD WINAPI Read(void *pBuffer, DWORD dwSize);
      C�����^�C�����C�u������ �� fread�AWin32API �� ReadFile �ɑ�������B
      �ǂݎ�����f�[�^�̃T�C�Y��Ԃ��BdwSize ��菬���Ȓl��Ԃ�����I�[��
      ���B�������Ƃ��Ӗ�����B�Ăяo�����ŏ�ɖ߂�l���m�F���邱�ƁB
      dwSize ��菬�����l��Ԃ��ꍇ�A�c��̃o�b�t�@���ǂ��Ȃ邩�͖���`�B
      �ʏ�A�[���N���A�Ȃǂ͂���Ȃ��B

    UINT64 WINAPI Seek(INT64 i64Pos, DWORD dwOrigin);
      C�����^�C�����C�u������ fseek�AWin32API �� SetFilePointer �ɑ�������B
      ���������ꍇ�̓V�[�N��̈ʒu��Ԃ��B���s�����ꍇ�� KPI_FILE_EOF ��Ԃ��B

      �v���O�C���ɑ΂��Ė{�̂���n����� IKpiFile::Seek �̌��ʂ́A
      �E�v�����ꂽ�̂Ɛ��m�ɓ����ʒu��Ԃ����AKPI_FILE_EOF ��Ԃ��B
      �EGetSize()==KPI_FILE_EOF �̏ꍇ�� dwOrigin==FILE_END �� Seek �����ꍇ��
        KPI_FILE_EOF ��Ԃ��B
      �EdwOrigin!=FILE_BEGIN && dwOrigin!=FILE_CURRENT && dwOrigin!=FILE_END �̏ꍇ
        �� Seek �̌��ʂ͖���`�B(FILE_BEGIN/CURRENT/END �ȊO��n���Ă͂����Ȃ�)
      �ESeek �� KPI_FILE_EOF ��Ԃ������ Read �� 0 ��Ԃ��B
        Seek �� KPI_FILE_EOF ��Ԃ�����A
        �EdwOrigin==FILE_CURRENT �� Seek �����ꍇ�� KPI_FILE_EOF ��Ԃ��B
        �EdwOrigin==FILE_BEGIN or FILE_END �� Seek �����ꍇ�ɐ�������΁A
          ����ȍ~�� Read �͐�������B
      �EKbMedia Player �̃v���O�C�����g���A�v���P�[�V���������ꍇ�A�v���O�C���ɓn��
        IKpiFile::Seek �͏�L�𖞂����悤�Ɏ������Ȃ���΂Ȃ�Ȃ�
      �EKbMedia Player �̃v���O�C�����쐬����ꍇ�́A���݈ʒu��c������ړI�ȊO��
        Seek �̖߂�l���m�F����K�v�͂��܂�Ȃ����ARead �̖߂�l�͏�Ɋm�F����ׂ��B

      �v���O�C��(��ɃA�[�J�C�u�v���O�C��)���{�̂ɒ񋟂��� IKpiFile::Seek �̌��ʂ́A
      �E�v�����ꂽ�̂Ɛ��m�ɓ����ʒu��Ԃ����A�v�����ꂽ����O�̈ʒu�A���邢��
        KPI_FILE_EOF ��Ԃ��B�v�����ꂽ�����̈ʒu�ɃV�[�N���Ă͂����Ȃ��B
      �EGetSize()==KPI_FILE_EOF �̏ꍇ�� dwOrigin==FILE_END �� Seek �����ꍇ��
        KPI_FILE_EOF ��Ԃ��B
      �EdwOrigin!=FILE_BEGIN && dwOrigin!=FILE_CURRENT && dwOrigin!=FILE_END �̏ꍇ
        �� Seek �̌��ʂ͖���`
      �ESeek �� KPI_FILE_EOF ��Ԃ������ Read �̌��ʂ͖���`�B
      �ESeek �� KPI_FILE_EOF ��Ԃ�����A
        �EdwOrigin==FILE_CURRENT �� Seek �������ʂ͖���`�B
        �EdwOrigin==FILE_BEGIN or FILE_END �� Seek �����ꍇ�ɐ�������΁A
          ����ȍ~�� Read �͐�������B
      �E�v����ʒu�ƈقȂ�ʒu��Ԃ����Ƃ�����A���s������� Read �̌��ʂ�����`�̂��߁A
        �A�[�J�C�u�v���O�C�����g�p����A�v���P�[�V���������ꍇ�A��� Seek �̖߂�l��
        �m�F���Ȃ���΂Ȃ�Ȃ��B(�v���O�C�����쎞�͂����܂Ő_�o���ɂȂ�K�v�͂Ȃ�)
      �E�A�[�J�C�u�v���O�C�����쎞�́A���ʂ�����`�ɂȂ�悤�� Seek/Read �̌Ă΂����
        ����Ȃ����Ƃ�z�肵�Ď������ėǂ��B

    UINT64 WINAPI GetSize(void);
      �t�@�C���T�C�Y��Ԃ��B���s��Ƀt�@�C���|�C���^�͕ω����Ȃ��B
      C�����^�C�����C�u�����ł� fseek(0,SEEK_END) ��� ftell �ɑ����B
      (�t�@�C���|�C���^�͈ړ����Ȃ���)
      Win32API �ł� GetFileSize �ɑ����B
      �T�C�Y���킩��Ȃ��ꍇ�� KPI_FILE_EOF ��Ԃ��B

    BOOL WINAPI CreateClone(IKpiFile **ppFile);
      �g�p���� IKpiFile �Ƃ͓Ɨ����ē��삷�� IKpiFile �������K�v�ȏꍇ�ɌĂяo���B
      ���������� 0 �ȊO��Ԃ��� *ppFile �Ɍ��� IKpiFile �̃R�s�[�ɑ�������C���X�^���X��Ԃ��B
      ���s������ 0 ��Ԃ��� *ppFile �� NULL ��Ԃ��B
      �A�[�J�C�u�v���O�C���̊J���ɂ����āA����t�@�C�����𓀒��ɕʂ̃t�@�C����
      �𓀂��邽�߂ɕK�v�ɂȂ�B
      �擾��� *ppFile �̃t�@�C���|�C���^�͐擪�ɐݒ肳���B
      (�R�s�[���̃t�@�C���|�C���^�Ɠ����ł͂Ȃ����Ƃɒ���)

    DWORD WINAPI GetFileName(wchar_t *pszName, DWORD dwSize);
      IKpiFile �̃t�@�C�����𓾂�(�t���p�X�ł͂Ȃ�)
      dwSize >= MAX_PATH*sizeof(wchar_t) �Ƃ���΁A�o�b�t�@�T�C�Y������Ȃ����Ƃ͂قڂȂ�
      �Đ�����t�@�C���̊g���q��m�肽���ꍇ��A�Đ�����t�@�C���Ɠ����̊O��
      �t�@�C�����K�v�ȏꍇ�ȂǂɎg���B(aaa.xxx ���Đ�����̂� aaa.yyy ���K�v�Ȃ�)
      �Ⴆ�� aaa.xxx �ɑ΂��� GetFileName() ���ĂԂ� "aaa.xxx" ��Ԃ��B
      ���O���Ȃ��ꍇ�� pszName �ɋ󕶎����Ԃ��B

      �t���p�X�𓾂��i�͒񋟂���Ȃ��B�t�@�C���̏ꏊ���ӎ�����ׂ��ł͂Ȃ��B

    BOOL WINAPI GetRealFileW(const wchar_t **ppszFileNameW);
      IKpiFile �̓��e�����t�@�C���Ƃ��ē���B
      ���������� 0 �ȊO��Ԃ��� *ppszFileNameW �Ɏ��̂̃t�@�C����(�t���p�X)������B
      ���s������ 0 ��Ԃ��� *ppszFileNameW �� NULL ������B
      *ppszFileNameW �y�ю��t�@�C���� IKpiFile �� Release ����܂ŗL���B
      �]���āA
       �EIKpiFile �� Release ����O�Ɏ��t�@�C������Ȃ���΂Ȃ�Ȃ��B
       �E���t�@�C�����g���O�� IKpiFile �� Release ���Ă͂����Ȃ��B
       �E���X���̂����[�J���t�@�C���������ꍇ�AIKpiFile �� Release ���Ă���
         �g�p���Ă����Ȃ����삵�Ă��܂����A����̓o�O�ł���B
      ���t�@�C���̓��e��������������A�t�@�C�����ړ��E�폜���Ă͂����Ȃ��B
      �A�[�J�C�u���������Ă���ꍇ�AKbMedia Player �� IKpiFile ���쐬����
      ���_�ł́A���t�@�C�����쐬���Ă��Ȃ����Ƃ�����B���̏ꍇ�� GetRealFile
      ���Ăяo���ƁA�e���|�����t�H���_�Ɉꎞ�t�@�C�����쐬���邱�ƂɂȂ�A
      �p�t�H�[�}���X�̖ʂŕs���ɂȂ�B

      �����ŕԂ����t�@�C������ GetFileName �ŕԂ��t�@�C�����͓����Ƃ͌���Ȃ�
      ���t�@�C������ C:\temp\aaa.txt �Ȃ̂� GetFileName �� bbb.txt ��Ԃ�����
      �����邱�Ƃɒ���

      ���C�u�����̓s���łǂ����Ă����t�@�C�������K�v�ȏꍇ�������A
      �Ȃ�ׂ� GetRealFileW/A �͎g�킸�� Read/Seek/GetBuffer �Ŏ������邱��

      �t�@�C�����𓾂邱�Ƃ������ړI�̏ꍇ�� GetFileName ���Ăяo�����ƁB

    BOOL WINAPI GetRealFileA(const char **ppszFileNameA);
      GetRealFileW �� ANSI ��
      �g�p���郉�C�u�����̓s����AUNICODE �t�@�C�����ɖ��Ή��ȂƂ��� GetRealFileW
      �̑���ɌĂяo���B
      ���t�@�C������ ANSI �ŕ\���o���Ȃ��ꍇ�́A���t�@�C���̃R�s�[���e���|������
      �쐬���ăR�s�[�̃t�@�C������Ԃ��B

      �����ŕԂ����t�@�C������ GetFileName �ŕԂ��t�@�C�����͓����Ƃ͌���Ȃ�
      ���t�@�C������ C:\temp\aaa.txt �Ȃ̂� GetFileName �� bbb.txt ��Ԃ�����
      �����邱�Ƃɒ��ӁB

      UNICODE �t�@�C�����̃R�s�[�� ANSI �t�@�C�����̓����_���ɐ��������
      �������A�t�H���_�������� UNICODE ���̏ꍇ�A�t�@�C���������̓I���W�i��
      �Ɠ������O�ɂȂ�B

    BOOL WINAPI GetBuffer(const BYTE **ppBuffer, size_t *pstSize);
      IKpiFile �̓��e���������o�b�t�@�Ƃ��ē���B
      ���������� 0 �ȊO��Ԃ��� *ppBuffer �Ƀ������o�b�t�@���A*pstSize �Ƀ�����
      �o�b�t�@�̃T�C�Y������B
      ���s������ 0 ��Ԃ��� *ppBuffer �ɂ� NULL ���A*pstSize �ɂ� 0 ������B
      *ppBuffer �� IKpiFile �� Release ����܂ŗL���B
      IKpiFile �� Release ��� *ppBuffer �ɃA�N�Z�X���Ă͂����Ȃ��B
      �������o�b�t�@�̓��e�����������Ă͂����Ȃ��B

      IKpiFile �̎��̂��������o�b�t�@�łȂ��ꍇ(�A�[�J�C�u�v���O�C�����Ԃ�
      IKpiFile::GetBuffer �Ɏ��s�����ꍇ)�� kbrunkpi ����Ă���ꍇ�A
      KbMedia Player �� GetBuffer ���G�~�����[�g����B
      ���̂Ƃ��A���̂̃T�C�Y(IKpiFile::GetSize ���Ԃ��l)���傫���ꍇ�͎��s����
      ���Ƃ�����B(�ő�� GET_BUFFER_MAXSIZE �܂�)
      ����̓T�C�Y�� GET_BUFFER_MAXSIZE ���傫���ƕK�� GetBuffer �Ɏ��s����
      ���Ƃ��Ӗ������ł͂Ȃ��B�v���O�C���̎����ɂ���Ă͂����Ƒ傫�ȃT�C�Y
      �ł��������邱�Ƃ�����B(64bit �łł� 4GB �𒴂��Ă��������邱�Ƃ����蓾��)
      GET_BUFFER_MAXSIZE �̎��ۂ̒l�͖{�̂̃v���b�g�t�H�[����o�[�W�����ɂ��
      �ύX����邱�Ƃ�����(���� 64bit ��)

      �����t�@�C���`�����ʏ� GET_BUFFER_MAXSIZE ��菬�����ARead/Seek ���g�����
      GetBuffer ���g���������������e�Ղł�������p�t�H�[�}���X�ʂŗL���Ȃ�΁A
      GetBuffer ��ϋɓI�ɗ��p����B������ GetBuffer �̓T�C�Y���傫������Ǝ��s
      ���邱�Ƃ����邱�Ƃɗ��ӂ��邱�ƁB

      ���� GetBufferSize �Ɏ��s�����ꍇ�� Read �� GetSize ���g���ăv���O�C����
      �����Ń����������蓖�Ă邩�A�v���O�C���������`���Ƃ��Ă̓T�C�Y���傫������
      �Ɣ��f���ēK�؂ɏ������邱�ƁB

      GetSize/CreateClone/GetFileName/GetRealFile/GetBuffer �̂ǂ̃��\�b�h���Ăяo���Ă�
      �t�@�C���|�C���^�͕ω������A���̌�� Read/Seek �̌��ʂɉe�����Ȃ��B

    BOOL WINAPI Abort(void);
      �����𒆒f����B���������� TRUE ��Ԃ��B
      ���̃��\�b�h���ďo����A�S�Ẵ��\�b�h�Ăяo���͎��s����B
      Abort() �͔C�ӂ̃X���b�h����񓯊��ɌĂяo�����B
      Abort() �͒ʏ�A�v���O�C�����g�p����A�v���P�[�V����(KbMedia Player)��
      ����ČĂяo�����B
      �v���O�C�����ł��̃��\�b�h���Ăяo���Ă͂����Ȃ��B
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiFolder : public IKpiUnknown
{
/*
    IKpiDecoderModule/IKpiUnArcModule::Open ���� IKpiFile �Ƌ��ɓn�����B
    IKpiFile �����݂���t�H���_�ɑ�������B
    IKpiFile ���瓾��f�[�^�����ł͏����o�����A���̓���t�H���_���̕ʃf�[�^��
    �K�v�ȏꍇ�Ɏg���B
    ���y�n�ł͍Đ��� PCM �t�@�C����K�v�Ƃ���`���ȂǂŕK�v�B
    �A�[�J�C�u�n�ł̓}���`�{�����[���╪���t�@�C���ȂǂɑΉ����邽�߂ɕK�v�B

    IKpiFolder �ɂ�����ƑΉ�����͍̂��������Ȃ��B

    �v���O�C���J���ɂ���𗘗p���邱�Ƃ͏��Ȃ��͂������A���p���Ȃ����
    �Ȃ�Ȃ��ꍇ�͊撣�邱�ƁB

    �ǂ����Ă���������ȏꍇ�́AIKpiFile::GetRealFileW/A �� IKpiFile �̎��t�@�C����
    �擾���āA���t�@�C���Ɠ����t�H���_�ɒ��ڃA�N�Z�X����B
    ��������t�H���_���̊֘A�t�@�C�����K�v�ȏꍇ�́A�K�v�ȃt�@�C���� OpenFile ����΁A
    �Đ�������t�@�C���Ɠ����t�H���_�Ɋ֘A�t�@�C���̎��t�@�C�����쐬�����B
    OpenFile ���Ȃ������ꍇ�A���t�@�C���Ɠ����t�H���_�Ɋ֘A�t�@�C�������݂��邩�ǂ�����
    ������Ȃ��B(�Ⴆ�΃A�[�J�C�u�ł͑��݂��Ȃ��\��������)
    IKpiFile/IKpiFolder �̎��̂̌`���ɂ���Ă͂��܂����삵�Ȃ��\��������B
    �Ⴆ�� http ���(�A�[�J�C�u�łȂ�)�t�@�C���ł� IKpiFolder �����삵�Ȃ��B

    �v���O�C�����p�ґ��ɂƂ��� IKpiFolder �̎����͂��Ȃ荢��ł���Ǝv����B
    ��(Kobarin)�������撣��Ηǂ����Ƃ����B

    �Ⴆ�� SPC ���Đ�����Ƃ��ɂ́A
       �E����t�H���_���̊g���q .7se �̓����t�@�C��
       �E����t�H���_���̊g���q .700 �̓����t�@�C��
       �E����t�H���_���� 65816.7se
       �E����t�H���_���� 65816.700
    �����݂���΁A�����ǂݍ���(Script700/7SE)

    �܂��AMDX ���Đ�����Ƃ��ɂ́A
       �E����t�H���_���� PDX �t�@�C��(�t�@�C������ MDX �t�@�C���Ɋi�[����Ă���)
    ���K�v�ɂȂ�B

    �������A�[�J�C�u�ɂ��Ή������邽�߂̋@�\�B

    �A�[�J�C�u�v���O�C�����}���`�{�����[�����ɂɑΉ��������ꍇ�ɂ��K�v�ɂȂ�
*/
public:
    virtual DWORD WINAPI GetFolderName(wchar_t *pszName, DWORD dwSize)=0;//dwSize �̒P�ʂ̓o�b�t�@�T�C�Y(�������ł͂Ȃ�)
    virtual DWORD WINAPI EnumFiles(DWORD dwIndex, wchar_t *pszName, DWORD dwSize, DWORD dwLevel)=0;//dwSize �̒P�ʂ̓o�b�t�@�T�C�Y(�������ł͂Ȃ�)
    virtual BOOL  WINAPI OpenFile(const wchar_t* cszName, IKpiFile **ppFile)=0;
    virtual BOOL  WINAPI OpenFolder(const wchar_t* cszName, IKpiFolder **ppFolder)=0;
/*
    DWORD WINAPI GetFolderName(wchar_t *pszName, DWORD dwSize);
      IKpiFolder �̃t�H���_���𓾂�(�t���p�X�ł͂Ȃ�)
      �t�H���_���̖����� \ �͂��Ȃ��B
      dwSize >= MAX_PATH*sizeof(wchar_t) �Ƃ���΁A�o�b�t�@�T�C�Y������Ȃ����Ƃ͂قڂȂ�
      IKpiFolder �̎��̂��A�[�J�C�u�̏ꍇ�A�A�[�J�C�u�t�@�C��������g���q��������
      ���̂�Ԃ��B(IKpiFolder �� aaa.lzh> �̏ꍇ�� "aaa" ���t�H���_���ƂȂ�)
      IKpiFolder ���h���C�u�̃��[�g�̏ꍇ�A: �� $ �ɒu���������̂�Ԃ��B
      (IKpiFolder �� C:\ �̏ꍇ�� "C$" ���t�H���_���ƂȂ�)
      ��������t�@�C�����u����Ă���t�H���_�̖��O���K�v�ȏꍇ�Ɏg���B
      (�t�H���_�������ɎQ�ƃt�@�C���������܂�Ȃ�)
      ���O���Ȃ��ꍇ�ApszName �ɂ͋󕶎��񂪓���B

    DWORD WINAPI EnumFiles(DWORD dwIndex, wchar_t *pszName, DWORD dwSize, DWORD dwLevel);
      IKpiFolder �Ɠ����t�H���_�ɂ���t�@�C���ƃt�H���_��񋓂���B
      dwSize >= MAX_PATH*sizeof(wchar_t) �Ƃ���΁A�o�b�t�@�T�C�Y������Ȃ����Ƃ͂قڂȂ�
      �񋓂���閼�O�͑S�� IKpiFolder ������t�H���_����̑��΃p�X�ƂȂ�B
      �T�u�t�H���_�ȉ����񋓂���B
      �p�X�̋�؂蕶���� / ���g����B(\ �ł͂Ȃ�)
      dwLevel �͌�������T�u�t�H���_�̐[���B
        �EdwLevel==0 �̏ꍇ�̓T�u�t�H���_�ȉ���񋓂��Ȃ��B(�T�u�t�H���_���͗񋓂���)
        �EdwLevel==(DWORD)-1 �̏ꍇ�͊K�w�����Ȃ��ɗ񋓁B
      ���������� dwIndex �ɑΉ�����t�@�C�����܂��̓t�H���_���� pszName �ɕԂ���
      ���O���i�[����̂ɕK�v�ȃo�b�t�@�̃T�C�Y��Ԃ��B(0 �ȊO��Ԃ�)
      ���O�̖����� / �łȂ��ꍇ�̓t�@�C�����A������ / �̏ꍇ�̓t�H���_���ƂȂ�B
      ���s������ 0 ��Ԃ��ė񋓏I���B(����ȏ�� dwIndex ��n���Ă����s)

      pszName �� dwIndex �� dwLevel �̑g�����Ō��܂�BdwIndex �� dwLevel �̑g����
      ���ς��΁AdwIndex �������ł� pszName ���ω�����\�������邱�Ƃɒ��ӁB
      �Ⴆ�΁A
        dwIndex=0, dwLevel=0 �̂Ƃ��� pszName="aaa.txt" �ƂȂ�������Ƃ����āA
        dwIndex=0, dwLevel=1 �̂Ƃ��� pszName="aaa.txt" �ƂȂ�Ƃ͌���Ȃ��B

      �񋓂���鏇�Ԃ͌����������ł����Ȃ��B�t�@�C���E�t�H���_�̂ǂ��炪���
      �񋓂���邩�͂킩��Ȃ����A�ʒu�֌W���΂�΂�̉\�������邱�Ƃɒ��ӁB
      �Ⴆ�΁AdwLevel=2 �� EnumFiles ���Ăяo�����ꍇ�A
        dwIndex=0: xxx/yyy/aaa.txt
        dwIndex=1: xxx/bbb.txt
        dwIndex=2: xxx/yyy/ccc.txt
        �̂悤�ȏ��Ԃŗ񋓂���邱�Ƃ�����BdwIndex=1 �̎��_�� xxx/yyy/ �t�H���_
        �ɂ͂����t�@�C�������݂��Ȃ��ƌ��ߕt���Ă͂����Ȃ��B

      EnumFiles �́AIKpiFile �̓��e�� IKpiFile::GetFileName�AIKpiFolder::GetFolderName
      ����K�v�ȎQ�ƃt�@�C���������肷�邱�Ƃ��o���Ȃ��ꍇ�������A�Ȃ�ׂ��Ăяo���Ȃ�
      �悤�ɂ��邱�ƁB�Ăяo���Ȃ�A�o���邾�� dwLevel < 2 �ɂ��邱�ƁB

      �񋓂���t�@�C���̒��ɃA�[�J�C�u�t�@�C�����������ꍇ�A�t�@�C���Ɠ�����
      �t�H���_�Ƃ��Ă��񋓂���A�t�H���_�Ƃ��Ă̖��O�̖����� > �ƂȂ�B
      �Ⴆ�� aaa.lzh ���܂܂�Ă���ꍇ�Aaaa.lzh �� aaa.lzh> ���񋓂����B
      aaa.lzh> �̉��� dwLevel �̒l�Ɋւ��Ȃ��񋓂���Ȃ��B
      aaa.lzh> �ȉ��̗񋓂��K�v�ȏꍇ�� OpenFolder("aaa.lzh>", &pArchiveFolder) ��
      �悤�ɂ��Ă��� pArchiveFolder �ɑ΂��� EnumFiles ���Ăяo���B

    BOOL WINAPI OpenFile(const wchar_t* cszName, IKpiFile **ppFile);
      EnumFiles �ŕԂ����O�ɑΉ�����t�@�C�����擾����B
      �擾���� *ppFile �́A�K�v���Ȃ��Ȃ����� Release ���邱�ƁB

      cszName �ɂ͑��΃p�X�A��΃p�X���g�p�\�B
      EnumFiles �ł��炩���ߗ񋓂��Ă����K�v�͂Ȃ��B
      �p�X�̋�؂蕶���� \ �� / �̂ǂ�����g�p���Ă��ǂ��B
      �������A�A�[�J�C�u�̒����w�肷��ꍇ�� > ���g�p����B
      ��1 �FOpenFile("a.txt", &pFile);        //���΃p�X
      ��2 �FOpenFile("a\\b\\c.txt", &pFile);  // \ �� / �̂ǂ���ł��ǂ�
      ��2'�FOpenFile("a/b/c.txt", &pFile);    // \ �� / �̂ǂ���ł��ǂ�
      ��3 �FOpenFile("..\\a.txt", &pFile);    //���΃p�X(�e�t�H���_)
      ��4 �FOpenFile("C:\\a\\b.txt", &pFile); //��΃p�X
      ��5 �FOpenFile("a.lzh>b/c.txt", &pFile);//�A�[�J�C�u(���΃p�X)        #���ݖ�����
      ��6 �FOpenFile("C:\\a.lzh>b.zip>c.txt", &pFile);//�A�[�J�C�u(��΃p�X)#���ݖ�����
      ��7 �FOpenFile("http://www.hoge.com/a.zip>b.txt", &pFile);//http://     #���ݖ�����

      ��5,6 �̓p�t�H�[�}���X�̊֌W���������錩���݂͏��Ȃ��B
      �ǂ����Ă��A�[�J�C�u�̒��̃t�@�C�����擾��������΁A

      ��5 : OpenFolder("a.lzh>", &pFolder);
            pFolder->OpenFile("b/c.txt", &pFile);
            �܂���
            OpenFolder("a.lzh>b/", &pFolder;
            pFolder->OpenFile("c.txt", &pFile);
      ��6 : OpenFolder("C:\\a.lzh>b.zip>", &pFolder);
            pFolder->OpenFile("c.txt", &pFile);

      �̂悤�ɂ��āAOpenFolder �ŃA�[�J�C�u���t�H���_�Ƃ��Ď��o���Ă���ɂ���B
      OpenFolder �̓A�[�J�C�u�̒��̃A�[�J�C�u�����ڎ擾�o����B

      OpenFile �ŕԂ��� IKpiFile �ɑ΂��� GetRealFile �����ꍇ�A�o�������
      �f�B���N�g���K�w�𕜌������t�@�C������Ԃ��悤�ɂȂ��Ă���B
      �������A��3 �̂悤�ɐe�t�H���_�̃t�@�C�����J�����ꍇ�͕�������Ȃ����Ƃ�����B
      �܂��A�A�[�J�C�u�v���O�C���̓���̎d�l��A�e�t�H���_���w�肵�ĂȂ��Ă����܂�
      ��������Ȃ����Ƃ�����B

    BOOL WINAPI OpenFolder(const wchar_t* cszName, IKpiFolder **ppFolder);
      EnumFiles �ŕԂ����O�ɑΉ�����t�H���_���擾����B
      �擾���� *ppFolder �́A�K�v���Ȃ��Ȃ����� Release ���邱�ƁB

      cszName �ɂ͑��΃p�X�A��΃p�X���g�p�\�B
      EnumFiles �ł��炩���ߗ񋓂��Ă����K�v�͂Ȃ��B
      ���O�̖����� \ �܂��� / �܂��� > ���K�v�B(���ĂȂ��ƕK�����s)
      �p�X�̋�؂蕶���� \ �� / �̂ǂ�����g�p���Ă��ǂ��B
      �������A�A�[�J�C�u�̒����w�肷��ꍇ�� > ���g�p����B
      ��1 �FOpenFolder("a/", &pFolder);             //���΃p�X
      ��2 �FOpenFolder("a/b/", &pFolder);           // \ �� / �̂ǂ���ł��ǂ�
      ��2'�FOpenFolder("a\\b\\", &pFolder);         // \ �� / �̂ǂ���ł��ǂ�
      ��3 �FOpenFolder("../", &pFolder);            //���΃p�X(�e�t�H���_)
      ��4 �FOpenFolder("C:\\a\\", &pFolder);        //��΃p�X
      ��5 �FOpenFolder("a.lzh>", &pFolder);         //�A�[�J�C�u(���΃p�X)
      ��6 �FOpenFolder("a.lzh>b/", &pFolder);       //�A�[�J�C�u���t�H���_(���΃p�X)
      ��7 �FOpenFolder("C:/a.lzh>b.zip>", &pFolder);//�A�[�J�C�u���A�[�J�C�u(��΃p�X)
      ��8 �FOpenFolder("http://www.hoge.com/", &pFolder);//http    #���ݖ�����
*/
};
///////////////////////////////////////////////////////////////////////////////
//�^�O�擾�֌W
///////////////////////////////////////////////////////////////////////////////
enum{//IKpiTagInfo::GetTagInfo �� dwType �ɓn���l
    KPI_TAGTYPE_NONE = 0, //�{�̂������Ă���^�O�擾���[�`���͎g��Ȃ����Ƃ�ʒm
    KPI_TAGTYPE_ID3V2,    //ID3Tag(v2.2/2.3/2.4)(v1 �͎擾���Ȃ�)
    KPI_TAGTYPE_APE,      //APETag(v2 ���Ȃ���� v1 ���擾)
    KPI_TAGTYPE_PSF,      //PSFTag
    KPI_TAGTYPE_ASF,      //ASFTag(WindowsMedia)
    KPI_TAGTYPE_RIFF,     //RIFF
    KPI_TAGTYPE_MP4,      //MP4(������)
    KPI_TAGTYPE_LYR3TAG,  //Lyrics3Tag
    KPI_TAGTYPE_ID3V1,    //ID3Tag(v1)
    //KPI_TAGTYPE_AUTO = (DWORD)-1, //�g���q�����Ɏ������ʂ��ă^�O�擾(���Ŗ���`�̌`�����܂�)
                          //�񋟂���Ȃ��BGetTagInfo ��1�x���Ă΂Ȃ���� Select ���
                          //�����ŌĂяo�����B
    //�{�̂������Ŏ擾����ꍇ�A�����̃^�O�`�����܂ޏꍇ��
    //�l�����������̂�D��(MP3 �Ȃ� ID3v2 => Ape => RIFF => Lyrics3Tag => ID3v1)
};
enum{//IKpiTagInfo::GetTagInfo �� dwTagGetFlags �ɓn���l
    KPI_TAGGET_FLAG_NONE = 0,          //�^�O�擾���s��Ȃ�
    KPI_TAGGET_FLAG_TITLE  = 0x01,     //�^�C�g�����擾
    KPI_TAGGET_FLAG_ARTIST = 0x01 << 1,//�A�[�e�B�X�g���擾
    KPI_TAGGET_FLAG_ALBUM  = 0x01 << 2,//�A���o��
    KPI_TAGGET_FLAG_GENRE  = 0x01 << 3,//�W������
    KPI_TAGGET_FLAG_DATE   = 0x01 << 4,//�N
    KPI_TAGGET_FLAG_COMMENT= 0x01 << 5,//�R�����g
    KPI_TAGGET_FLAG_TRACK  = 0x01 << 6,//�g���b�N�ԍ�/���A�f�B�X�N�ԍ�/��(�ʂɎw��͕s��)
    KPI_TAGGET_FLAG_ALBMART= 0x01 << 7,//�A���o���A�[�e�B�X�g
    KPI_TAGGET_FLAG_COPYRIGHT= 0x01 << 8,//���쌠
    KPI_TAGGET_FLAG_RG     = 0x01 << 9,  //���v���C�Q�C���e��(�ʂɎw��͕s��)
    KPI_TAGGET_FLAG_PICTURE = 0x01 << 10,//�A���o���A�[�g(���ߍ��݉摜)
    //
    KPI_TAGGET_FLAG_GENERAL= 0x01 << 31, //���̑��S��(�v���O�C�������߂�)
    //KPI_TAGGET_FLAG_GENERAL ���Z�b�g����Ȃ��ꍇ�A��ʓI�łȂ��t�B�[���h�͎擾���Ȃ�
    //���͌�Ō��߂�(�S����31�g����̂ŏ\��?)
    //
    KPI_TAGGET_FLAG_ID3 = KPI_TAGGET_FLAG_TITLE   |
                          KPI_TAGGET_FLAG_ARTIST  |
                          KPI_TAGGET_FLAG_ALBUM   |
                          KPI_TAGGET_FLAG_GENRE   |
                          KPI_TAGGET_FLAG_DATE    |
                          KPI_TAGGET_FLAG_COMMENT |
                          KPI_TAGGET_FLAG_TRACK   |
                          KPI_TAGGET_FLAG_ALBMART, //ID3�^�O�S�ʂ��擾
    KPI_TAGGET_FLAG_ALL = (DWORD)-1,//�S�Ă̏����擾
};

class IKpiTagInfo : public IKpiUnknown
{/* �^�O�擾�p
    ��������͖̂{�̑�
    �v���O�C�����͌Ăяo������
    cszName/cszValue �̏I�[��
     0 �ŏI���ꍇ��     nNameSize/nValueSize �� -1 ��n������
     0 �ŏI���Ȃ��ꍇ�� nNameSize/nValueSize �ɂ��ꂼ��̃o�b�t�@�T�C�Y��
                          �n������(�������ł͂Ȃ����Ƃɒ���)
    �ł��������₷�����\�b�h���ĂׂΗǂ��A���݂��ČĂяo���Ă��\��Ȃ�
    KbMedia Player �� name/value �Ƃ��� utf16 �ŊǗ����Ă��邽�߁A
    wSetValueW ���ł������I�ɓ��삷��(�I�ׂ�Ȃ� wSetValueW �ŌĂԂ��Ƃ𐄏�)
    value ���ʓ|�ł� name �����ł��I�ׂ�Ȃ� wSetValueX �𐄏�

    ��`�ς݂� name �ɂ��Ă� kmp_pi.h ���Q��
*/
public:
    virtual DWORD WINAPI GetTagInfo(IKpiFile *pFile, IKpiFolder *pFolder,
                                        DWORD dwType, DWORD dwTagGetFlags)=0;//�{�̂������Ă���^�O�擾���[�`�����Ăяo��
    virtual DWORD WINAPI GetValue(const wchar_t *cszName, wchar_t *pszValue, int nSize)=0;
    virtual void WINAPI SetOverwrite(BOOL bOverwrite)=0;//�㏑�����[�h�̐؂�ւ�
    virtual void WINAPI SetPicture(DWORD dwType,                 //ID3v2 �� "APIC" �݊�(0�`20)(�s���̏ꍇ�� 0 or 3(Cover (front)))
                                   const wchar_t *cszType,       //"" �̏ꍇ�� dwType ����{�̂������񐶐�(0="Other", 3="Cover (front)", etc...)
                                   const wchar_t *cszMIMEType,   //"image/jpeg", "image/png", "image/gif", "image/bmp" ("" �̏ꍇ�͎�������)
                                   const wchar_t *cszDescription,//�摜�̕\����
                                   DWORD dwWidth, DWORD dwHeight,//�s���ȏꍇ��0
                                   const BYTE *pPictureData, DWORD dwSize)=0;
    //Name=Ansi ��
    virtual void WINAPI aSetValueA(const char    *cszNameA,   int nNameSize,    //Name: Ansi
                                   const char    *cszValueA,  int nValueSize)=0;//Value:Ansi
    virtual void WINAPI aSetValueW(const char    *cszNameA,   int nNameSize,    //Name: Ansi
                                   const wchar_t *cszValueW,  int nValueSize)=0;//Value: Utf16
    virtual void WINAPI aSetValueU8(const char   *cszNameA,   int nNameSize,    //Name: Ansi
                                    const char   *cszValueU8, int nValueSize)=0;//Value: Utf8
    //Name=utf16 ��
    virtual void WINAPI wSetValueA(const wchar_t  *cszNameW,   int nNameSize,    //Name: utf15
                                   const char     *cszValueA,  int nValueSize)=0;//Value; Ansi
    virtual void WINAPI wSetValueW(const wchar_t  *cszNameW,   int nNameSize,    //Name: Utf16
                                   const wchar_t  *cszValueW,  int nValueSize)=0;//Value: Utf16
    virtual void WINAPI wSetValueU8(const wchar_t *cszNameW,   int nNameSize,    //Name; utf16
                                    const char    *cszValueU8, int nValueSize)=0;//Utf8: utf8
    //Name=utf8 ��
    virtual void WINAPI u8SetValueA(const char    *cszNameU8,  int nNameSize,    //Name: utf8
                                    const char    *cszValueA,  int nValueSize)=0;//Value: Ansi
    virtual void WINAPI u8SetValueW(const char    *cszNameU8,  int nNameSize,    //Name; utf8
                                    const wchar_t *cszValueW,  int nValueSize)=0;//Value: Utf16
    virtual void WINAPI u8SetValueU8(const char   *cszNameU8,  int nNameSize,    //Name: utf8
                                     const char   *cszValueU8, int nValueSize)=0;//Value: Utf8
/*
    DWORD WINAPI GetTagInfo(IKpiFile *pFile, IKpiFolder *pFolder,
                                  DWORD dwType, DWORD dwTagGetFlags);

      �����Ă���f�[�^�̃^�O�`�����{�̂����m�̂��̂ł���ꍇ�A�{�̂����^�O�擾
      ���[�`�����g�����Ƃ��o����B
      �v���O�C�����Ǝ��Ɏ擾��������ʒm����ꍇ�� xSetValueX/SetPicture ���Ăяo���B
      GetTagInfo �Ŗ{�̂��擾�������ƃv���O�C�����ʒm�������̂ǂ��炪
      �K�p����邩�͏㏑�����[�h(SetOverWrite �Őݒ�)�ƌĂяo���̏����Ō��܂�B

      �{�̂̃^�O�擾���[�`����S���g��Ȃ��ꍇ�� dwType �� KPI_TAGTYPE_NONE ��
      �n���� GetTagInfo ���Ăяo�����ƁB(�Ă΂Ȃ��� Select ����߂�����ɖ{�̂�
      �^�O�擾���s��)

      pFile/pFolder:
        �ʏ�� IKpiDecoderModule::Open �œn���ꂽ IKpiFile/IKpiFolder �����̂܂ܓn���B
        �Ăяo���O�Ƀt�@�C���|�C���^��擪�Ɉړ�����K�v�͂Ȃ��A�Ăяo���O���
        �t�@�C���|�C���^���ς��Ȃ��B

        pFile ���v���O�C�����Ǝ��Ɏ�������ꍇ�ARead �� Seek ������������Ώ\���B
        ���̑��̃��\�b�h(GetBuffer/GetSize/CreateClone)���������Ă��{�̂��Ăяo��
        ���Ƃ͂Ȃ��B

        �v���O�C�����Ǝ��Ɏ�������K�v�������邱�Ƃ͏��Ȃ��������Akbtak.kpi ���Y��
        ���Ă���B�����̕K�v������ꍇ�� kbtak.kpi �̃\�[�X�R�[�h���Q�l�ɂ��邱�ƁB

        ���ݖ{�̂��Ή����Ă��� dwType �� pFolder ���K�v�ȃ^�O�`���͂Ȃ����߁A
        pFolder �� NULL ��n���Ă��ǂ��BpFolder ���K�v�ȃ^�O�`���ɑΉ������ꍇ��
        ���̌`���ɂ��Ă͓n���悤�ɂ��邱�ƁBpFolder ���K�v�Ȍ`���� NULL ��n
        ���Ă�����͂��邪�����������擾���邱�Ƃ͏o���Ȃ��B

        GetTagInfo �� IKpiDecoder::Select/IKpiPlayer::Select ���ŌĂяo�����̂�
        ���邽�߁A�Ăяo���̎��_�ł̓f�R�[�h/���t���܂��J�n���Ă��Ȃ��B
        ���̂��߁A�f�R�[�h���C�u�������f�R�[�h�O�̒i�K�ł��o�b�N�O���E���h�ŃX���b�h��
        �쐬���� pFile �ɃA�N�Z�X����悤�Ȏ����ɂȂ��Ă��Ȃ�����AOpen ���� IKpiFile
        �����̂܂ܓn���Ă����͋N����Ȃ��B

        �����f�R�[�h���C�u�����̎�����AGetTagInfo �̌Ăяo������ pFile ��
        �ʃX���b�h�Ŏg�p����ꍇ�́ACreateClone �������̂�n�����ƁB
        (���̂悤�ȃ��C�u�����͏��Ȃ��Ǝv����)

      dwType: �^�O�̌`��
        KPI_TAGTYPE_NONE : �{�̂������Ă���^�O�擾���[�`�����g��Ȃ����Ƃ�ʒm
                           (pFile/pFolder/dwTagGetFlags �͖��������
        KPI_TAGTYPE_ID3V2: ID3Tag(v2.2/2.3/2.4)(v1 �͎擾���Ȃ�)
                           (pFolder �͖��������)
        KPI_TAGTYPE_APE  : APETag(v2 ���Ȃ���� v1 ���擾)
                           (pFolder �͖��������)
        KPI_TAGTYPE_PSF  : PSFTag
                           (pFolder �͖��������)
        KPI_TAGTYPE_ASF  : ASFTag(WindowsMedia)
                           (pFolder �͖��������)
        KPI_TAGTYPE_RIFF : RIFF
                           (pFolder �͖��������)
        KPI_TAGTYPE_MP4  : MP4(������)((DWORD)-1 ��Ԃ�)
                           (pFolder �͖��������)
        KPI_TAGTYPE_LYR3TAG: Lyrics3Tag
                           (pFolder �͖��������)
        KPI_TAGTYPE_ID3V1:   ID3Tag(v1)
                           (pFolder �͖��������)
        KPI_TAGTYPE_AUTO ; �g���q�����Ɏ������ʂ��ă^�O�擾(���Ŗ���`�̌`�����܂�)
                           �񋟂���Ȃ��BSelect ����߂�܂ł� GetTagInfo ��
                           1 �x���Ă΂Ȃ���Ύ����ŌĂяo�����B

      �^�O�̌`����������ނ���ꍇ�� GetTagInfo �𕡐���Ăяo���B
      dwType �ɉ������^�O�擾�ɐ�������(�w�肳�ꂽ�`���̃^�O�����܂�ł����ꍇ)
      �� 0 ��Ԃ��B�^�O�`������������ꍇ�A0 ��ԋp��͗D�揇�ʂ��Ⴂ�`���ł�
      GetTagInfo ���Ă΂Ȃ��悤�ɂ��邱�ƂŃ^�O�擾���ȗ����邱�Ƃ��o����B

      �^�O�`�����Ȃ��ꍇ�� 1 ��Ԃ��B�w��� dwType �ɖ{�̂����Ή�(������)�̏ꍇ
      �� (DWORD)-1 ��Ԃ��BdwType �� KPI_TAGTYPE_NONE �̏ꍇ�͏�� 1 ��Ԃ��B

      GetTagInfo ���Ă΂�Ȃ������ꍇ�A�{�̂� IKpiDecoder::Select ����
      �߂�����A�f�[�^�̊g���q�����ɂ����f�t�H���g�̃^�O�擾�����݂�BSelect
      ����߂�����{�̂Ƀ^�O�擾�����Ȃ��ꍇ�� GetTagInfo ���Ăяo�����ƁB
      KPI_TAGTYPE_NONE �ȊO���n���ꂽ�ꍇ���A�{�̂� Select ����߂������
      �^�O�擾���Ȃ��B

      KPI_TAGTYPE_AUTO �͒񋟂���Ȃ��BSelect ����߂�܂ł� GetTagInfo ��
      1�x���Ă΂�Ȃ���� Select ��ɑ������郋�[�`���������ŌĂяo����邪�A
      Select ����߂�O�ɖ����I�ɌĂяo�����Ƃ͏o���Ȃ��B�v���O�C�����g���^�O��
      �`���Ƃ��̍\����m���Ă���ꍇ�́AKPI_TAGTYPE_AUTO �ɂ͗��炸�A�����I��
      KPI_TAGTYPE_ID3 ���̋�̓I�Ȍ`����n�����AKPI_TAGTYPE_NONE ��n���ăv���O
      �C�����g�Ŏ擾���邱�ƁB

      dwTagGetFlags: �擾����^�O�̃t�B�[���h

      KPI_TAGGET_FLAG_TITLE/ARTIST/ALBUM etc �̑g�ݍ��킹�B
      �ʏ�� IKpiDecoder::Select �� dwTagGetFlags �����̂܂ܓn�����A���̒l����
      �s�v�ȃt�B�[���h�����������̂�n���B

      �Ⴆ�΃^�C�g���̓v���O�C���Ǝ��Ɏ擾����ꍇ�� dwTagGetFlags ����
      KPI_TAGGET_FLAG_TITLE ��������(dwTagGetFlags & ~KPI_TAG_FLAG_TITLE)
      ����Ăяo�����A�Ăяo����� SetOverwrite(TRUE) ���Ă��� xSetValueX ����B

      �s�v�ȃt�B�[���h���t���O����O���Ă��A�`����{�̂̃o�[�W�����ɂ���Ă�
      �O�����t�B�[���h�̏����擾���Ă��܂����Ƃ�����B���̂��߃t���O�̏���
      �͎Q�l���x�ɂƂǂ߁A�s�v�ȃt�B�[���h���m���Ƀv���O�C���Ǝ��Ŏ擾�����l
      �ɐݒ肵�����ꍇ�� GetTagInfo �Ăяo����� SetOverwrite(TRUE)
      ���Ă��炻�̃t�B�[���h�̒l�� xSetValueX ���邱�ƁB

      KPI_TAGGET_FLAG_XXX �Œ�`����Ȃ��t�B�[���h�̒l���擾�������ꍇ��
      dwTagGetFlags �� KPI_TAGGET_FLAG_GENERAL ���Z�b�g���邱�ƁB

    DWORD WINAPI GetValue(const wchar_t *cszName, wchar_t *pszValue, int nSize);

      cszName �ɑ�������t�B�[���h�̒l���擾����B
      GetTagInfo �Ăяo����ɖ{�̂��擾�����l���v���O�C�����g�������ꍇ
      �ɌĂяo���B�v���O�C�������g�� xSetValueX �����f�[�^���擾�o����B
      nSize �� pszValue �̃o�b�t�@�T�C�Y�B(�������P�ʂł͂Ȃ�)
      �߂�l�� pszValue �ւ̃R�s�[�ɕK�v�ȃo�b�t�@�̃T�C�Y�B�T�C�Y������Ȃ�
      �ꍇ�͐؂�̂Ă���B

      ���v���C�Q�C�����擾�o���邪�A���v���C�Q�C���̏����͖{�̂��s�����߁A
      �v���O�C���ŏ�������ׂ��ł͂Ȃ��B

      �t�B�[���h���̗񋓂͏o���Ȃ����߁A�擾������t�B�[���h�����\�ߕ�������
      ����ꍇ�ɂ����g���Ȃ��B(�L�����Ȃ��̂őΉ��͌����킹��)

    void WINAPI SetOverwrite(BOOL bOverwrite);

      �㏑�����[�h�̐؂�ւ��B
      �擾�ς݂̃t�B�[���h���ɑ΂��� xSetValueX/SetPicture �����Ƃ��A
      �㏑�����[�h�� TRUE �̏ꍇ�A�ォ��ʒm���������K�p�����B
      �㏑�����[�h�� FALSE �̏ꍇ�A��ɒʒm���������K�p�����B

      IKpiDecoder::Select �Ăяo������̏㏑�����[�h�� FALSE �ł���B
      �]���� GetTagInfo ��� SetOverwrite �����Ƀv���O�C���Ǝ��̏���
      �ʒm���Ă��㏑������Ȃ����Ƃɒ��ӁB

      �Ⴆ�΃^�O�̌`���� ID3v2 �̏ꍇ�AGetDefaultTagInfo(KPI_TAGTYPE_ID3) ��
      �Ăяo���΁AID3v2 �Ƀ��v���C�Q�C����񂪊܂܂�Ă���Ζ{�̂�������擾����B
      �����Ă���`���� ID3v2 �Ƃ͕ʂɖ{�̂��m��Ȃ����v���C�Q�C�������܂݁A�����
      ID3v2 �����D��x��Ⴍ�������Ƃ����󋵂ł́A

      GetTagInfo(pFile, pFolder, KPI_TAGTYPE_ID3, dwTagGetFlags);
      SetOverwrite(FALSE);//�����l�� FALSE �Ȃ̂Ŏ��ۂ͌Ăяo���s�v
      wSetValueW(L"replaygain_track_gain", ...");

      �̂悤�ɂ���΁AID3v2 �Ƀ��v���C�Q�C����񂪊܂܂�Ă��Ȃ��ꍇ�ɂ�
      �v���O�C�����ʒm������񂪓K�p����AID3v2 �Ɋ܂܂�Ă���ꍇ�� ID3v2 �̕�
      ���K�p�����B���̗�� SetOverwrite �� TRUE ��n�����ꍇ�̓v���O�C����
      �ʒm���������K�p�����B

      SetOverwrite �͌Ăяo����̑S�Ă� xSetValueX/SetPicture �ɑ΂��ēK�p�����B
      �r���ŏ㏑�����[�h��؂�ւ��邱�Ƃ��o����B
      ��� xSetValueX/SetPicture ���Ă��� SetOverwrite ���� GetTagInfo
      ���邱�Ƃ��o����B

      Select ����߂�܂ł� GetTagInfo �� 1 �x���Ă΂�Ȃ��ꍇ�A���̌��
      �{�̂̃^�O�擾�� Select ����߂钼�O�̏㏑�����[�h�ŏ��������B

    void WINAPI SetPicture(DWORD dwType,                 //ID3v2 �� "APIC" �݊�(0�`20)(�s���̏ꍇ�� 0 or 3(Cover (front)))
                           const wchar_t *cszType,       //"" �̏ꍇ�� dwType ����{�̂������񐶐�(0="Other", 3="Cover (front)", etc...)
                           const wchar_t *cszMIMEType,   //"image/jpeg", "image/png", "image/gif", "image/bmp" ("" �̏ꍇ�͎�������)
                           const wchar_t *cszDescription,//�摜�̕\����
                           DWORD dwWidth, DWORD dwHeight,//�s���ȏꍇ��0
                           const BYTE *pPictureData, DWORD dwSize)=0;

      �摜(�A���o���A�[�g)��ʒm����B��{�I�� ID3v2 �� "APIC" �݊��ł���B

      dwType: �摜�̎�ށBID3v2 �� "APIC" �݊��̒l��n���B
              cszType != "" �̏ꍇ�͖��������B

         0: "Other"
         1: "32x32 pixels 'file icon' (PNG only)"
         2: "Other file icon"
         3: "Cover (front)"
         4: "Cover (back)"
         5: "Leaflet page",
         6: "Media (e.g. label side of CD)",
         7: "Lead artist/lead performer/soloist",
         8: "Artist/performer",
         9: "Conductor",
        10: "Band/Orchestra",
        11: "Composer",
        12: "Lyricist/text writer",
        13: "Recording Location",
        14: "During recording",
        15: "During performance",
        16: "Movie/video screen capture",
        17: "A bright coloured fish",
        18: "Illustration",
        19: "Band/artist logotype",
        20: "Publisher/Studio logotype"

        �{�̂����m(0�`20 �͈̔͊O)�̒l���n���ꂽ�ꍇ�� 0("Other") �ƌ��Ȃ��B

      cszType: �摜�̎�ޖ��B"" �̏ꍇ�� dwType �ɉ�������ޖ��ɂȂ�B
               NULL ��n���Ă͂����Ȃ��B

      cszMIMEType: MIME �̃^�C�v�B�摜�̌`���BNULL ��n���Ă͂����Ȃ��B
        �Ή����Ă���͈̂ȉ��̒ʂ�B

        "image/jpeg" : JPEG �`�� ("image.jpg" ���󂯕t���邪�������Ȃ�)
        "image/png"  : PNG �`��
        "image/gif"  : GIF �`��
        "image/bmp"  : BMP �`��

        cszMIMEType �� "" ��n�����ꍇ�͖{�̂��摜�̌`�����������ʂ���B
        �{�̂����m�̕����񂪓n���ꂽ�ꍇ�́A�摜�̕\���Ɏ��s����B

      cszDescription: �摜�̕\�����B�Ȃ��ꍇ�� "" ��n�����ƁB
                      NULL ��n���Ă͂����Ȃ��B

      dwWidth/dwHeight : �摜�f�[�^�̉����Əc���B�s���ȏꍇ�� 0 ��n���B

      pPictureData, dwSize: �摜�f�[�^�Ƃ��̃T�C�Y

      ���y�f�[�^�Ɠ���t�H���_���ɂ���摜�͖{�̂������œǂݎ�邽�߁A
      �ʒm����K�v�͂Ȃ��B

    void WINAPI xSetValueX(const char/wchar_t *cszName,  int nNameSize,
                           const char/wchar_t *cszValue, int nValueSize);

      cszName/cszValue �̕�����̏I�[�� 0 �ŏI���ꍇ�� nNameSize/nValueSize=-1
      �Ƃ��邱�ƁB�I�[�� 0 �ŏI���Ȃ��ꍇ�̓o�b�t�@�T�C�Y(�������P�ʂł͂Ȃ�)��n�����ƁB
      cszName ���d�������ꍇ�A�㏑�����[�h�ɂ���ď㏑������邩��������邩�����܂�B

      ���d�l�� IKmpTagInfo::SetValue �Ƃ̌݊�������AcszName/cszValue �Ƃ���
      NULL ��n�����ꍇ�ɂ� GetTagInfo(NULL, NULL, KPI_TAGTYPE_NONE, 0);
      ���Ăяo���̂Ɠ������ʂ�����B(Select ����߂�����ɖ{�̂��^�O�擾���Ȃ��Ȃ�)
*/
};
///////////////////////////////////////////////////////////////////////////////
//�ݒ�֌W
///////////////////////////////////////////////////////////////////////////////
enum{//KPK_CFG_INFO::dwType(�ݒ荀�ڂ̌^)
    KPI_CFG_TYPE_BOOL,   //bool(�\���� false/true, �f�[�^�� 0/1)
    KPI_CFG_TYPE_INT,    //INT64
    KPI_CFG_TYPE_FLOAT,  //double
    KPI_CFG_TYPE_STR,    //������
    KPI_CFG_TYPE_BIN,    //�o�C�i��(�ݒ��ʂ̓v���O�C�����Ǝ��ɍ쐬)
    KPI_CFG_TYPE_FILE,   //�t�@�C����(�t�@�C���I���_�C�A���O��\��)
    KPI_CFG_TYPE_FOLDER, //�t�H���_��(�t�H���_�I���_�C�A���O��\��)(������ \ �t��)
    KPI_CFG_TYPE_EXT,    //�g���q('.'���܂݁A'/' �ŋ�؂�ꂽ������)(��F".mp1/.mp2/.mp3")
};
enum{//IKpiXXXModule::ApplyConfig() �̖߂�l(�ݒ肪���f�����^�C�~���O)
    KPI_CFGRET_OK,          //�ݒ�͑����ɔ��f�����
    KPI_CFGRET_NOTIFY,      //�f�R�[�_�C���X�^���X���ւ̕ύX�ʒm���K�v(IKpiXXX::Update()���Ă΂ꂽ�甽�f)
    KPI_CFGRET_RELOAD_DATA, //�f�[�^�̍ă��[�h���K�v(����IKpiXXXModule::Open()���Ă΂ꂽ�����甽�f)
    KPI_CFGRET_RELOAD_KPI,  //�v���O�C���̍ă��[�h���K�v
    KPI_CFGRET_REBOOT_PLAYER,//�v���C���[�{�̂̍ċN�����K�v(���ʂȎ���Ȃ�����
                             //RELOAD_KPI �ő���锤����)
    KPI_CFGRET_REBOOT_OS,    //OS �̍ċN�����K�v
    KPI_CFGRET_UNKNOWN = (DWORD)-1
                             //�s��(�O���v���O�������Ăяo�����A�ݒ�֌W�̈ˑ�
                             //�R�[�h�̏ڍׂ�������Ȃ���)
};
enum{//�Z�N�V������/�L�[��/�l�̍ő咷(�������P�ʁA�I�[�� 0 ���܂�)
    KPI_MAX_SECTION = 1024,//�Z�N�V������
    KPI_MAX_KEY = 1024,    //�L�[��
    KPI_MAX_VALUE = 4096   //�l
    //�Z�N�V�������ƃL�[���͓��ʂȎ���(�v���O�����I�ɐ�������铙)���Ȃ�����A
    //�펯�I�Ȓ����ɗ}����悤�ɂ��邱��
};
struct KPI_CFG_SECTION
{//IKpiConfigEnumerator::EnumSection �ɓn���\����
 //�Z�N�V�����̕\�����ƃw���v(�Ή����Ȃ��ꍇ�͕s�v)
    const wchar_t *cszSection;//�Z�N�V������(�}���`�o�C�g�����񐄏�������ނ𓾂Ȃ��ꍇ�͉�)
    const wchar_t *cszSecDesc;//�Z�N�V�����̕\����(NULL �̏ꍇ�� cszSection �Ɠ���)
    const wchar_t *cszSecHelp;//�Z�N�V�����Ɋւ���w���v(NULL �̏ꍇ�͔�\��)
/*
    �e�핶����͖{�̂ɂ���ăR�s�[����邽�߁A�ÓI�ȃo�b�t�@�Ɏ��K�v�͂Ȃ�
    (EnumSection �ɓn���O�ɓ��I�ɕ����񐶐��A�߂����������Ă����Ȃ�)

    EnumKey �ŗ񋓂����Z�N�V�����ɕ\�����ƃw���v���ǉ�����ꍇ�ɕK�v
    (�\�������Z�N�V�������Ɠ����Ńw���v���Ȃ��ꍇ�͕s�v)

    cszSection:
      �E�Z�N�V������(�}���`�o�C�g�񐄏�)
      �ENULL �� "" �͎g�p�s��(���ɂȂ��Ȃ� "General" ��v���O�C���̃t�@�C���������g��)
      �E�}���`�o�C�g�����񐄏��B�g���Ȃ�Ȃ�ׂ� cszSecDesc �̕��ɂ���B
      �E�������A�f�o�C�X�v���O�C���ł̓Z�N�V��������
        �u�f�o�C�X��-���ʎq�v
        (��F�v���C�}�� �T�E���h �h���C�o�[-{00000000-0000-0000-0000-000000000000})
        �Ƃ����ꍇ�͖{�̂��f�o�C�X�p�Z�N�V�����Ɣ��f���邽�߁A�g�p���Ă��ǂ��B
      �E�f�o�C�X�p�Z�N�V�����̂悤�ɁA���O���v���O�����I�Ɏ擾���铙�A��ނ𓾂Ȃ�
        �ꍇ�̓}���`�o�C�g�������g�p���Ă��ǂ��B
      �E�Z�N�V�������̕�����̒����� KPI_MAX_SECTION �ȓ��ɗ}���邱�ƁB
    cszSecDesc:
      �E�Z�N�V�����̕\����
      �E�ݒ��ʏ�� cszSection �ł͂Ȃ� cszSecDesc �̕����\�������B
      �ENULL or "" �̏ꍇ�� cszSection �Ɠ����ƌ��Ȃ����B
    cszSecHelp:
      �E�Z�N�V�����̃w���v(NULL or ""�̏ꍇ�͔�\��)

    IKpiConfig �ɑΉ����Ă��Ă��AIKpiConfigEnumerator::EnumSection �̌Ăяo��
    �͕K�{�ł͂Ȃ��B(EnumKey �̌Ăяo���͕K�{)
    EnumSection ���Ă΂�Ȃ��ꍇ�AcszSecDesc/cszSecHelp �Ƃ��� NULL ��
    ���Ȃ����B(�\�������Z�N�V�������Ɠ����Ńw���v���Ȃ��ƌ��Ȃ����)
    �]���āAcszSecDesc/cszSecHelp �Ƃ��� NULL or "" �Ȃ� EnumSection ���Ă�
    �K�v�͂Ȃ��B

    EnumKey �ŗ񋓂���Ȃ��Z�N�V�������� EnumSection �ŗ񋓂����ꍇ�A
    �ݒ��ʏ�̓Z�N�V�������F������邪�ݒ荀�ڂ��Ȃ���ԂƂȂ�B
    (�Ȃ̂Ő������Ȃ�)
*/
};
struct KPI_CFG_KEY
{//IKpiConfigEnumerator::EnumKey �ɓn���\����
 //�e��ݒ�̃Z�N�V��������L�[���A�����l�A�͈�
    DWORD dwType;             //�^(KPI_CFG_TYPE_BOOL/INT etc...)
    const wchar_t *cszSection;//�Z�N�V������(�\�����ƃw���v�� kPI_CFG_SECTION ���Q��)
    const wchar_t *cszKey;    //�L�[��(�}���`�o�C�g�����񐄏�)
    const wchar_t *cszKeyDesc;//�L�[�̕\����(NULL �̏ꍇ�� cszKey ��\��)
    const wchar_t *cszDefault;//�����l(�l�𕶎���)("1", "256", "3.14", "abcd")
    const wchar_t *cszMin;    //�ŏ��l
    const wchar_t *cszMax;    //�ő�l(������̏ꍇ�͍ő咷(�I�[��0���܂�)(�������P��))
    const wchar_t *cszList;   //�l�̃��X�g���^�u����\t�ŋ�؂��ĕ�����("1\t3\t5", "abc\tabcd\tabc")
    const wchar_t *cszListDesc;//�l�̃��X�g�̕\����
    const wchar_t *cszHelp;   //�w���v(NULL �̏ꍇ�͔�\��)
/*
    �e�핶����͖{�̂ɂ���ăR�s�[����邽�߁A�ÓI�ȃo�b�t�@�Ɏ��K�v�͂Ȃ�
    (EnumKey �ɓn���O�ɓ��I�ɕ����񐶐��A�߂����������Ă����Ȃ�)
    NULL �� "" �͓����ƌ��Ȃ�
    NULL �� "" �� BOOL/INT/FLOAT �Ő��l���o���Ȃ��l�ƌ��Ȃ�(0�ł͂Ȃ�)
    dwType:
      �EKPI_CFG_TYPE_BOOL �̏ꍇ�� cszMin/cszMax/cszList �͖��������(NULL �ɂ��邱��)
        �\����̒l�� false �� true �����A�f�[�^��� 0 �� 1 �ƂȂ�
        (cszDefault ������ɍ��킹�� "0" �� "1" ��n������)
      �EKPI_CFG_TYPE_STR �̏ꍇ�� cszMin �͖��������(NULL �ɂ��邱��)
      �EKPI_CFG_TYPE_EXT �̏ꍇ�� cszMin �͖��������(NULL �ɂ��邱��)
        �g���q�� '.' ���܂݁A'/' �ŋ�؂�ꂽ������(��F".mp1/.mp2/.mp3")
        KPI_DECODER_MODULEINFO::cszSupportExts/cszMultiSongExts ���ɂ��̂܂ܓn�����Ƃ��o����
        ������͏������ɕϊ�����A�g���q�̏d���`�F�b�N���s����
      �EKPI_CFG_TYPE_BIN �̏ꍇ�� cszDefault/cszMin/cszMax/cszList/cszListDesc �͖��������(NULL �ɂ��邱��)
        �{�̂������Ƃ��� cszSection �� cszKey ��n����  ApplyConfig() ���Ăяo���̂ŁA
        �����Ńv���O�C�����Ǝ��̃_�C�A���O��\�����Đݒ肷��
        �ݒ�̕ۑ��ꏊ��{�̂ɊǗ�������ꍇ�� SetBin/GetBin ���Ăяo������
    cszSection:
      �E�Z�N�V������(�}���`�o�C�g�񐄏�)
      �ENULL �� ""�͎g�p�s��(���ɂȂ��Ȃ� "General" ��v���O�C���̃t�@�C���������g��)
      �E�f�o�C�X�v���O�C���ł̓f�o�C�X���Ǝ��ʎq�����ɂ����Z�N�V�������쐬���A
        �f�o�C�X���Ƃɐݒ�o����悤�ɂ���Ɨǂ�
        �f�o�C�X��-���ʎq(��F�v���C�}�� �T�E���h �h���C�o�[-{00000000-0000-0000-0000-000000000000})
        �Ƃ������O�̃Z�N�V�����͊Y���f�o�C�X�p�̃Z�N�V�����ƌ��Ȃ���A�K�v��
        �����Đݒ��ʏ�ŃZ�N�V�����������I�������
      �E�����̌`����g���q�ɑΉ�����v���O�C���ł͌`�����ƂɃZ�N�V�������쐬���A
        �`�����Ƃɐݒ�o����悤�ɂ���Ɨǂ�
      �E�Z�N�V�����̕\�����ƃw���v�ɂ��Ή��������ꍇ�� KPI_CFG_SECTION ���Q��
    cszKey:
      �E�L�[��(�}���`�o�C�g�����񐄏�)
      �ENULL/�󕶎���͎g�p�s��
      �E���݂̎����ł̓L�[���� '=' ���g�p���邱�Ƃ͏o���Ȃ�('=' �ȍ~��l�ƌ��Ȃ��Ă��܂�)
      �E�d���s��(�ォ��񋓂��ꂽ�����̗p)
    cszKeyDesc:
      �E�L�[�̕\����(�ۑ��� cszKey �ŊǗ�)
      �ENULL �̏ꍇ�� cszKey ��\��
      �EcszKey ���قȂ��Ă���� cszKeyDesc �̏d�����\�����A�ݒ��ʏ�ł̋��
        �����Â炢����(����Z�N�V�������ł�)�񐄏�
    cszDefault:
      �ENULL ���ABOOL/INT/FLOAT �Ő��l���o���Ȃ��ꍇ�� "0" �ƌ��Ȃ�
      �EBOOL �̕\����̒l�� false/true �����A�f�[�^��� 0 �� 1 �ƂȂ邽�߁A
        cszDefault ������ɍ��킹�� "0" �� "1" ��n������
    cszMin/cszMax:
      �ENULL ���AINT �Ő��l���o���Ȃ��ꍇ�� cszMin �ł� _I64_MIN, cszMax �ł� _I64_MAX �ƌ��Ȃ�
      �ENULL ���AFLOAT �Ő��l���o���Ȃ��ꍇ�� cszMin �ł� -DBL_MAX, cszMax �ł� DBL_MAX �ƌ��Ȃ�
      �EBOOL �ł� cszMin/cszMax �͖��������(NULL �ɂ��邱��)
      �ESTR �ł� cszMin �͖��������(NULL �ɂ��邱��)
                 cszMax �͕�����̍ő咷(�������P��)�ƌ��Ȃ�(�I�[��0���܂�)
      �EFILE �̏ꍇ��
          cszMin �� GetOpenFileName �� OPENFILENAME �\���̂ɓn���g���q�̃t�B���^
          (OPENFILENAME::lpstrFilter �ɑ������邪�A��؂�� \0 �ł͂Ȃ��� \t �Ƃ��邱�Ƃɒ���)
          ��0:"�\��������1\t�g���q�̃��X�g1\t�\��������2\t�g���q�̃��X�g2\t..."
          ��1:"���s�t�@�C��(*.exe)\t*.exe"
          ��2:"Txt/Doc files(*.txt;*.doc)\t*.txt;*.doc\tTxt files(*.txt)\t*.txt\tDoc files(*.doc);*.doc")
          cszMax �͕�����̍ő咷(�������P��)(�I�[��0���܂�)
      �EFOLDER �̏ꍇ�� cszMin �͖��������
                        cszMax �͕�����̍ő咷(�������P��)(�I�[��0���܂�)
      �EEXT �̏ꍇ�� cszMin �͖��������
                     cszMax �͕�����̍ő咷(�������P��)(�I�[��0���܂�)
      �EcszList!=NULL �� cszMin/cszMax ���Ƃ��� NULL �̏ꍇ�� cszList ���̒l�ȊO�͎g�p�s��
      �EcszList!=NULL �� cszMin/cszMax �̂ǂ��炩�� ��NULL �̏ꍇ�� cszList ���̒l�ȊO���g�p�\
    cszList:
      �EcszList!=NULL �̏ꍇ�͒l���R���{�{�b�N�X�őI���\�ɂȂ�
      �E��������^�u����(\t)�ŋ�؂�
        ��1: cszList = "1\t10\t100" ���� 1, 10, 100 ��I���\�ɂȂ�
        ��2: cszList = "abc\tdef\tghi" ���� abc, def, ghi ��I���\�ɂȂ�
        ��3: cszList = "\tabc\tdef" ���� abc, def �ɉ����ċ󕶎�����I���\�ɂȂ�
      �EcszList=NULL �ɂ���� cszDefault ���I���\�ɂȂ�
      �EcszMin/cszMax ������ NULL �̏ꍇ�̓��X�g���̒l�ȊO�͎g�p�s��
        (���X�g�ȊO�̒l�����͂��ꂽ�� cszDefault �ɂȂ�)
      �EcszMin/cszMax �̂ǂ��炩���� NULL �̏ꍇ�� cszList ���̒l�ȊO���g�p�\
    cszListDesc:
      �E�l�̕\�����̃��X�g�BcszList �����������Ȃ��ꍇ�A����Ȃ����̕\���� cszList �Ɠ���
        ��1�FcszList="1\t2\t3", cszListDesc="�l1\t�l2\t�l3" �̏ꍇ�A�ݒ��ʏ�ł�
            �l1, �l2, �l3 �ƕ\������邪�A�f�[�^��̒l�� 1, 2, 3 �ƂȂ�
        ��2�FcszList="0\t128\t256", cszDescList="�[��" �̏ꍇ�A�ݒ��ʏ�̕\����
            �[��, 128, 256 �ƂȂ�A�f�[�^��̒l�� 0, 128, 256 �ƂȂ�
      �ECFG_TYPE_BOOL �� cszType=CFG_TYPE_INT, cszMin=NULL, cszMax=NULL,
                         cszList="0\t1", cszDescList="false\ttrue" �Ɠ���
      �ENULL �̏ꍇ�� cszList ���g����
    cszHelp:
      �E��NULL �̏ꍇ�̓w���v��\��
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiConfigEnumerator : public IKpiUnknown
{//�e��ݒ荀�ڂ̏����
 //�{�̂������B�v���O�C�����͌Ăяo������
public:
    virtual BOOL WINAPI EnumSection(const KPI_CFG_SECTION *pSection)=0;
    virtual BOOL WINAPI EnumKey(const KPI_CFG_KEY *pKey)=0;
/*
    BOOL WINAPI EnumSection(const KPI_CFG_SECTION *pSection);
      EnumKey �ŗ񋓂���Z�N�V�����̕\�����ƃw���v���K�v�ȏꍇ�͌Ăяo���B
      EnumSection ���Ă΂Ȃ��ꍇ�A�Z�N�V�����̕\������ KPI_CFG_KEY::cszSection
      ���̂��̂ƂȂ�A�Z�N�V�����̃w���v�͕\������Ȃ��B
      �������O�̃Z�N�V������񋓂����ꍇ�A�ォ��Ăяo���������K�p�����B
      �Z�N�V�����̕\�����ƃw���v�ɑΉ����Ȃ��ꍇ�͌Ăяo���Ȃ��ėǂ��B
      pSection ���̊e�핶����(cszXXX) �̓��e�� EnumSection �����ŃR�s�[����邽�߁A
      �Ăяo����ɕ������ύX�E�j�������Ă��\��Ȃ��B

    BOOL WINAPI EnumKey(const KPI_CFG_KEY *pKey);
      �e��ݒ荀�ڂ̖��O�ƌ^�A�l�̎�蓾��͈͂�{�̂ɒʒm�B
      �ʒm���ꂽ�������ɖ{�̂��ݒ��ʂ����������B
      ��������������ʂ��C�ɓ���Ȃ��ꍇ�̓v���O�C�����œƎ��ɍ쐬���邱�ƁB
      �ŏ��ɒʒm���ꂽ���̂��珇�ɕ\������B
      dwType == KPI_CFG_TYPE_BIN �ł�����̂��ʒm���ꂽ�ꍇ�A
      IKpiXXXModule::ApplyConfig �Őݒ��ʂ��Ăяo���Đݒ���s���B
      �񋓂��ꂽ�^�Ɣ͈͂�{�̂��󂯓��ꂽ�ꍇ�� TRUE ��Ԃ��B
      pKey ���̊e�핶����(cszXXX) �̓��e�� EnumKey �����ŃR�s�[����邽�߁A
      �Ăяo����ɕ������ύX�E�j�������Ă��\��Ȃ��B

    �Z�N�V������ EnumKey/EnumSection �Ő�ɌĂяo�������̂���\�������B

    EnumSection �𕡐���Ăяo�����ꍇ�A��ɌĂяo�������̃Z�N�V��������\��
    �����B�d������Z�N�V������񋓂����ꍇ�A�ォ��񋓂��������K�p�����B

    EnumKey �𕡐���Ăяo�����ꍇ�A��ɌĂяo�������̃Z�N�V����/�L�[����
    �\�������B�d������Z�N�V����/�L�[��񋓂����ꍇ�A�ォ��񋓂�������
    �K�p�����B

    EnumSection �� EnumKey �͂ǂ�����ɌĂяo���Ă��\��Ȃ����A�{�̂̎�����A
    EnumSection ���ɌĂяo���������A�킸�������������ǂ��Ȃ�B

    EnumSection �ŗ񋓂��Ă�̂� EnumKey �ł͗񋓂���Ȃ��悤�ȃZ�N�V������
    �������Ȃ��B

    IKpiConfigEnumerator �̃��\�b�h�� IKpiXXXModule::EnumConfig() �֐����ȊO��
    �Ăяo���Ă͂����Ȃ��B(AddRef ���Ă����ĕʂ̃��\�b�h���ŌĂԂȂ�)
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiConfig : public IKpiUnknown
{//�v���O�C���̐ݒ�
 //�{�̂������B�v���O�C�����͌Ăяo���̂݁B
public:
    //int(INT64)
    virtual void   WINAPI SetInt(const wchar_t *cszSection, const wchar_t *cszKey, INT64  nValue)=0;
    virtual INT64  WINAPI GetInt(const wchar_t *cszSection, const wchar_t *cszKey, INT64  nDefault)=0;
    //float(double)
    virtual void   WINAPI SetFloat(const wchar_t *cszSection, const wchar_t *cszKey, double dValue)=0;
    virtual double WINAPI GetFloat(const wchar_t *cszSection, const wchar_t *cszKey, double dDefault)=0;
    //str(wchar_t*)
    virtual void   WINAPI SetStr(const wchar_t *cszSection, const wchar_t *cszKey, const wchar_t *cszValue)=0;
    virtual DWORD  WINAPI GetStr(const wchar_t *cszSection, const wchar_t *cszKey, wchar_t *pszValue,
                                 DWORD dwSize, const wchar_t *cszDefault)=0;
    //binary
    virtual void   WINAPI SetBin(const wchar_t *cszSection, const wchar_t *cszKey, const BYTE *pBuffer, DWORD dwSize)=0;
    virtual DWORD  WINAPI GetBin(const wchar_t *cszSection, const wchar_t *cszKey, BYTE *pBuffer, DWORD dwSize)=0;
/*
    IKpiConfig �̃C���X�^���X�́A�v���O�C�����G�N�X�|�[�g����֐�

    HRESULT WINAPI kpi_CreateInstance(REFIID riid,
                                      void **ppvObject,
                                      IKpiUnknown *pUnknown);

    �̑�3����(pUnknown) ����擾����B�ʏ�� kpi_CreateConfig �w���p�[�֐���
    �g�p����B�ڍׂ� IKpiUnkProvider ���Q�ƁB

    IKpiConfig �ɑΉ�����ꍇ�Akpi_CreateConfig �ɂ�� IKpiConfig �̃C���X�^���X
    �擾�́A�x���Ƃ� kpi_CreateInstance ���Ŏ��s���邱�ƁB
    kpi_CreateInstance ����߂����i�K�Ŗ��� kpi_CreateConfig ���Ă΂�Ă��Ȃ�
    �ꍇ�A�ݒ�̕ۑ���{�̂ɊǗ������邱�Ƃ͏o���Ȃ��B
    (EnumConfig/ApplyConfig �͗L�������o�C�i���ȊO�̌^�͈����Ȃ�)


    BOOL/INT �ł� SetInt/GetInt �Őݒ�/�擾����B
    FLOAT �ł� SetFloat/GetFloat �Őݒ�/�擾����B
    STR/FILE/FOLDER/EXT �ł� SetStr/GetStr �Őݒ�/�擾����B
    BIN �ł� SetBin/GetBin �Őݒ�/�擾����B

    SetBool/GetBool �͒񋟂���Ȃ��B
      �^�� KPI_CFG_TYPE_BOOL �ł���ꍇ�̃f�[�^��̒l�� 0 / 1 �ł���B(false / true �ł͂Ȃ�)
      GetInt �̖߂�l�� 0 �Ȃ� false, 0 �ȊO�Ȃ� true �Ƃ��ď������邱�ƁB

    Set �����̂Ƃ͈قȂ�^�� Get �����ꍇ�͌^�ϊ������B(SetBin/GetBin �͏���)
      �Ⴆ�� SetInt/SetFloat �����L�[�ɑ΂��� GetStr �����ꍇ�́A�����񉻂��ꂽ
      �l���Ԃ�B

      SetFloat �����L�[�ɑ΂��� GetInt �����ꍇ�͐�����(�����_�ȉ��؂�̂�)
      ���ꂽ�l���Ԃ�B

      SetStr �����L�[�ɑ΂��� GetInt/GetFloat �����ꍇ�͐��l���ɐ�������΂���
      �l���A���s�����ꍇ�̓f�t�H���g�l���Ԃ�B

    SetBin �����L�[�ɑ΂��� GetInt/GetFloat/GetStr �������ʂ͖���`�B
      ���݂̎����ł� base64 �G���R�[�h����������Ƃ��Đݒ��ۑ����Ă��邽�߁A
      GetStr ����� base64 �G���R�[�h���ꂽ�����񂪕Ԃ�B

    SetInt/SetFloat/SetStr �����L�[�ɑ΂��� GetBin �������ʂ͖���`�B
      SetInt/SetFloat/SetStr �͕����񉻂��ĕۑ������BGetBin �Ŏ擾����ꍇ��
      �f�[�^�� base64 �G���R�[�h���ꂽ������ƌ��Ȃ����߁A�ʏ�̓f�R�[�h�Ɏ��s
      ���ă[���N���A���ꂽ�f�[�^���Ԃ邪�A���R�f�R�[�h�ɐ������Ă��܂����Ƃ�
      ���_�I�ɂ͂��蓾��B

    GetStr:
      dwSize �� pszValue �̃o�b�t�@�T�C�Y(�������P�ʂł͂Ȃ�)
      ������̊i�[�ɕK�v�ȃo�b�t�@�̃T�C�Y��Ԃ��B
      �o�b�t�@�T�C�Y������Ȃ��ꍇ�A����Ȃ����̕�����͐؂�̂Ă���B
      �Y������Z�N�V����/�L�[���Ȃ������ꍇ�� cszDefault ���Ԃ���邪�A�L�[��
      ���݂��Ēl���󕶎���̏ꍇ�͋󕶎��񂪕Ԃ邱�Ƃɒ��ӂ��邱�ƁB

    SetBin:
      dwSize �͊i�[���ׂ��o�C�i���f�[�^(pBuffer)�̃o�b�t�@�T�C�Y
      �i�[���ׂ��o�C�i���f�[�^�̐ݒ��ʂ̓v���O�C�����ō쐬����K�v������B

    GetBin:
      dwSize �� pBuffer �̃T�C�Y
      �߂�l�͊i�[�ɕK�v�ȃo�b�t�@�̃T�C�Y
      �o�b�t�@�T�C�Y������Ȃ��ꍇ�ApBuffer �� dwSize �������[���N���A�����(����Ȃ����̐؂�̂Ăł͂Ȃ�)
      �i�[�����o�b�t�@�̃T�C�Y�� dwSize �����������ꍇ�A�c��̓[���N���A�����B
      64bit �ł� 32bit �łƂŃo�C�i���f�[�^�Ɍ݊������Ȃ��ꍇ�́A�L�[����ς���
      �Ȃǂ��Ă������ƁB


  ���ӎ����F

    IKpiXXXModule::EnumConfig �ŗ񋓂���Ȃ�(���������̐ݒ��ʂŐݒ�o���Ȃ�)
    ���ł��ASetXXX/GetXXX ���邱�Ƃ��o����B
    (�Ō�ɐݒ��ۑ������Ƃ��̃v���O�C���̃o�[�W�����Ƃ�)

    GetXXX �Ŏ擾�����l���͈͓��Ɏ��܂��Ă��邱�Ƃ�������x�O��Ƃ��ėǂ����A
    �͈͊O�ł��s���I��/�\�����Ȃ��悤�ɍŒ���̔z���͕K�v�B

    EnumConfig �����ɒl�͈̔̓`�F�b�N��(�v���O�C�����g��)�s���ĕۑ����Ă͂���
    ���A�v���O�C���̐ݒ�ۑ��ꏊ�� %APPDATA% ���� INI �t�@�C���ɂ���A���[�U�[
    �����ڕҏW���邱�Ƃ��\�ł��邽�߁A�S�ʓI�ɐM������ׂ��ł͂Ȃ��B

    IKpiConfig �̊e�탁�\�b�h�͔C�ӂ̃X���b�h����Ăяo�����Ƃ��o���邪�A�ȉ���
    ������S�Ė������Ă���ꍇ�͎擾�������ʂ��������Ȃ��ꍇ������B

    1.�v���O�C�����񋟂��郁�\�b�h��{�̂��Ăяo�����łȂ�
    2.IKpiXXXModule ���񋟂���C���X�^���X���P�����݂��Ă��Ȃ�

    ����������ƃ��[�h�͂��Ă��邪�v���O�C����{�̂��g�p���Ă��Ȃ���Ԃł���B

    ���̏󋵂Ńv���O�C���� GetXXX ���Ăяo�����Ƃ͒ʏ�͂Ȃ��������A
    IKpiXXXModule �������ō쐬�����X���b�h�����Ăяo���������蓾��B
    (�o�b�N�O���E���h�Őݒ�����I�ɓǂݎ�铙)
    �ݒ�̓K�p��(�K�p���O)�� IKpiXXXModule::ApplyConfig ���Ă΂�A�K�p���
    IKpiXXX::UpdateConfig ���Ă΂�邽�߁A����I�ɐݒ���擾���ɂ����K�v��
    �Ȃ����ł���B

    IKpiConfig �̓v���O�C�������삵�Ă���S�Ẵv���Z�X�ŋ��L�����B
    kbrunkpi �o�R�œ��삵�Ă���ꍇ�AIKpiConfig ��p�ɂɌĂяo����
    �p�t�H�[�}���X���ቺ���邽�߁A�Ăяo���̕p�x�͋ɗ͗}���邱�ƁB

    IKpiConfig �̌Ăяo���̃^�C�~���O�Ƃ��Đ��������̂́A

    �EIKpiXXXModule �̊e�탁�\�b�h��(GetModuleInfo/Open ��)
    �EIKpiXXX::UpdateConfig() ��
    �EIKpiXXX �̊e�탁�\�b�h�̂����A�Ăяo���̕p�x�������Ȃ�����
      (IKpiDecoder::Select/Seek �͖��Ȃ�)
      (IKpiDecoder::Render �͂���قǖ��Ȃ��� UpdateConfig �̕����]�܂���)
      (IKpiPlayer::GetPosition/GetStatus �͔񐄏�)
*/
};
///////////////////////////////////////////////////////////////////////////////
class IKpiUnkProvider : public IKpiUnknown
{//�v���O�C���̎g�p��(�ʏ�� KbMedia Player �{��)���v���O�C���ɑ΂���
 //IKpiConfig �� IKpiDecoder ���̃C���X�^���X���
 //�����t QueryInterface �̂悤�Ȃ���
 //�{�̂������A�v���O�C�����͎g�p����̂�
public:
    virtual DWORD WINAPI CreateInstance(REFIID riid,    //�擾�������C���^�[�t�F�[�X�� IID
                                        void *pvParam1, //����1(riid �ɂ���ĕω�)
                                        void *pvParam2, //����2(riid �ɂ���ĕω�)
                                        void *pvParam3, //����3(riid �ɂ���ĕω�)
                                        void *pvParam4, //����4(riid �ɂ���ĕω�)
                                        void **ppvObj)=0;
/*
    kpi_CreateInstance �̑�3�����ɓn����� pUnknown �� QueryInterface �����
    ������
    �ݒ�̊Ǘ���{�̂ɔC����ꍇ��A�v���O�C������ʂ̃v���O�C�����g�p����ꍇ
    ���A�{�̂��񋟂���@�\���v���O�C��������g�p�������ꍇ�ɕK�v

    DWORD WINAPI CreateInstance(REFIID riid,    //�擾�������C���^�[�t�F�[�X�� IID
                                void *pvParam1, //����1(riid �ɂ���ĕω�)
                                void *pvParam2, //����2(riid �ɂ���ĕω�)
                                void *pvParam3, //����3(riid �ɂ���ĕω�)
                                void *pvParam4, //����4(riid �ɂ���ĕω�)
                                void **ppvObj);
      �{�̂��� IKpiConfig/IKpiDecoder �����擾����B
      �ʏ�� kpi_CreateConfig/kpi_CreateDecoder ���̃w���p�[�֐����ŌĂяo�����B
      ���ڂ͎g�p���Ȃ��B
      �����t�� QueryInterface �̂悤�Ȃ��̂����A�߂�l 0 �͐������Ӗ�����Ƃ�
      ����Ȃ��B(HRESULT �ł͂Ȃ��̂�)
      �ʏ�� *ppvObj �� NULL ���� NULL ���Ŕ��f���邱�ƁB

      �߂�l�ƈ����̈Ӗ��� riid �ɂ���ĈقȂ�B
      riid �����m�̏ꍇ�͉������Ȃ��� 0 ��Ԃ��B

      kpi_CreateConfig �� pdwPlatForm(pvParam2)�̂悤�ɁA�l�̑�������҂����
      �|�C���^��n���ꍇ�A�n���ꂽ riid �� IKpiUnkProvider �����m���ƒl���ύX
      ����Ȃ��B(���m�̏ꍇ�ł� *ppvObj �ɂ� NULL ����������)
      ���̂��߁A�Ăяo���O�ɉ��炩�̒l�������Ă������ƁB

    �ȉ��̃w���p�[�֐����p�ӂ���Ă���(�g�p����ꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ����邱��)

    DWORD WINAPI kpi_CreateConfig(IKpiUnkProvider *pProvider,
                                  const GUID *pGUID, //�ݒ�̕ۑ��ꏊ�����߂鎯�ʎq
                                  DWORD *pdwPlatform,//�{�̂����ڃv���O�C�����Ăяo���Ă��邩
                                  IKpiConfig **ppConfig);
      IKpiConfig �ɑΉ����A�ݒ��ʂ̎���������ۑ��̊Ǘ���{�̂ɔC�������ꍇ�ɌĂяo���B
      ��1������ IKpiUnknown ��(kpi_CreateInstance �̑�3���������̂܂ܓn��)
      ���񋟂����B

      IKpiConfig �ɑΉ�����ꍇ�Akpi_CreateConfig �ɂ�� IKpiConfig �̃C���X�^���X
      �擾�́A�x���Ƃ� kpi_CreateInstance ���Ŏ��s���邱�ƁB
      kpi_CreateInstance ����߂����i�K�Ŗ��� kpi_CreateConfig ���Ă΂�Ă��Ȃ�
      �ꍇ�A�ݒ�̕ۑ���{�̂ɊǗ������邱�Ƃ͏o���Ȃ��B
      (EnumConfig/ApplyConfig �͗L�������o�C�i���ȊO�̌^�͈����Ȃ�)

      riid ��F�������� 0 �ȊO��Ԃ����A0 �ȊO��Ԃ�������Ƃ����� ppConfig
      �̎擾�ɐ��������Ƃ͌���Ȃ��BppConfig �̎擾�ɐ����������� *ppConfig ��
      NULL ���ǂ����Ŕ��f���邱�ƁB

      pGUID: �ݒ�̕ۑ��ꏊ�����߂鎯�ʎq

      �n���ꂽ GUID �Ɋ�Â��Đݒ�̕ۑ��ꏊ�����肳���B
      �v���O�C���̒u���ꏊ�Ɉˑ����Ȃ����߁A�v���O�C���̃v���b�g�t�H�[����
      �o�[�W�����A�t�@�C�����Ɋւ�炸�ݒ肪���L�����B

      pGUID �� NULL (�܂��� GUID_NULL)��n���ƁA�ݒ�̕ۑ��ꏊ�͊�t�H���_
      ���猩���v���O�C���̑��΃p�X�ɂ���Č��肳���B
      ��t�H���_�́A���݂̎����ł͖{�̎��s�t�@�C��(Kbmplay.exe) �̐e�t�H���_
      (readme.txt �����u���Ă���t�H���_)

      pdwPlatform: �{�̂��v���O�C���𒼐ڌĂяo���Ă��邩�{�̂Ƃ͕ʃv���Z�X��

      �v���O�C�������[�h���Ă�����s�t�@�C�����{�̎��g(Kbmplay.exe)�̏ꍇ�� 0
      ������A�{�̂Ƃ͕ʃv���Z�X(kbrunkpi.exe)�o�R�̏ꍇ�� 32 �� 64 ������B
      �ʃv���Z�X�̏ꍇ�A�{�̂̃v���b�g�t�H�[���� x64 �̂Ƃ��� 64�Ax86 �̂Ƃ���
      32 ������Bkbrunkpi.exe �� x86 �ł��{�̂� x86/x64 �̂ǂ���Ȃ̂���
      ���ʕs�\�Ȃ��߁A�ǂ���Ȃ̂����K�v�ȏꍇ�� pdwPlatform �Ŕ��f����B
      *pdwPlatform �� 0 ���������ꍇ�A�{�̂� x86 �Ȃ̂� x64 �Ȃ̂��̓v���O�C��
      ���g�Ɠ����Ȃ̂ŕ����锤�ł���B
      �{�̂� kpi_CreateConfig ��(IID_IKpiConfig �̒l�ύX���ɂ��)���m�̏ꍇ��
      *pdwPlatform �ɂ� (DWORD)-1 ������(���m�ɂ́ACreateInstance �Ăяo���O��
      -1 �����A�{�̂��������Ȃ��̂ł��̂܂܂Ƃ����`�ƂȂ�)

      ���炩�̎���� kbrunkpi.exe �o�R�̏ꍇ�Ƀv���O�C���̓����ݒ�����
      �������ꍇ�� pdwPlatform �̖߂�l�𗘗p����B
      pdwPlatform ���s�v�ȏꍇ�� NULL ��n���Ă��ǂ��B

      ppConfig: IKpiConfig �̃C���X�^���X���󂯎��|�C���^

      �ݒ荀�ڂ����ɂȂ��ApdwPlatform �������ړI�̏ꍇ�� NULL ��n���Ă��ǂ��B


      �ݒ�̋��L�ɂ���

        A: x86 �Ŗ{�̂� x86 �Ńv���O�C�����g�p
        B: x86 �Ŗ{�̂� x64 �Ńv���O�C�����g�p
        C: x64 �Ŗ{�̂� x86 �Ńv���O�C�����g�p
        D: x64 �Ŗ{�̂� x64 �Ńv���O�C�����g�p

      *pGUID != GUID_NULL : A �` D �̐ݒ�͑S�ċ��L�����
      *pGUID == GUID_NULL : A �� C�AB �� D �����L�����
      (or pGUID == NULL)   (�W���v���O�C���̃t�H���_�\���̏ꍇ)

      x86 �ł� x64 �ł̃v���O�C���Őݒ����ʂ���(A �� C�AB �� D �����L)�A
      �v���O�C���̒u���ꏊ���ς���Ă��ݒ肪�����p�����悤�ɂ������ꍇ�́A
      GUID != GUID_NULL �Ƃ������ x64 �ŗp�� x86 �ŗp�ƂňقȂ�Z�N�V�������A
      �L�[�����g�p����Ηǂ��B���邢�� kpi_CreateConfig �ɈقȂ� GUID ��n���B

      kbrunkpi �o�R�Ŏg�p���Ɩ{�̂����ڎg�p���ƂŐݒ����ʂ������ꍇ�����l�B

      kpi_CreateConfig �ɂ�� IKpiConfig �̎擾�� 1 �̃v���O�C���ɂ� 1 ��
      �����L���B2 ��ڈȍ~�� 1 ��ڂƈقȂ� GUID ��n���ĕ����̐ݒ���g�p����
      ���Ƃ͏o�����A�Q�ƃJ�E���^�������₵�� 1 ��ڂƓ����C���X�^���X��Ԃ��B

    DWORD WINAPI kpi_CreateDecoder(IKpiUnkProvider *pProvider,
                                   const KPI_MEDIAINFO *cpRequest,//�Đ����g�����̗v���l���܂܂ꂽ KPI_MEDIAINFO
                                   IKpiFile     *pFile,   //���y�f�[�^
                                   IKpiFolder   *pFolder, //���y�f�[�^������t�H���_
                                   IKpiDecoder **ppDecoder);
      �f�R�[�_�v���O�C�����ʂ̃f�R�[�_�v���O�C�����g�p�������ꍇ�ɌĂяo���B
      �����Ɩ߂�l�̈Ӗ��� IKpiDecoderModule::Open �Ɠ����B
      pFile/pFolder �� IKpiDecoderModule::Open �œn���ꂽ pFile/pFolder �����̂܂�
      �n���Ă��ǂ����AOpenFile/OpenFolder ���ē������̂�n���Ă��ǂ��B
      �v���O�C�����Ǝ��Ɏ������� pFile/pFolder ��n���Ă��ǂ��B

      ���ݖ�����
      kbrunkpi.exe �o�R�ł̎��������G�����Ė��������c
*/
};
//IKpiUnkProvider �̃w���p�[�֐�(�g�p����ꍇ�� kpi.cpp ���v���W�F�N�g�ɒǉ����邱��)
extern DWORD WINAPI kpi_CreateConfig(IKpiUnkProvider *pProvider,
                                     const GUID *pGUID,
                                     DWORD *pdwPlatform,
                                     IKpiConfig **ppConfig);
extern DWORD WINAPI kpi_CreateConfig(IKpiUnknown *pUnknown, //kpi_CreateInstance �̑�3�������璼�ڎ擾
                                     const GUID *pGUID,
                                     DWORD *pdwPlatform,
                                     IKpiConfig **ppConfig);
//�v���O�C�����G�N�X�|�[�g����֐�
typedef HRESULT (WINAPI *pfn_kpiCreateInstance)(REFIID riid,      //IKpiXXXModule �� IID
                                                void **ppvObject, //IKpiXXXModule �̖߂�l
                                                IKpiUnknown *pUnknown);
//HRESULT WINAPI kpi_CreateInstance(REFIID riid, void **ppvObject, IKpiUnknown *pUnknown);
//���v���O�C�������ۂɃG�N�X�|�[�g����֐�
/*
    riid: IID_IKpiDecoderModule (�f�R�[�_�v���O�C��)
          IID_IKpiUnArcModule   (�A�[�J�C�u�v���O�C��)
          IID_IKpiPlayerMoodule (���t�v���O�C��)
          IID_IKpiOutDeviceModule (�f�o�C�X�v���O�C��)
        �̂����ꂩ���n�����B
    �f�R�[�_�v���O�C���� riid �� IID_IKpiDecoderModule ��n���ꂽ��
      *ppvObject �� IKpiDecoderModule �̃C���X�^���X��n���� S_OK ��Ԃ��B
    �A�[�J�C�u�v���O�C���� riid �� IID_IKpiUnArcModule ��n���ꂽ��
      *ppvObject �� IKpiUnArcModule �̃C���X�^���X��n���� S_OK ��Ԃ��B
    ���t�v���O�C���� riid �� IID_IKpiPlayerModule ��n���ꂽ��
      *ppvObject �� IID_IKpiPlayerModule �̃C���X�^���X��n���� S_OK ��Ԃ��B
    �f�o�C�X�v���O�C���� riid �� IID_IKpiOutDeviceModule ��n���ꂽ��
      *ppvObject �� IID_IKpiOutDeviceModule �̃C���X�^���X��n���� S_OK ��Ԃ��B
    �Ή����Ȃ� riid ���n���ꂽ�ꍇ�� *ppvObject �� NULL ��n���� E_NOINTERFACE ��Ԃ��B

    IKpiConfig �ɑΉ����A�ݒ��ʂ̎��������Ɛݒ�̕ۑ��Ǘ���{�̂ɔC�������ꍇ��
    kpi_CreateConfig �w���p�[�֐����g�p����B���̏ꍇ�� kpi.cpp ���v���W�F�N�g��
    �ǉ����邱�ƁB

    IKpiConfig �ɑΉ�����ꍇ�Akpi_CreateInstance ����߂�܂ł� kpi_CreateConfig ��
    �Ăяo�����ƁB���̒i�K�� kpi_CreateConfig ���Ă΂�Ă��Ȃ��ꍇ�ł�
    dwSupportConfig != 0 �ł���� EnumConfig �Őݒ��񋓂��邱�Ƃ��o���邪�A
    �o�C�i���ȊO�̌^�̐ݒ�͈������Ƃ��o���Ȃ��B

    �v���O�C������ʂ̃v���O�C�����g�p����ꍇ kpi_CreateDecoder ���Ăяo���B
    �v���O�C�����ʂ̃v���O�C�����g�p����ꍇ�� kpi_CreateDecoder ����Ȃ���΂Ȃ�Ȃ��B

    pUnknown �ɒ��� IKpiUnkProvider ��n������U QueryInterface �����悤��
    �Ȃ��Ă���̂́A�����̋@�\�̒ǉ���d�l�ύX�ɑΉ����邽�߁B

    kpi_CreateInstance �͖{�̂ɂ���Ĉ�x�����Ă΂��B���ۂ� riid ��ς��ĉ��x
    ���Ăяo�����A*ppvObject �ɃC���X�^���X���ŏ��ɕԂ��Ĉȍ~�͌Ă΂Ȃ��B

    2��ڈȍ~�̌Ăяo���̌��ʂ͖���`�B(2�ڈȍ~�� *ppModule ���g�p�������ʂ͖���`)

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
//extern HRESULT WINAPI kpi_CreateDecoderModule(pfn_kpiCreateInstance fnCreateInstance,
//                                              IKpiDecoderModule **ppModule, IKpiUnknown *pUnknown);
//���v���O�C���g�p���p�̃w���p�[�֐�(�v���O�C���쐬�ɂ͕s�v)
//�v���O�C���g�p��(�ʏ�� KbMedia Player)���Ăяo���ꍇ�� kpi.cpp ��
//�v���W�F�N�g�ɒǉ�����
///////////////////////////////////////////////////////////////////////////////
#pragma pack(pop) //�\���̂̃A���C�������g

#endif

