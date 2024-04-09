@echo off

set "vcpkg_dir=./vcpkg"

IF NOT EXIST "%vcpkg_dir%" (
	git clone https://github.com/microsoft/vcpkg
	call vcpkg\bootstrap-vcpkg.bat
)

vcpkg\vcpkg.exe install glew:x64-windows
vcpkg\vcpkg.exe install sdl2:x64-windows
vcpkg\vcpkg.exe install glm:x64-windows
vcpkg\vcpkg.exe install imgui[core,sdl2-binding,opengl3-binding]:x64-windows