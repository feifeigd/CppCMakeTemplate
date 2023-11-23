@echo off

vcpkg install --triplet=x64-windows-static ^
    asio boost cppcoro gtest
