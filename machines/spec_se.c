/* spec_se.c: ZX Spectrum SE specific routines
   Copyright (c) 1999-2011 Fredrick Meunier, Philip Kendall, Darren Salt

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: philip-fuse@shadowmagic.org.uk

*/

#include <config.h>

#include <stdio.h>
#include <string.h>

#include <libspectrum.h>

#include "fuse.h"
#include "keyboard.h"
#include "machine.h"
#include "machines.h"
#include "machines_periph.h"
#include "memory.h"
#include "peripherals/dck.h"
#include "peripherals/scld.h"
#include "snapshot.h"
#include "sound.h"
#include "spec128.h"
#include "settings.h"
#include "spectrum.h"
#include "tc2068.h"
#include "ui/ui.h"

static int dock_exrom_reset( void );
static int spec_se_reset( void );
static int spec_se_memory_map( void );

int
spec_se_init( fuse_machine_info *machine )
{
  machine->machine = LIBSPECTRUM_MACHINE_SE;
  machine->id = "se";

  machine->reset = spec_se_reset;

  machine->timex = 1;
  machine->ram.port_from_ula = tc2048_port_from_ula;
  machine->ram.contend_delay = spectrum_contend_delay_65432100;
  machine->ram.contend_delay_no_mreq = spectrum_contend_delay_65432100;

  machine->unattached_port = spectrum_unattached_port_none;

  machine->shutdown = NULL;

  machine->memory_map = spec_se_memory_map;

  return 0;
}

static int
dock_exrom_reset( void )
{
  /* The dock is always active on the SE */
  dck_active = 1;

  return 0;
}

int
spec_se_reset( void )
{
  int error;
  size_t i;

  error = dock_exrom_reset(); if( error ) return error;

  error = machine_load_rom( 0, settings_current.rom_spec_se_0,
                            settings_default.rom_spec_se_0, 0x4000 );
  if( error ) return error;
  error = machine_load_rom( 1, settings_current.rom_spec_se_1,
                            settings_default.rom_spec_se_1, 0x4000 );
  if( error ) return error;

  periph_clear();
  machines_periph_128();
  
  /* SE style memory paging present */
  periph_set_present( PERIPH_TYPE_128_MEMORY, PERIPH_PRESENT_NEVER );
  periph_set_present( PERIPH_TYPE_SE_MEMORY, PERIPH_PRESENT_ALWAYS );

  /* ULA uses full decoding */
  periph_set_present( PERIPH_TYPE_ULA, PERIPH_PRESENT_NEVER );
  periph_set_present( PERIPH_TYPE_ULA_FULL_DECODE, PERIPH_PRESENT_ALWAYS );

  /* As does the AY chip */
  periph_set_present( PERIPH_TYPE_AY, PERIPH_PRESENT_NEVER );
  periph_set_present( PERIPH_TYPE_AY_FULL_DECODE, PERIPH_PRESENT_ALWAYS );

  /* Timex-style AY also present */
  periph_set_present( PERIPH_TYPE_AY_TIMEX, PERIPH_PRESENT_ALWAYS );

  /* SCLD always present */
  periph_set_present( PERIPH_TYPE_SCLD, PERIPH_PRESENT_ALWAYS );

  /* ZX Printer available */
  periph_set_present( PERIPH_TYPE_ZXPRINTER_FULL_DECODE, PERIPH_PRESENT_OPTIONAL );

  periph_update();

  /* Mark as present/writeable */
  for( i = 0; i < 34; ++i )
    memory_map_ram[i].writable = 1;

  for( i = 0; i < MEMORY_PAGES_IN_64K; i++ ) {

    timex_dock[i] = memory_map_ram[ i + 18 ];
    timex_dock[i].page_num = i;
    timex_dock[i].contended = 0;
    timex_dock[i].source = memory_source_dock;

    timex_exrom[i] = memory_map_ram[ i + 26 ];
    timex_exrom[i].page_num = i;
    timex_exrom[i].contended = 0;
    timex_exrom[i].source = memory_source_exrom;
  }

  /* The dock and exrom aren't cleared by the reset routine, so do
     so manually (only really necessary to keep snapshot sizes down) */
  for( i = 0; i < MEMORY_PAGES_IN_64K; i++ ) {
    memset( timex_dock[i].page,  0, MEMORY_PAGE_SIZE );
    memset( timex_exrom[i].page, 0, MEMORY_PAGE_SIZE );
  }

  /* RAM pages 1, 3, 5 and 7 contended */
#if 0
  for( i = 0; i < 8; i++ ) 
    memory_map_ram[ 2 * i ].contended =
      memory_map_ram[ 2 * i + 1 ].contended = i & 1;
#endif

  machine_current->ram.locked=0;
  machine_current->ram.last_byte = 0;

  machine_current->ram.current_page=0;
  machine_current->ram.current_rom=0;

  memory_current_screen = 5;
  memory_screen_mask = 0xdfff;

  scld_dec_write( 0x00ff, 0x80 );
  scld_dec_write( 0x00ff, 0x00 );
  scld_hsr_write( 0x00f4, 0x00 );

  tc2068_tc2048_common_display_setup();

  return 0;
}

static int
spec_se_memory_map( void )
{
  memory_page *exrom_dock;

  /* Spectrum SE memory paging is just a combination of the 128K
     0x7ffd and TimexDOCK/EXROM paging schemes with one exception */
  spec128_memory_map();
  scld_memory_map();

  /* Exceptions apply if an odd bank is paged in via 0x7ffd */
  if( machine_current->ram.current_page & 0x01 ) {

  /* If so, bits 2 and 3 of 0xf4 also control whether the DOCK/EXROM
     is paged in at 0xc000 and 0xe000 respectively */
    exrom_dock = 
      scld_last_dec.name.altmembank ? timex_exrom : timex_dock;

    if( scld_last_hsr & ( 1 << 2 ) )
      memory_map_read[6] = memory_map_write[6] = exrom_dock[6];

    if( scld_last_hsr & ( 1 << 3 ) )
      memory_map_read[7] = memory_map_write[7] = exrom_dock[7];
  }

  memory_romcs_map();

  return 0;
}
