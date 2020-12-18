#[Defines]
  #DEC_SPECIFICATION              = 0x00010005
  #PACKAGE_NAME                   = UefiAppPkg
  #PACKAGE_GUID                   = 4509cf3e-54da-49a9-879a-8e935a723e67
  #PACKAGE_VERSION                = 0.1
  #SUPPORTED_ARCHITECTURES        = X64
  #OUTPUT_DIRECTORY               = BUILD/UefiAppPkg
  #BUILD_TARGETS                  = DEBUG|RELEASE
  #SKUID_IDENTIFIER               = DEFAULT	  

[Defines]
  PLATFORM_NAME                  = UefiAppPkg
  PLATFORM_GUID                  = 419172a9-45a5-40af-b7b8-a7d0f7e8b3ad
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  SUPPORTED_ARCHITECTURES        = X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  OUTPUT_DIRECTORY               = UefiAppPkg/BUILD

#[BuildOptions]
  #GCC:RELEASE_*_*_CC_FLAGS             = -DMDEPKG_NDEBUG
  #INTEL:RELEASE_*_*_CC_FLAGS           = /D MDEPKG_NDEBUG
  #MSFT:RELEASE_*_*_CC_FLAGS            = /D MDEPKG_NDEBUG
  #RVCT:RELEASE_*_*_CC_FLAGS            = -DMDEPKG_NDEBUG
  #*_*_*_CC_FLAGS                       = -D DISABLE_NEW_DEPRECATED_INTERFACES

[LibraryClasses]

 UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf

  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf

[Components]
  UefiAppPkg/EnhancedUefiAppDxe/UefiApp.inf
