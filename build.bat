@echo off

:: 生成sln
:: 配置
::cmake --preset <configurePreset-name>
cmake --preset=win-x64-debug

:: 构建
::cmake --build --preset <buildPreset-name> 
cmake --build --preset=win-x64-debug

:: 清理构件 .exe .lib .dll 等
:: cmake --build --preset=win-x64-debug -t clean
