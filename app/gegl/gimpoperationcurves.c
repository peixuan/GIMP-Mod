/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpoperationcurves.c
 * Copyright (C) 2007 Michael Natterer <mitch@gimp.org>
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

#include "config.h"

#include <gegl.h>

#include "libgimpcolor/gimpcolor.h"
#include "libgimpmath/gimpmath.h"

#include "gegl-types.h"

#include "gimpcurvesconfig.h"
#include "gimpoperationcurves.h"


enum
{
  PROP_0,
  PROP_CONFIG
};


static void     gimp_operation_curves_finalize     (GObject       *object);
static void     gimp_operation_curves_get_property (GObject       *object,
                                                    guint          property_id,
                                                    GValue        *value,
                                                    GParamSpec    *pspec);
static void     gimp_operation_curves_set_property (GObject       *object,
                                                    guint          property_id,
                                                    const GValue  *value,
                                                    GParamSpec    *pspec);

static gboolean gimp_operation_curves_process      (GeglOperation *operation,
                                                    void          *in_buf,
                                                    void          *out_buf,
                                                    glong          samples);


G_DEFINE_TYPE (GimpOperationCurves, gimp_operation_curves,
               GEGL_TYPE_OPERATION_POINT_FILTER)

#define parent_class gimp_operation_curves_parent_class


static void
gimp_operation_curves_class_init (GimpOperationCurvesClass * klass)
{
  GObjectClass                  *object_class    = G_OBJECT_CLASS (klass);
  GeglOperationClass            *operation_class = GEGL_OPERATION_CLASS (klass);
  GeglOperationPointFilterClass *point_class     = GEGL_OPERATION_POINT_FILTER_CLASS (klass);

  object_class->finalize     = gimp_operation_curves_finalize;
  object_class->set_property = gimp_operation_curves_set_property;
  object_class->get_property = gimp_operation_curves_get_property;

  point_class->process       = gimp_operation_curves_process;

  gegl_operation_class_set_name (operation_class, "gimp-curves");

  g_object_class_install_property (object_class, PROP_CONFIG,
                                   g_param_spec_object ("config",
                                                        "Config",
                                                        "The config object",
                                                        GIMP_TYPE_CURVES_CONFIG,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));
}

static void
gimp_operation_curves_init (GimpOperationCurves *self)
{
}

static void
gimp_operation_curves_finalize (GObject *object)
{
  GimpOperationCurves *self = GIMP_OPERATION_CURVES (object);

  if (self->config)
    {
      g_object_unref (self->config);
      self->config = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gimp_operation_curves_get_property (GObject    *object,
                                    guint       property_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GimpOperationCurves *self = GIMP_OPERATION_CURVES (object);

  switch (property_id)
    {
    case PROP_CONFIG:
      g_value_set_object (value, self->config);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_operation_curves_set_property (GObject      *object,
                                    guint         property_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GimpOperationCurves *self = GIMP_OPERATION_CURVES (object);

  switch (property_id)
    {
    case PROP_CONFIG:
      if (self->config)
        g_object_unref (self->config);
      self->config = g_value_dup_object (value);
      break;

   default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static inline gdouble
gimp_operation_curves_map (gdouble    value,
                           GimpCurve *curve)
{
  if (value < 0.0)
    {
      value = curve->curve[0] / 255.0;
    }
  else if (value >= 1.0)
    {
      value = curve->curve[255] / 255.0;
    }
  else /* interpolate the curve */
    {
      gint    index = floor (value * 255.0);
      gdouble f     = value * 255.0 - index;

      value = ((1.0 - f) * curve->curve[index    ] +
                      f  * curve->curve[index + 1] ) / 255.0;
    }

  return value;
}

static gboolean
gimp_operation_curves_process (GeglOperation *operation,
                               void          *in_buf,
                               void          *out_buf,
                               glong          samples)
{
  GimpOperationCurves *self   = GIMP_OPERATION_CURVES (operation);
  GimpCurvesConfig    *config = self->config;
  gfloat              *src    = in_buf;
  gfloat              *dest   = out_buf;
  glong                sample;

  if (! config)
    return FALSE;

  for (sample = 0; sample < samples; sample++)
    {
      gint channel;

      for (channel = 0; channel < 4; channel++)
        {
          gdouble value;

          value = gimp_operation_curves_map (src[channel],
                                             config->curve[channel + 1]);

          /* don't apply the overall curve to the alpha channel */
          if (channel != ALPHA_PIX)
            value = gimp_operation_curves_map (value,
                                               config->curve[0]);

          dest[channel] = value;
        }

      src  += 4;
      dest += 4;
    }

  return TRUE;
}