call vendor\bin\premake\premake5.exe vs2022 --RenderAPI=opengl



:: 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Please right-click on this script and select "Run as Administrator"
    pause
    exit /b
)


:: 设置源目录和目标目录
set SOURCE=%~dp0assets
set DEST1=%~dp0SandBox\assets
set DEST2=%~dp0..\assets

:: 显示路径信息
echo src: %SOURCE%
echo dst1: %DEST1%
echo dst2: %DEST2%

:: 创建目标目录（如果不存在）
if not exist "%DEST1%" mkdir "%DEST1%"
if not exist "%DEST2%" mkdir "%DEST2%"

:: 使用robocopy复制并排除.vshistory目录， 复制到SandBox 运行时资源路径
robocopy "%SOURCE%" "%DEST1%" /E /XD ".vshistory" /XF "premake5.lua" /COPYALL /R:1 /W:1 /NP /LOG:%~dp0copy_log1.txt
:: 检查结果
if %ERRORLEVEL% LSS 8 (
    echo copy to SandBox assets success!
) else (
    echo copy to SandBox assets fail!
)

:: 使用robocopy复制并排除.vshistory目录, 复制到Android 运行时资源路径
robocopy "%SOURCE%" "%DEST2%" /E /XD ".vshistory" /XF "premake5.lua" /COPYALL /R:1 /W:1 /NP /LOG:%~dp0copy_log2.txt

:: 检查结果
if %ERRORLEVEL% LSS 8 (
    echo copy to SandBoxAndroid assets success!
) else (
    echo copy to SandBoxAndroid  assets fail!
)

:: 显示日志文件
::type copy_log.txt
pause
PAUSE