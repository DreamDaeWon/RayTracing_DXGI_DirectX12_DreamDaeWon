#include <windows.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <cstdio>
#include <string>
int wmain(int argc, wchar_t** argv) {
    if (argc != 5) { fprintf(stderr, "Usage: ShaderCompiler input output entry profile\n"); return 1; }
    char entry[128]{}, profile[128]{};
    if (!WideCharToMultiByte(CP_UTF8, 0, argv[3], -1, entry, sizeof(entry), nullptr, nullptr) ||
        !WideCharToMultiByte(CP_UTF8, 0, argv[4], -1, profile, sizeof(profile), nullptr, nullptr)) return 1;
    Microsoft::WRL::ComPtr<ID3DBlob> code, errors;
    HRESULT hr = D3DCompileFromFile(argv[1], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, profile, D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &code, &errors);
    if (errors) fwrite(errors->GetBufferPointer(), 1, errors->GetBufferSize(), stderr);
    if (FAILED(hr)) return 1;
    hr = D3DWriteBlobToFile(code.Get(), argv[2], TRUE);
    if (FAILED(hr)) { fprintf(stderr, "Cannot write shader blob: %08lX\n", static_cast<unsigned long>(hr)); return 1; }
    return 0;
}
