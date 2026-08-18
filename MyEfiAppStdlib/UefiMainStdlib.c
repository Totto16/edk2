#include "/home/totto/Code/coder2k/oopetris_pr5/temp/edk2/OvmfPkg/Library/PlatformDebugLibIoPort/DebugLibDetect.h"

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>

#include <stdio.h>


const CHAR16* EFIAPI bool_string(bool value) {
    return value ? L"true" : L"false";
}

/***
  Demonstrates basic workings of the main() function by displaying a
  welcoming message.

  Note that the UEFI command line is composed of 16-bit UCS2 wide characters.
  The easiest way to access the command line parameters is to cast Argv as:
      wchar_t **wArgv = (wchar_t **)Argv;

  @param[in]  Argc    Number of argument tokens pointed to by Argv.
  @param[in]  Argv    Array of Argc pointers to command line tokens.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
int
main (
  IN int Argc,
  IN char **Argv
  )
{

    DEBUG((DEBUG_ERROR, "[error] HELLO WORLD.\n"));
    DEBUG((DEBUG_INFO, "[info] HELLO WORLD.\n"));
    DEBUG((DEBUG_VERBOSE, "[verbose] HELLO WORLD.\n"));
    DEBUG((DEBUG_WARN, "[warn] HELLO WORLD.\n"));


    bool plat_detected = PlatformDebugLibIoPortDetect();

    bool debug_print_enabled = DebugPrintEnabled();

    Print(L"Hello from UEFI!: plat_debug: %s debug: %s\r\n", bool_string(plat_detected),
          bool_string(debug_print_enabled));

    // this should happend by some constructor of the lib "UefiBootServicesTableLib"
    // gST = sysTable;
    // gBS = sysTable->BootServices;
    //gImageHandle = imgHandle;

    Print(L"st %p bs %p imgH: %p\r\n", gST, gBS, gImageHandle);
    ASSERT(gST != NULL);
    ASSERT(gBS != NULL);
    ASSERT(gImageHandle != NULL);


    fprintf(stderr, "stderr print\r\n");
    fflush(stderr);
    printf("stdout print: %d\r\n", 42);
    fflush(stdout);

    return 0;
}
