@echo off

:: 生成sln
cmake --preset=win-x64-debug
:: 构建
cmake --build --preset=win-x64-debug
