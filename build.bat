@echo off

:: 生成sln
::cmake --preset <configurePreset-name>
cmake --preset=win-x64-debug

:: 构建
::cmake --build --preset <buildPreset-name> 
cmake --build --preset=win-x64-debug
