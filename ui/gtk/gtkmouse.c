/* gtkmouse.c: GTK+ routines for emulating Spectrum mice
   Copyright (c) 2004 Darren Salt

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: linux@youmustbejoking.demon.co.uk

*/

#include "config.h"

#ifdef UI_GTK

#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

#include "gtkinternals.h"
#include "ui/ui.h"

/* For XWarpPointer *only* - see below */
#include <gdk/gdkx.h>
#include <X11/Xlib.h>

/* GDK1 bits */
#ifndef GDK_GRAB_SUCCESS
#define GDK_GRAB_SUCCESS 0
#endif

static GdkCursor *nullpointer = NULL;
const char *const nullpixmap[] = { "1 1 1 1", " 	c None", " " };

static void
gtkmouse_reset_pointer( void )
{
  /* Ugh. GDK doesn't have its own move-pointer function :-|
   * Framebuffer users and win32 users will have to make their own
   * arrangements here.
   */
  XWarpPointer( GDK_WINDOW_XDISPLAY( gtkui_drawing_area->window ), None,
		GDK_WINDOW_XWINDOW( gtkui_drawing_area->window ),
		0, 0, 0, 0, 128, 128 );
}

gboolean
gtkmouse_position( GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  if( !ui_mouse_grabbed ) return TRUE;

  if( event->x != 128 || event->y != 128 )
    gtkmouse_reset_pointer();
  ui_mouse_motion( event->x - 128, event->y - 128 );
  return TRUE;
}

gboolean
gtkmouse_button( GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if( event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS
      || event->type == GDK_3BUTTON_PRESS )
    ui_mouse_button( event->button, 1 );
  else
    ui_mouse_button( event->button, 0 );
  return TRUE;
}

int
ui_mouse_grab( int startup )
{
  if( startup ) return 0;

  if( !nullpointer ) {
    const char bits = 0;
    GdkColor colour = { 0 };
    GdkPixmap *image = gdk_bitmap_create_from_data( NULL, &bits, 1, 1 );
    nullpointer = gdk_cursor_new_from_pixmap( image, image, &colour, &colour,
					      1, 1 );
  }

  if( gdk_pointer_grab( gtkui_drawing_area->window, FALSE,
			GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK, gtkui_drawing_area->window,
			nullpointer, GDK_CURRENT_TIME ) == GDK_GRAB_SUCCESS ) {
    gtkmouse_reset_pointer();
    return 1;
  }

  ui_error( UI_ERROR_WARNING, "Mouse grab failed" );
  return 0;
}

int
ui_mouse_release( int suspend GCC_UNUSED )
{
  gdk_pointer_ungrab( GDK_CURRENT_TIME );
  return 0;
}

#endif
