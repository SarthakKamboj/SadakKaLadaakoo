REM https://learn.microsoft.com/en-us/answers/questions/1341012/how-to-compile-a-code-for-windows-gui-application
REM https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-creation
del **.obj
del **.exe
del **.pdb
del **.ilk
del **.idb

cl /EHsc /W4  ^
    src\win32_main.cpp src\platform\app.cpp  ^
    src\platform\win32\skl_d3d12.cpp src\platform\win32\skl_win32.cpp   ^
    /Fe:main.exe  ^
    /I external/DirectX-Headers/include/directx  ^
    /I src  ^
    /D SKL_WINDOWS  ^
    /link /SUBSYSTEM:WINDOWS user32.lib kernel32.lib D3d12.lib dxgi.lib D3DCompiler.lib
