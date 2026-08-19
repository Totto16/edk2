[Defines]
  PLATFORM_NAME           = MyEfiAppSdl2
  PLATFORM_GUID           = 8fd29a57-a693-4e11-bb17-dc46086af568
  PLATFORM_VERSION        = 1.0
  DSC_SPECIFICATION       = 0x0001001B
  OUTPUT_DIRECTORY        = Build/MyEfiAppSdl2
  SUPPORTED_ARCHITECTURES = X64
  BUILD_TARGETS           = DEBUG|RELEASE
  SKUID_IDENTIFIER        = DEFAULT

  # FLASH_DEFINITION               = EmulatorPkg/EmulatorPkg.fdf


  #
  # Network definition
  #
  DEFINE NETWORK_SNP_ENABLE       = FALSE
  DEFINE NETWORK_IP6_ENABLE       = FALSE
  DEFINE NETWORK_TLS_ENABLE       = FALSE
  DEFINE NETWORK_HTTP_BOOT_ENABLE = FALSE
  DEFINE NETWORK_HTTP_ENABLE      = FALSE
  DEFINE NETWORK_ISCSI_ENABLE     = FALSE
  DEFINE SECURE_BOOT_ENABLE       = FALSE

  #
  # Redfish definition
  #
  DEFINE REDFISH_ENABLE = FALSE

  DEFINE DEBUG_ON_SERIAL_PORT = FALSE
  DEFINE DEBUG_TO_MEM = FALSE

!ifndef OOPETRIS_RUNTIME_TARGET
  !error "OOPETRIS_RUNTIME_TARGET must be set"
!endif

!if $(OOPETRIS_RUNTIME_TARGET) == "hardware"
  DEFINE PLAT_QEMU               = FALSE
  DEFINE QEMU_PV_VARS            = FALSE
  DEFINE DEBUG_ON_SERIAL_PORT    = FALSE
!elseif $(OOPETRIS_RUNTIME_TARGET) == "emulator"
  DEFINE PLAT_QEMU               = TRUE
  DEFINE QEMU_PV_VARS            = FALSE
  DEFINE DEBUG_ON_SERIAL_PORT    = TRUE
!else
  !error "OOPETRIS_RUNTIME_TARGET has invalid value"
!endif


[Packages]
  MdePkg/MdePkg.dec

#!include MdePkg/MdeLibs.dsc.inc


[LibraryClasses]
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf

  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf


  RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
  StackCheckLib|MdePkg/Library/StackCheckLib/StackCheckLib.inf
  StackCheckFailureHookLib|MdePkg/Library/StackCheckFailureHookLibNull/StackCheckFailureHookLibNull.inf



!include StdLib/StdLib.inc


[LibraryClasses.common.UEFI_APPLICATION]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  #DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  #DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  #DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  #SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  MemDebugLogLib|OvmfPkg/Library/MemDebugLogLib/MemDebugLogLibNull.inf
  DebugLib|OvmfPkg/Library/PlatformDebugLibIoPort/PlatformDebugLibIoPort.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf

  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf

  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf


!if $(OOPETRIS_RUNTIME_TARGET) == "hardware"
  #TODO: doesn't work on qemu, but maybe on CPU??
  TimerLib|UefiCpuPkg/Library/CpuTimerLib/BaseCpuTimerLib.inf
  LibUEfiSupport|LibraryPkg/SupportLib/Library/TimerLib/SupportLibTimerImpl.inf
!else
  LibUEfiSupportNanosleep|LibraryPkg/SupportLib/Library/Default/SupportLibDefaultNanosleep.inf

  #LibUEfiSupportClock|LibraryPkg/SupportLib/Library/TimerLib/SupportLibTimerClock.inf
  LibUEfiSupportClock|LibraryPkg/SupportLib/Library/Default/SupportLibDefaultClock.inf
  #LibUEfiSupportClock|LibraryPkg/SupportLib/Library/Null/SupportLibNullClock.inf

  ## doesn't work on qemu, setup (cpuid leaf 0x15) error
  ## TimerLib|UefiCpuPkg/Library/CpuTimerLib/BaseCpuTimerLib.inf

  ## not supported for DXE or UEFI_APPLICATION:
  ## TimerLib|OvmfPkg/Library/AcpiTimerLib/BaseAcpiTimerLib.inf

  ## NOT WORKING need ovmf PEI to enable it? qemu at least reports wrong performancecounter
  ## TimerLib|OvmfPkg/Library/AcpiTimerLib/DxeAcpiTimerLib.inf

  ## not working: need special emulator?
  ## TimerLib|EmulatorPkg/Library/DxeTimerLib/DxeTimerLib.inf

!endif


[Components]
  MyEfiAppSdl2/MyEfiAppSdl2.inf

[PcdsFixedAtBuild]
#define DEBUG_WARN      0x00000002       // Warnings
#define DEBUG_INFO      0x00000040       // Informational debug messages
#define DEBUG_VERBOSE   0x00400000       // Detailed debug messages that may
#define DEBUG_ERROR     0x80000000       // Error
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80400042

# this is the same address as QEMU uses
  gUefiOvmfPkgTokenSpaceGuid.PcdDebugIoPort|0x402|UINT16|4

#define DEBUG_PROPERTY_DEBUG_ASSERT_ENABLED       0x01
#define DEBUG_PROPERTY_DEBUG_PRINT_ENABLED        0x02
#define DEBUG_PROPERTY_DEBUG_CODE_ENABLED         0x04
#define DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED       0x08
#define DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED  0x10
#define DEBUG_PROPERTY_ASSERT_DEADLOOP_ENABLED    0x20

  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x3F

  # disable auto initialize, initialize manually, and if it fails, use backup non shell code backup
  gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE


!if $(OOPETRIS_RUNTIME_TARGET) == "hardware"

!else

## Defines the ACPI register set base address.
  #  The invalid 0xFFFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Timer IO Port Address
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddress         |0x0400

  ## Defines the PCI Bus Number of the PCI device that contains the BAR and Enable for ACPI hardware registers.
  # @Prompt ACPI Hardware PCI Bus Number
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciBusNumber            |  0x00

  ## Defines the PCI Device Number of the PCI device that contains the BAR and Enable for ACPI hardware registers.
  #  The invalid 0xFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Device Number
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciDeviceNumber         |  0x1F

  ## Defines the PCI Function Number of the PCI device that contains the BAR and Enable for ACPI hardware registers.
  #  The invalid 0xFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Function Number
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciFunctionNumber       |  0x00

  ## Defines the PCI Register Offset of the PCI device that contains the Enable for ACPI hardware registers.
  #  The invalid 0xFFFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Register Offset
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciEnableRegisterOffset |0x0044

  ## Defines the bit mask that must be set to enable the APIC hardware register BAR.
  # @Prompt ACPI Hardware PCI Bar Enable BitMask
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoBarEnableMask           |  0x00

  ## Defines the PCI Register Offset of the PCI device that contains the BAR for ACPI hardware registers.
  #  The invalid 0xFFFF is as its default value. It must be configured to the real value.
  # @Prompt ACPI Hardware PCI Bar Register Offset
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPciBarRegisterOffset    |0x0040

  ## Defines the offset to the 32-bit Timer Value register that resides within the ACPI BAR.
  # @Prompt Offset to 32-bit Timer register in ACPI BAR
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiPm1TmrOffset              |0x0008

  ## Defines the bit mask to retrieve ACPI IO Port Base Address
  # @Prompt ACPI IO Port Base Address Mask
  gPcAtChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddressMask     |0xFFFE

  ## Reset Control Register address in I/O space.
  # @Prompt Reset Control Register address
  ##NOT USED: gPcAtChipsetPkgTokenSpaceGuid.PcdResetControlRegister|0x64|UINT64|0x00000019

  ## 8bit Reset Control Register value for cold reset.
  # @Prompt Reset Control Register value for cold reset
  ##NOT USED: gPcAtChipsetPkgTokenSpaceGuid.PcdResetControlValueColdReset|0xFE|UINT8|0x0000001A

  ## Specifies the initial value for Register_A in RTC.
  # @Prompt Initial value for Register_A in RTC.
  ##NOT USED: gPcAtChipsetPkgTokenSpaceGuid.PcdInitialValueRtcRegisterA|0x26|UINT8|0x0000001B

  ## Specifies the initial value for Register_B in RTC.
  # @Prompt Initial value for Register_B in RTC.
  ##NOT USED: gPcAtChipsetPkgTokenSpaceGuid.PcdInitialValueRtcRegisterB|0x02|UINT8|0x0000001C

  ## Specifies the initial value for Register_D in RTC.
  # @Prompt Initial value for Register_D in RTC.
  ##NOT USED: gPcAtChipsetPkgTokenSpaceGuid.PcdInitialValueRtcRegisterD|0x00|UINT8|0x0000001D

  ## RTC Update Timeout Value(microsecond).
  # @Prompt RTC Update Timeout Value.
  ##NOT USED: gPcAtChipsetPkgTokenSpaceGuid.PcdRealTimeClockUpdateTimeout|100000|UINT32|0x00000020


!endif

[Components]
  LibraryPkg/SDL2Pkg/SDL2Pkg.inf

!include LibraryPkg/SDL2Pkg/SDL2Pkg.inc

