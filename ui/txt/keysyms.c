/* keysyms.c: UI keysym to Fuse input layer keysym mappings
   Copyright (c) 2000-2007 Philip Kendall, Matan Ziv-Av, Russell Marks
                           Fredrick Meunier, Catalin Mihaila, Stuart Brady

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

/* This file is autogenerated from keysyms.dat by keysyms.pl.
   Do not edit unless you know what you're doing! */

#include "config.h"


#include "input.h"
#include "keyboard.h"

#include <ui/txt/termbox2.h>

keysyms_map_t keysyms_map[] = {

    {TB_KEY_TAB,    INPUT_KEY_Tab},
    {TB_KEY_ENTER,  INPUT_KEY_Return},
    {TB_KEY_ESC,    INPUT_KEY_Escape},
    {TB_KEY_SPACE,  INPUT_KEY_space},
    {'!',           INPUT_KEY_exclam},
    {'"',           INPUT_KEY_quotedbl},
    {'#',           INPUT_KEY_numbersign},
    {'$',           INPUT_KEY_dollar},
    {'&',           INPUT_KEY_ampersand},
    {'\'',          INPUT_KEY_apostrophe},
    {'(',           INPUT_KEY_parenleft},
    {')',           INPUT_KEY_parenright},
    {'*',           INPUT_KEY_asterisk},
    {'+',           INPUT_KEY_plus},
    {',',           INPUT_KEY_comma},
    {'-',           INPUT_KEY_minus},
    {'.',           INPUT_KEY_period},
    {'/',           INPUT_KEY_slash},
    {'0',           INPUT_KEY_0},
    {'1',           INPUT_KEY_1},
    {'2',           INPUT_KEY_2},
    {'3',           INPUT_KEY_3},
    {'4',           INPUT_KEY_4},
    {'5',           INPUT_KEY_5},
    {'6',           INPUT_KEY_6},
    {'7',           INPUT_KEY_7},
    {'8',           INPUT_KEY_8},
    {'9',           INPUT_KEY_9},
    {':',           INPUT_KEY_colon},
    {';',           INPUT_KEY_semicolon},
    {'<',           INPUT_KEY_less},
    {'=',           INPUT_KEY_equal},
    {'>',           INPUT_KEY_greater},
    {'?',           INPUT_KEY_question},
    {'@',           INPUT_KEY_at},
    {'[',           INPUT_KEY_bracketleft},
    {'\\',          INPUT_KEY_backslash},
    {']',           INPUT_KEY_bracketright},
    {'^',           INPUT_KEY_asciicircum},
    {'_',           INPUT_KEY_underscore},
    {'A',           INPUT_KEY_a},
    {'B',           INPUT_KEY_b},
    {'C',           INPUT_KEY_c},
    {'D',           INPUT_KEY_d},
    {'E',           INPUT_KEY_e},
    {'F',           INPUT_KEY_f},
    {'G',           INPUT_KEY_g},
    {'H',           INPUT_KEY_h},
    {'I',           INPUT_KEY_i},
    {'J',           INPUT_KEY_j},
    {'K',           INPUT_KEY_k},
    {'L',           INPUT_KEY_l},
    {'M',           INPUT_KEY_m},
    {'N',           INPUT_KEY_n},
    {'O',           INPUT_KEY_o},
    {'P',           INPUT_KEY_p},
    {'Q',           INPUT_KEY_q},
    {'R',           INPUT_KEY_r},
    {'S',           INPUT_KEY_s},
    {'T',           INPUT_KEY_t},
    {'U',           INPUT_KEY_u},
    {'V',           INPUT_KEY_v},
    {'W',           INPUT_KEY_w},
    {'X',           INPUT_KEY_x},
    {'Y',           INPUT_KEY_y},
    {'Z',           INPUT_KEY_z},
    {TB_KEY_BACKSPACE, INPUT_KEY_BackSpace},
    {TB_KEY_ARROW_UP, INPUT_KEY_Up},
    {TB_KEY_ARROW_DOWN, INPUT_KEY_Down},
    {TB_KEY_ARROW_LEFT, INPUT_KEY_Left},
    {TB_KEY_ARROW_RIGHT, INPUT_KEY_Right},
    {TB_KEY_INSERT, INPUT_KEY_Insert},
    {TB_KEY_DELETE, INPUT_KEY_Delete},
    {TB_KEY_HOME, INPUT_KEY_Home},
    {TB_KEY_END, INPUT_KEY_End},
    {TB_KEY_PGUP, INPUT_KEY_Page_Up},
    {TB_KEY_PGDN, INPUT_KEY_Page_Down},
    {TB_KEY_F1, INPUT_KEY_F1},
    {TB_KEY_F2, INPUT_KEY_F2},
    {TB_KEY_F3, INPUT_KEY_F3},
    {TB_KEY_F4, INPUT_KEY_F4},
    {TB_KEY_F5, INPUT_KEY_F5},
    {TB_KEY_F6, INPUT_KEY_F6},
    {TB_KEY_F7, INPUT_KEY_F7},
    {TB_KEY_F8, INPUT_KEY_F8},
    {TB_KEY_F9, INPUT_KEY_F9},
    {TB_KEY_F10, INPUT_KEY_F10},
    {TB_KEY_F11, INPUT_KEY_F11},
    {TB_KEY_F12, INPUT_KEY_F12},

    {0, 0} /* End marker: DO NOT MOVE! */

};
