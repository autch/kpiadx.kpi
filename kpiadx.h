// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された KPIADX_EXPORTS
// シンボルでコンパイルされます。このシンボルは、この DLL を使うプロジェクトで定義することはできません。
// ソースファイルがこのファイルを含んでいるほかのプロジェクトは、
// KPIADX_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef KPIADX_EXPORTS
#define KPIADX_API __declspec(dllexport)
#else
#define KPIADX_API __declspec(dllimport)
#endif
