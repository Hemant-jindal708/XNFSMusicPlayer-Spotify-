@echo off
echo Compiling project...

:: The compile command for both files
cl /EHsc /std:c++17 main.cpp play.cpp /I"C:\Users\jinda\vcpkg\installed\x86-windows\include" /link /LIBPATH:"C:\Users\jinda\vcpkg\installed\x86-windows\lib" cpr.lib libcurl.lib Ws2_32.lib Shell32.lib /OUT:main.exe

:: If the compilation is successful (Error Level 0), run the executable
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful! Launching mod...
    echo ========================================
    main.exe
) else (
    echo.
    echo Compilation failed. Check the errors above.
)