/** @file

  Unified linker script for GCC and CLANG based builds

  Copyright (c) 2010 - 2021, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2015, Linaro Ltd. All rights reserved.<BR>
  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

PHDRS {
  text      PT_LOAD     FLAGS(5);  /* R_X */
  data      PT_LOAD     FLAGS(6);  /* RW_ */
  dynamic   PT_DYNAMIC  FLAGS(4);  /* R__ */
}

SECTIONS {

  /*
   * The PE/COFF binary consists of DOS and PE/COFF headers, and a sequence of
   * section headers adding up to PECOFF_HEADER_SIZE bytes (which differs
   * between 32-bit and 64-bit builds). The actual start of the .text section
   * will be rounded up based on its actual alignment.
   */
  . = PECOFF_HEADER_SIZE;

  .text : ALIGN(CONSTANT(COMMONPAGESIZE)) {
    KEEP(*(.entry))
    *(.text .text.* .stub .gnu.linkonce.t.*)
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.got .got.*)

    /*
     * The contents of AutoGen.c files are mostly constant from the POV of the
     * program, but most of it ends up in .data or .bss by default since few of
     * the variable definitions that get emitted are declared as CONST.
     * Unfortunately, we cannot pull it into the .text section entirely, since
     * patchable PCDs are also emitted here, but we can at least move all of the
     * emitted GUIDs here.
     */
    *:AutoGen.obj(.data.g*Guid)
  } :text

  /*
   * The alignment of the .data section should be less than or equal to the
   * alignment of the .text section. This ensures that the relative offset
   * between these sections is the same in the ELF and the PE/COFF versions of
   * this binary.
   */
  .data ALIGN(ALIGNOF(.text)) : ALIGN(CONSTANT(COMMONPAGESIZE)) {
    *(.data .data.* .gnu.linkonce.d.*)
    *(.bss .bss.*)
    KEEP(*(.prmexportdescriptor))
  } :data

  .eh_frame ALIGN(CONSTANT(COMMONPAGESIZE)) : {
    KEEP (*(.eh_frame))
  }

  .dynamic : { *(.dynamic) } :data :dynamic

  .rela (INFO) : {
    *(.rela .rela.*)
  }

  .hii : ALIGN(CONSTANT(COMMONPAGESIZE)) {
    KEEP (*(.hii))
  } :data

  .got : {
    *(.got)
  }
  ASSERT(SIZEOF(.got) == 0, "Unexpected GOT entries detected!")

  .got.plt (INFO) : {
    *(.got.plt)
  }
  ASSERT(SIZEOF(.got.plt) == 0 || SIZEOF(.got.plt) == 0xc || SIZEOF(.got.plt) == 0x18, "Unexpected GOT/PLT entries detected!")

  /*
   * Retain the GNU build id but in a non-allocatable section so GenFw
   * does not copy it into the PE/COFF image.
   */
  .build-id (INFO) : { *(.note.gnu.build-id) }

  /*
   * Retain the GCC static / global constructor / initializer and destructors / finalizer
   */
  .init_array : {
    PROVIDE_HIDDEN(__init_array_start = .);

    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP(*(.init_array))

    PROVIDE_HIDDEN(__init_array_end = .);

    PROVIDE_HIDDEN(__libc_init_array_start = .);

    KEEP(*(SORT_BY_NAME(.__libc_init.*)))

    PROVIDE_HIDDEN(__libc_init_array_end = .);
  }

  .fini_array : {
    PROVIDE_HIDDEN(__fini_array_start = .);

    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
    KEEP(*(.fini_array))

    PROVIDE_HIDDEN(__fini_array_end = .);

    PROVIDE_HIDDEN(__libc_fini_array_start = .);

    KEEP(*(SORT_BY_NAME(.__libc_fini.*)))

    PROVIDE_HIDDEN(__libc_fini_array_end = .);
  }

  /*
   * Error on unsupported (or not yet implemented) sections for construction / destruction
   */
  .preinit_array : {
    *(.preinit_array)
  }
  ASSERT(SIZEOF(.preinit_array) == 0, "error: .preinit_array is not supported (yet)")

  .init : {
    *(.init)
  }
  ASSERT(SIZEOF(.init) == 0, "error: .init is deprecated (System V uses it, and GCC uses other mechanisms)")

  .fini : {
    *(.fini)
  }
  ASSERT(SIZEOF(.fini) == 0, "error: .fini is deprecated (System V uses it, and GCC uses other mechanisms)")

  .ctors : {
    *(.ctors)
  }
  ASSERT(SIZEOF(.ctors) == 0, "error: .ctors is not implemented yet (C++ uses that)")

  .dtors : {
    *(.dtors)
  }
  ASSERT(SIZEOF(.dtors) == 0, "error: .dtors is not implemented yet (C++ uses that)")

  /DISCARD/ : {
    *(.note.GNU-stack)
    *(.gnu_debuglink)
    *(.interp)
    *(.dynsym)
    *(.dynstr)
    *(.hash .gnu.hash)
    *(.comment)
  }
}
