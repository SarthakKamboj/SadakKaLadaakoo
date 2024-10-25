REM https://learn.microsoft.com/en-us/answers/questions/1341012/how-to-compile-a-code-for-windows-gui-application
REM https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-creation
cl /EHsc /W4 src\main.cpp /link /SUBSYSTEM:WINDOWS user32.lib kernel32.lib
