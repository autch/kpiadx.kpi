#pragma once
#ifndef kpi_implH
#define kpi_implH

#include "kpi.h"
///////////////////////////////////////////////////////////////////////////////
//IKpiXXX �� QueryInterface/AddRef/Release
//�y�ю������K�{�ł͂Ȃ����\�b�h�̃f�t�H���g�̓�������������N���X�Q
//�v���O�C���J�����A�ʓ|�Ȃ炱���̃N���X����h��������Ηǂ����A�K�{�ł͂Ȃ�
//
//�����ɂȂ����̂� IKpiUnknown �̔h���N���X����������ꍇ��
//
//class CKpiUnArc : public KbKpiUnknwonImpl<IKpiUnArc>
//{
//};
//�܂���
//typedef KbKpiUnknwonImpl<IKpiUnArc> CKpiUnArc;
//
//class CKpiPlayer : public KbKpiUnknwonImpl<IKpiPlayer>
//{
//};
//�܂���
//typedef KbKpiUnknwonImpl<IKpiPlayer> CKpiPlayer;
//
//�̂悤�ɂ���� QueryInterface �� AddRef/Release �����͎����I�Ɏ��������
//
//KbKpiFileImpl �ȊO�� typedef ���Ă��邾���Ȃ̂ŁA���̃��\�b�h�͑S�Ď���
//����K�v������B
class IKpiDecoder;      //kpi_decoder.h �Ɉړ������̂őO���錾���Ă���
class IKpiDecoderModule;//kpi_decoder.h �Ɉړ������̂őO���錾���Ă���
///////////////////////////////////////////////////////////////////////////////
template<class T> class KbKpiUnknownImpl : public T
{
protected:
    LONG m_cRef;
    KbKpiUnknownImpl(void):m_cRef(1){}
    virtual ~KbKpiUnknownImpl(void){}
public:
    //IUnknown �h��
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG   STDMETHODCALLTYPE AddRef(void);
    ULONG   STDMETHODCALLTYPE Release(void);
};
///////////////////////////////////////////////////////////////////////////////
template<class T>
HRESULT WINAPI KbKpiUnknownImpl<T>::QueryInterface(REFIID riid,
                                                   void **ppvObject)
{
    if (!ppvObject) {
        return E_INVALIDARG;
    }
    else if(IsEqualGUID(riid, __uuidof(T))){
        *ppvObject = (T*)this;
    }
    else if(IsEqualGUID(riid, __uuidof(IKpiUnknown))){
        *ppvObject = (IKpiUnknown*)this;
    }
    //else if(IsEqualGUID(riid, __uuidof(IUnknown))){
    //    *ppvObject = (IUnknown*)this;
    //}
    //��typedef IUnknown IKpiUnknown;
    //  IUnknown == IKpiUnknown �Ȃ̂ŕK�v�Ȃ�
    else{
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
template<class T>
ULONG WINAPI KbKpiUnknownImpl<T>::AddRef(void)
{
    return InterlockedIncrement(&m_cRef);
}
///////////////////////////////////////////////////////////////////////////////
template<class T>
ULONG WINAPI KbKpiUnknownImpl<T>::Release(void)
{
    ULONG ret = InterlockedDecrement(&m_cRef);
    if(ret == 0){
        delete this;
    }
    return ret;
}
///////////////////////////////////////////////////////////////////////////////
class KbKpiFileImpl : public KbKpiUnknownImpl<IKpiFile>
{//�f�R�[�_�v���O�C���͎g�p����̂�
 //�{��(�܂��̓v���O�C�����g�p����\�t�g)������
 //�A�[�J�C�u�v���O�C���ł��������K�v
public:
    //IKpiFile �h��
    DWORD WINAPI GetFileName(wchar_t *pszName, DWORD dwSize){pszName[0] = 0;return 0;}
    BOOL  WINAPI GetRealFileW(const wchar_t **ppszFileName){*ppszFileName=NULL;return FALSE;}
    BOOL  WINAPI GetRealFileA(const char    **ppszFileName){*ppszFileName=NULL;return FALSE;}
    BOOL  WINAPI GetBuffer(const BYTE **ppBuffer, size_t *pstSize){*ppBuffer=NULL;*pstSize=0;return FALSE;}
    BOOL  WINAPI CreateClone(IKpiFile **ppFile){*ppFile=NULL;return FALSE;}
    BOOL  WINAPI Abort(void){return FALSE;}
/*  �ȉ��͕K���������Ȃ���΂Ȃ�Ȃ�
    DWORD  WINAPI Read(void *pBuffer, DWORD dwSize);
    UINT64 WINAPI Seek(INT64 i64Pos, DWORD dwOrigin);
    UINT64 WINAPI GetSize(void);
*/
};
///////////////////////////////////////////////////////////////////////////////
typedef KbKpiUnknownImpl<IKpiFolder> KbKpiFolderImpl;
/*
class KbKpiFolderImpl : public KbKpiUnknownImpl<IKpiFolder>
{//�{��(�܂��̓v���O�C�����g�p����\�t�g)������
 //�f�R�[�_�v���O�C���A�A�[�J�C�u�v���O�C���͎g�p����݂̂Ŏ����s�v
public:
    IKpiFolder �h��
    �ȉ��͕K���������Ȃ���΂Ȃ�Ȃ�
    DWORD WINAPI GetFolderName(wchar_t *pszName, DWORD dwSize);
    DWORD WINAPI EnumFiles(DWORD dwIndex, wchar_t *pszName, DWORD dwSize, DWORD dwLevel);
    BOOL  WINAPI OpenFile(const wchar_t* cszName, IKpiFile **ppFile);
    BOOL  WINAPI OpenFolder(const wchar_t* cszName, IKpiFolder **ppFolder);
};
*/
///////////////////////////////////////////////////////////////////////////////
class KbKpiNullFolder : public KbKpiFolderImpl
{//�����I�ɉ������Ȃ� IKpiFolder
 //�{��(�܂��̓v���O�C�����g�p����\�t�g)�ŕK�v(����)
public:
    //IKpiFolder �h��
    DWORD WINAPI GetFolderName(wchar_t *pszName, DWORD dwSize){pszName[0] = 0;return 0;}
    DWORD WINAPI EnumFiles(DWORD dwIndex, wchar_t *pszName, DWORD dwSize, DWORD dwLevel){pszName[0] = 0; return 0;}
    BOOL  WINAPI OpenFile(const wchar_t* cszName, IKpiFile **ppFile){*ppFile=NULL;return FALSE;}
    BOOL  WINAPI OpenFolder(const wchar_t* cszName, IKpiFolder **ppFolder){*ppFolder=NULL;return FALSE;}
};
///////////////////////////////////////////////////////////////////////////////
typedef KbKpiUnknownImpl<IKpiDecoder> KbKpiDecoderImpl;
/*
class KbKpiDecoderImpl : public KbKpiUnknownImpl<IKpiDecoder>
{//�f�R�[�_�v���O�C���Ŏ������K�v
public:
    //IKpiDecoder �h��
    //�ȉ��͕K���������Ȃ���΂Ȃ�Ȃ�
    DWORD  WINAPI Select(DWORD dwNumber,                    //�Ȕԍ�
                         const KPI_MEDIAINFO **ppMediaInfo, //may be NULL
                         IKpiTagInfo *pTagInfo,             //may be NULL
                         DWORD dwTagGetFlags);//�ǂ̃t�B�[���h�̃^�O���擾���邩�̃t���O
    DWORD  WINAPI Render(BYTE *pBuffer, DWORD dwSizeSample);
    UINT64 WINAPI Seek(UINT64 qwPosSample, DWORD dwFlag);
    DWORD  WINAPI UpdateConfig(void *pvReserved){return 0;}
    //UpdateConfig �͑����̏ꍇ�����s�v�����A��Ŏ����������Ȃ�
    //��������Ȃ��̂Ŋ����Ă����ł͎������Ȃ�(�K�v�Ȃ�R�s�y)
};
*/
///////////////////////////////////////////////////////////////////////////////
typedef KbKpiUnknownImpl<IKpiDecoderModule> KbKpiDecoderModuleImpl;
/*
class KbKpiDecoderModuleImpl : public KbKpiUnknownImpl<IKpiDecoderModule>
{//�f�R�[�_�v���O�C���Ŏ������K�v
public:
    //IKpiDecoderModule �h��
    //�ȉ��͕K���������Ȃ���΂Ȃ�Ȃ�
    void  WINAPI GetModuleInfo(const KPI_DECODER_MODULEINFO **ppInfo);
    DWORD WINAPI Open(const KPI_MEDIAINFO *cpRequest,//�Đ����g�����̗v���l���܂܂ꂽ KPI_MEDIAINFO
                              IKpiFile     *pFile,   //���y�f�[�^
                              IKpiFolder   *pFolder, //���y�f�[�^������t�H���_
                              IKpiDecoder **ppDecoder);
    BOOL  WINAPI EnumConfig(IKpiConfigEnumerator *pEnumerator){return FALSE;}
    DWORD WINAPI ApplyConfig(const wchar_t *cszSection,//�Z�N�V������
                             const wchar_t *cszKey,    //�L�[��
                             INT64  nValue, //�ύX��̒l(BOOL/INT �̏ꍇ)
                             double dValue, //�ύX��̒l(FLOAT �̏ꍇ)
                             const wchar_t *cszValue){//�ύX��̒l(������)
        return KPI_CFGRET_OK;
    }
    //EnumConfig/ApplyConfig �͑����̏ꍇ�����s�v�����A��Ŏ����������Ȃ�
    //��������Ȃ��̂Ŋ����Ă����ł͎������Ȃ�(�K�v�Ȃ�R�s�y)
};
*/
///////////////////////////////////////////////////////////////////////////////
typedef KbKpiUnknownImpl<IKpiTagInfo> KbKpiTagInfoImpl;
/*
class KbKpiTagInfoImpl : public KbKpiUnknownImpl<IKpiTagInfo>
{//�{�̂�����
 //�f�R�[�_�v���O�C���͎g�p����̂݁B�����s�v
 //�A�[�J�C�u�v���O�C���͎g�p���������s�v
public:
    //IKpiTagInfo �h��
    //�ȉ��͕K���������Ȃ���΂Ȃ�Ȃ�
    void WINAPI aSetValueA(const char    *cszNameA,   int nNameSize,  //Name: Ansi
                           const char    *cszValueA,  int nValueSize);//Value:Ansi
    void WINAPI aSetValueW(const char    *cszNameA,   int nNameSize,  //Name: Ansi
                           const wchar_t *cszValueW,  int nValueSize);//Value: Utf16
    void WINAPI aSetValueU8(const char   *cszNameA,   int nNameSize,  //Name: Ansi
                            const char   *cszValueU8, int nValueSize);//Value: Utf8
    //Name=utf16 ��
    void WINAPI wSetValueA(const wchar_t  *cszNameW,   int nNameSize,  //Name: utf15
                           const char     *cszValueA,  int nValueSize);//Value; Ansi
    void WINAPI wSetValueW(const wchar_t  *cszNameW,   int nNameSize,  //Name: Utf16
                           const wchar_t  *cszValueW,  int nValueSize);//Value: Utf16
    void WINAPI wSetValueU8(const wchar_t *cszNameW,   int nNameSize,  //Name; utf16
                            const char    *cszValueU8, int nValueSize);//Utf8: utf8
    //Name=utf8 ��
    void WINAPI u8SetValueA(const char    *cszNameU8,  int nNameSize,  //Name: utf8
                            const char    *cszValueA,  int nValueSize);//Value: Ansi
    void WINAPI u8SetValueW(const char    *cszNameU8,  int nNameSize,  //Name; utf8
                            const wchar_t *cszValueW,  int nValueSize);//Value: Utf16
    void WINAPI u8SetValueU8(const char   *cszNameU8,  int nNameSize,  //Name: utf8
                             const char   *cszValueU8, int nValueSize);//Value: Utf8
};
*/
#endif

