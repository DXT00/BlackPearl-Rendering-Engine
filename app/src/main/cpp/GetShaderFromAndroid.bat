@echo off
cd /d "%~dp0

@REM call vendor\bin\premake\premake5.exe vs2022 --RenderAPI=opengl



@REM :: 检查管理员权限
@REM net session >nul 2>&1
@REM if %errorlevel% neq 0 (
@REM     echo Please right-click on this script and select "Run as Administrator"
@REM     pause
@REM     exit /b
@REM )


@REM :: 设置源目录和目标目录
@REM set SOURCE=%~dp0assets
@REM set DEST1=%~dp0SandBox\assets
@REM set DEST2=%~dp0..\assets

@REM :: 显示路径信息
@REM echo src: %SOURCE%
@REM echo dst1: %DEST1%
@REM echo dst2: %DEST2%

@REM :: 创建目标目录（如果不存在）
@REM if not exist "%DEST1%" mkdir "%DEST1%"
@REM if not exist "%DEST2%" mkdir "%DEST2%"

@REM :: 使用robocopy复制并排除.vshistory目录， 复制到SandBox 运行时资源路径
@REM robocopy "%SOURCE%" "%DEST1%" /E /XD ".vshistory" /XF "premake5.lua" /XF "CMakeLists.txt" /COPYALL /R:1 /W:1 /NP /LOG:%~dp0copy_log1.txt
@REM :: 检查结果
@REM if %ERRORLEVEL% LSS 8 (
@REM     echo copy to SandBox assets success!
@REM ) else (
@REM     echo copy to SandBox assets fail!
@REM )

@REM :: 使用robocopy复制并排除.vshistory目录, 复制到Android 运行时资源路径
@REM robocopy "%SOURCE%" "%DEST2%" /E /XD ".vshistory" /XF "premake5.lua" /COPYALL /R:1 /W:1 /NP /LOG:%~dp0copy_log2.txt

@REM :: 检查结果
@REM if %ERRORLEVEL% LSS 8 (
@REM     echo copy to SandBoxAndroid assets success!
@REM ) else (
@REM     echo copy to SandBoxAndroid  assets fail!
@REM )

@echo off
cd /d "%~dp0

adb pull /storage/emulated/0/Android/data/com.example.blackpearl/files %~dp0/android_shaders


:: 显示日志文件
::type copy_log.txt
pause
PAUSE