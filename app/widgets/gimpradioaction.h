/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpradioaction.h
 * Copyright (C) 2004 Michael Natterer <mitch@gimp.org>
 * Copyright (C) 2008 Sven Neumann <sven@gimp.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GIMP_RADIO_ACTION_H__
#define __GIMP_RADIO_ACTION_H__


#include <gtk/gtkradioaction.h>


#define GIMP_TYPE_RADIO_ACTION            (gimp_radio_action_get_type ())
#define GIMP_RADIO_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_RADIO_ACTION, GimpRadioAction))
#define GIMP_RADIO_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_RADIO_ACTION, GimpRadioActionClass))
#define GIMP_IS_RADIO_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_RADIO_ACTION))
#define GIMP_IS_RADIO_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), GIMP_TYPE_ACTION))
#define GIMP_RADIO_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GIMP_TYPE_RADIO_ACTION, GimpRadioActionClass))


typedef GtkRadioAction      GimpRadioAction;
typedef GtkRadioActionClass GimpRadioActionClass;


GType            gimp_radio_action_get_type (void) G_GNUC_CONST;

GtkRadioAction * gimp_radio_action_new      (const gchar *name,
                                             const gchar *label,
                                             const gchar *tooltip,
                                             const gchar *stock_id,
                                             gint         value);


#endif  /* __GIMP_RADIO_ACTION_H__ */