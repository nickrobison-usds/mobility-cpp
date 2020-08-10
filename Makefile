
extern/vcpkg/bootstrap-vcpkg.sh:
	git submodule update --init --recursive

extern/vcpkg/vcpkg: extern/vcpkg/bootstrap-vcpkg.sh
	extern/vcpkg/bootstrap-vcpkg.sh


.PHONY: system
system: extern/vcpkg/vcpkg
	extern/vcpkg/vcpkg install @cmake/vcpkg_x64-linux.txt