@echo off

vcpkg install --triplet=x64-windows-static ^
    boost cppcoro gtest
