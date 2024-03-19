/* null_ui.c: Routines for dealing with the null user interface
   Copyright (c) 2017 Philip Kendall

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

#include "config.h"

#include "keyboard.h"
#include "ui/ui.h"

#include "../uijoystick.c"

#include "fuse.h"
#include "settings.h"
#include "timer/timer.h"

#include "ui_io.h"

#include <stdio.h>

#define TB_IMPL
#include "termbox2.h"

/* primary ZX colors in 216 6*6*6 RGB mode */
#define RGB_TO_216(r, g, b) (1 + 36 * (r) + 6 * (g) + (b))

#define ZX_DIM_BLACK RGB_TO_216(0, 0, 0)
#define ZX_DIM_BLUE RGB_TO_216(0, 0, 4)
#define ZX_DIM_RED RGB_TO_216(4, 0, 0)
#define ZX_DIM_MAGENTA RGB_TO_216(4, 0, 4)
#define ZX_DIM_GREEN RGB_TO_216(0, 4, 0)
#define ZX_DIM_CYAN RGB_TO_216(0, 4, 4)
#define ZX_DIM_YELLOW RGB_TO_216(4, 4, 0)
#define ZX_DIM_WHITE RGB_TO_216(4, 4, 4)

#define ZX_BR_BLACK RGB_TO_216(0, 0, 0)
#define ZX_BR_BLUE RGB_TO_216(0, 0, 5)
#define ZX_BR_RED RGB_TO_216(5, 0, 0)
#define ZX_BR_MAGENTA RGB_TO_216(5, 0, 5)
#define ZX_BR_GREEN RGB_TO_216(0, 5, 0)
#define ZX_BR_CYAN RGB_TO_216(0, 5, 5)
#define ZX_BR_YELLOW RGB_TO_216(5, 5, 0)
#define ZX_BR_WHITE RGB_TO_216(5, 5, 5)

static const libspectrum_word PALETTE[16] = {
    ZX_DIM_BLACK, ZX_DIM_BLUE, ZX_DIM_RED,    ZX_DIM_MAGENTA,
    ZX_DIM_GREEN, ZX_DIM_CYAN, ZX_DIM_YELLOW, ZX_DIM_WHITE,
    ZX_BR_BLACK,  ZX_BR_BLUE,  ZX_BR_RED,     ZX_BR_MAGENTA,
    ZX_BR_GREEN,  ZX_BR_CYAN,  ZX_BR_YELLOW,  ZX_BR_WHITE,
};

static int txt_output_enabled = 1;

// keysyms_map_t keysyms_map[] = {
//   { 0, 0 } /* End marker */
// };

// scaler_type
// menu_get_scaler( scaler_available_fn selector )
// {
//   /* No scaler selected */
//   return SCALER_NUM;
// }

// int
// menu_select_roms_with_title( const char *title, size_t start, size_t count,
//     int is_peripheral )
// {
//   /* No error */
//   return 0;
// }

// void
// ui_breakpoints_updated( void )
// {
//   /* Do nothing */
// }

// ui_confirm_save_t
// ui_confirm_save_specific( const char *message )
// {
//   return UI_CONFIRM_SAVE_DONTSAVE;
// }

// ui_confirm_joystick_t
// ui_confirm_joystick( libspectrum_joystick libspectrum_type, int inputs )
// {
//   return UI_CONFIRM_JOYSTICK_NONE;
// }

// int
// ui_debugger_activate( void )
// {
//   /* No error */
//   return 0;
// }

// int
// ui_debugger_deactivate( int interruptable )
// {
//   /* No error */
//   return 0;
// }

// int
// ui_debugger_disassemble( libspectrum_word addr )
// {
//   /* No error */
//   return 0;
// }

// int
// ui_debugger_update( void )
// {
//   /* No error */
//   return 0;
// }

static libspectrum_word fuzx_image[DISPLAY_SCREEN_HEIGHT][DISPLAY_SCREEN_WIDTH];

static int pressed = 0;
static int log_message_y = 0;

void log_message_reset() {
    log_message_y = txt_output_enabled ? (240 / 2 + 1) : 0;
}

void log_message(const char *fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    if (txt_output_enabled) {
        tb_printf_inner(0, log_message_y++, ZX_BR_WHITE, ZX_BR_BLACK, NULL, fmt, vl);
        tb_present();
    } else {
        vprintf(fmt, vl);
    }
    va_end(vl);
}

int ui_init(int *argc, char ***argv) {
    if( ui_widget_init() ) return 1;
    // if (txt_output_enabled) {
        tb_init();
        tb_set_output_mode(TB_OUTPUT_216);
        log_message_reset();
    // }
    display_ui_initialised = 1;

    /* No error */
    return 0;
}

int ui_end(void) {
    ui_widget_end();
    // if (txt_output_enabled) {
        tb_shutdown();
    // }
    /* No error */
    return 0;
}

// int
// ui_error_specific( ui_error_level severity, const char *message )
// {
//   /* No error */
//   return 0;
// }

int ui_event(void) {
    static struct input_event_t fuse_event;
    struct tb_event ev;

    if (pressed) {
        // log_message("\n\r");
        // log_message("input_event release: %d\n\r", fuse_event.types.key.native_key);
        fuse_event.type = INPUT_EVENT_KEYRELEASE;
        input_event(&fuse_event);
        pressed = 0;
    }

    tb_peek_event(&ev, 1);
    while (ev.type == TB_EVENT_KEY) {
        // log_message("\n\r");
        // log_message_reset();
        // log_message("key: %d ch: %d\n\r", ev.key, ev.ch);
        input_key fuse_keysym;
        fuse_keysym = keysyms_remap(ev.ch | ev.key);
        if (fuse_keysym != INPUT_KEY_NONE) {
            // log_message("sym key: %d -> %d\n\r", ev.ch, fuse_keysym);
            fuse_event.type = INPUT_EVENT_KEYPRESS;
            fuse_event.types.key.native_key = fuse_keysym;
            fuse_event.types.key.spectrum_key = fuse_keysym;
            // log_message("input_event pressed: %d\n\r", fuse_event.types.key.native_key);
            input_event(&fuse_event);
            pressed = fuse_keysym;
        // } else {
        //     log_message("fuse_keysym = INPUT_KEY_NONE\n\r");
        }
        break;
    }

    return 0;
}

// char*
// ui_get_open_filename( const char *title )
// {
//   /* No filename */
//   return NULL;
// }

// int
// ui_get_rollback_point( GSList *points )
// {
//   /* No rollback point */
//   return -1;
// }

// char*
// ui_get_save_filename( const char *title )
// {
//   /* No filename */
//   return NULL;
// }

// int
// ui_menu_item_set_active( const char *path, int active )
// {
//   /* No error */
//   return 0;
// }

int ui_mouse_grab(int startup) {
    /* Successful grab */
    return 1;
}

int ui_mouse_release(int suspend) {
    /* No error */
    return 0;
}

// void
// ui_pokemem_selector( const char *filename )
// {
//   /* Do nothing */
// }

// int
// ui_query( const char *message )
// {
//   /* Query confirmed */
//   return 1;
// }

// int
// ui_statusbar_update( ui_statusbar_item item, ui_statusbar_state state )
// {
//   /* No error */
//   return 0;
// }

// static int __counter_ui_statusbar_update_speed = 0;

// int
// ui_statusbar_update_speed( float speed )
// {
//   if (__counter_ui_statusbar_update_speed++ == 50) {
//     printf("speed %.0f\n", speed);
//     __counter_ui_statusbar_update_speed = 0;
//   }
//   /* No error */
//   return 0;
// }

// int
// ui_tape_browser_update( ui_tape_browser_update_type change,
//     libspectrum_tape_block *block )
// {
//   /* No error */
//   return 0;
// }

// int
// ui_widgets_reset( void )
// {
//   /* No error */
//   return 0;
// }

void uidisplay_area(int x, int y, int w, int h) {
    if (!txt_output_enabled) {
        return;
    }
    int ink, paper;
    int y2 = y + h;
    y -= (y % 2);
    y2 += (y2 % 2);
    for (int yy = y; yy < y2; yy += 2) {
        libspectrum_word *ptr1 = &image[yy][x];
        libspectrum_word *ptr2 = &image[yy + 1][x];
        int xx = x;
        while (xx < w + x) {
        ink = *ptr1;
        paper = *ptr2;
        if (paper == ink) {
            tb_set_cell(xx, yy / 2, ' ', PALETTE[ink], PALETTE[paper]);
        } else {
            tb_set_cell(xx, yy / 2, L'▀', PALETTE[ink], PALETTE[paper]);
        }
        ++xx;
        ++ptr1;
        ++ptr2;
        }
    }
}

void uidisplay_frame_end(void) {
    if (txt_output_enabled) {
        tb_present();
    }
}

int uidisplay_hotswap_gfx_mode(void) {
    /* No error */
    return 0;
}

int uidisplay_init(int width, int height) {
    /* No error */
    return 0;
}

int uidisplay_end(void) {
    /* No error */
    return 0;
}

void uidisplay_plot16(int x, int y, libspectrum_word data, libspectrum_byte ink,
                      libspectrum_byte paper) {
  /* Do nothing */
}

void uidisplay_plot8(int x, int y, libspectrum_byte data, libspectrum_byte ink,
                     libspectrum_byte paper) {
    x <<= 3;
    libspectrum_word *dest = &image[y][x];
    *(dest++) = (data & 0x80) ? ink : paper;
    *(dest++) = (data & 0x40) ? ink : paper;
    *(dest++) = (data & 0x20) ? ink : paper;
    *(dest++) = (data & 0x10) ? ink : paper;
    *(dest++) = (data & 0x08) ? ink : paper;
    *(dest++) = (data & 0x04) ? ink : paper;
    *(dest++) = (data & 0x02) ? ink : paper;
    *dest = (data & 0x01) ? ink : paper;
}

void uidisplay_putpixel(int x, int y, int colour) {
    fuzx_image[y][x] = colour;
}

void uidisplay_frame_save(void) {
}

void uidisplay_frame_restore(void) {
}
