// �ȉ��� ifdef �u���b�N�� DLL ����̃G�N�X�|�[�g��e�Ղɂ���}�N�����쐬���邽�߂�
// ��ʓI�ȕ��@�ł��B���� DLL ���̂��ׂẴt�@�C���́A�R�}���h ���C���Œ�`���ꂽ KPIADX_EXPORTS
// �V���{���ŃR���p�C������܂��B���̃V���{���́A���� DLL ���g���v���W�F�N�g�Œ�`���邱�Ƃ͂ł��܂���B
// �\�[�X�t�@�C�������̃t�@�C�����܂�ł���ق��̃v���W�F�N�g�́A
// KPIADX_API �֐��� DLL ����C���|�[�g���ꂽ�ƌ��Ȃ��̂ɑ΂��A���� DLL �́A���̃}�N���Œ�`���ꂽ
// �V���{�����G�N�X�|�[�g���ꂽ�ƌ��Ȃ��܂��B
#ifdef KPIADX_EXPORTS
#define KPIADX_API __declspec(dllexport)
#else
#define KPIADX_API __declspec(dllimport)
#endif

#define KPIADX_VERSION   ((2 << 8) | 0)      // 2.0

#ifdef _DEBUG
#define KPIADX_DESC      L"CRI ADX decoder plugin for KbMedia Player [DEBUG]"
#else
#define KPIADX_DESC      L"CRI ADX decoder plugin for KbMedia Player"
#endif
#define KPIADX_COPYRIGHT L"Copyright (c) 2006, 2021, Autch."
