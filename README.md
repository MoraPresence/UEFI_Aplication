UEFI APPLICATION(DXE-driver)

file to bin -> xxd -i <filename>

compile(edk2/UefiAppPkg/BUILD/RELEASE_GCC5/X64) -> build -n 2 -a X64 -t GCC5 -b RELEASE -p UefiAppPkg/UefiApp.dsc -m UefiAppPkg/EnhancedUefiAppDxe/UefiApp.inf 

start Qemu -> ./runQemu.sh
