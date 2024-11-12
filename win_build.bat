REM https://learn.microsoft.com/en-us/answers/questions/1341012/how-to-compile-a-code-for-windows-gui-application
REM https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-creation
del **.obj
del main.exe
cl /EHsc /W4 src\main.cpp src\gfx_api\windows\skl_d3d12.cpp src\gfx_api\windows\skl_win32.cpp /I external/DirectX-Headers/include/directx /I src /D SKL_WINDOWS /link /SUBSYSTEM:WINDOWS user32.lib kernel32.lib D3d12.lib dxgi.lib 	D3DCompiler.lib
