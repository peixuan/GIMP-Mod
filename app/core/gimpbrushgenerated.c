/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimp_brush_generated module Copyright 1998 Jay Cox <jaycox@earthlink.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <glib-object.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpmath/gimpmath.h"

#include "core-types.h"

#include "base/temp-buf.h"

#include "gimpbrushgenerated.h"
#include "gimpbrushgenerated-load.h"
#include "gimpbrushgenerated-save.h"

#include "gimp-intl.h"


#define OVERSAMPLING 4


enum
{
  PROP_0,
  PROP_SHAPE,
  PROP_RADIUS,
  PROP_SPIKES,
  PROP_HARDNESS,
  PROP_ASPECT_RATIO,
  PROP_ANGLE
};


/*  local function prototypes  */

static void          gimp_brush_generated_set_property  (GObject      *object,
                                                         guint         property_id,
                                                         const GValue *value,
                                                         GParamSpec   *pspec);
static void          gimp_brush_generated_get_property  (GObject      *object,
                                                         guint         property_id,
                                                         GValue       *value,
                                                         GParamSpec   *pspec);

static void          gimp_brush_generated_dirty         (GimpData     *data);
static const gchar * gimp_brush_generated_get_extension (GimpData     *data);
static GimpData    * gimp_brush_generated_duplicate     (GimpData     *data);

static void          gimp_brush_generated_transform_size(GimpBrush    *gbrush,
                                                         gdouble       scale,
                                                         gdouble       aspect_ratio,
                                                         gdouble       angle,
                                                         gint         *width,
                                                         gint         *height);
static TempBuf     * gimp_brush_generated_transform_mask(GimpBrush    *gbrush,
                                                         gdouble       scale,
                                                         gdouble       aspect_ratio,
                                                         gdouble       angle,
                                                         gdouble       hardness);

static TempBuf     * gimp_brush_generated_calc          (GimpBrushGenerated      *brush,
                                                         GimpBrushGeneratedShape  shape,
                                                         gfloat                   radius,
                                                         gint                     spikes,
                                                         gfloat                   hardness,
                                                         gfloat                   aspect_ratio,
                                                         gfloat                   angle,
                                                         GimpVector2             *xaxis,
                                                         GimpVector2             *yaxis);
static void          gimp_brush_generated_get_half_size (GimpBrushGenerated      *gbrush,
                                                         GimpBrushGeneratedShape  shape,
                                                         gfloat                   radius,
                                                         gint                     spikes,
                                                         gfloat                   hardness,
                                                         gfloat                   aspect_ratio,
                                                         gdouble                  angle_in_degrees,
                                                         gint                    *half_width,
                                                         gint                    *half_height,
                                                         gdouble                 *_s,
                                                         gdouble                 *_c,
                                                         GimpVector2             *_x_axis,
                                                         GimpVector2             *_y_axis);


G_DEFINE_TYPE (GimpBrushGenerated, gimp_brush_generated, GIMP_TYPE_BRUSH)

#define parent_class gimp_brush_generated_parent_class


static void
gimp_brush_generated_class_init (GimpBrushGeneratedClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GimpDataClass  *data_class   = GIMP_DATA_CLASS (klass);
  GimpBrushClass *brush_class  = GIMP_BRUSH_CLASS (klass);

  object_class->set_property  = gimp_brush_generated_set_property;
  object_class->get_property  = gimp_brush_generated_get_property;

  data_class->save            = gimp_brush_generated_save;
  data_class->dirty           = gimp_brush_generated_dirty;
  data_class->get_extension   = gimp_brush_generated_get_extension;
  data_class->duplicate       = gimp_brush_generated_duplicate;

  brush_class->transform_size = gimp_brush_generated_transform_size;
  brush_class->transform_mask = gimp_brush_generated_transform_mask;

  g_object_class_install_property (object_class, PROP_SHAPE,
                                   g_param_spec_enum ("shape", NULL,
                                                      _("Brush Shape"),
                                                      GIMP_TYPE_BRUSH_GENERATED_SHAPE,
                                                      GIMP_BRUSH_GENERATED_CIRCLE,
                                                      GIMP_PARAM_READWRITE |
                                                      G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class, PROP_RADIUS,
                                   g_param_spec_double ("radius", NULL,
                                                        _("Brush Radius"),
                                                        0.1, 4000.0, 5.0,
                                                        GIMP_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class, PROP_SPIKES,
                                   g_param_spec_int    ("spikes", NULL,
                                                        _("Brush Spikes"),
                                                        2, 20, 2,
                                                        GIMP_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class, PROP_HARDNESS,
                                   g_param_spec_double ("hardness", NULL,
                                                        _("Brush Hardness"),
                                                        0.0, 1.0, 0.0,
                                                        GIMP_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class, PROP_ASPECT_RATIO,
                                   g_param_spec_double ("aspect-ratio",
                                                        NULL,
                                                        _("Brush Aspect Ratio"),
                                                        1.0, 20.0, 1.0,
                                                        GIMP_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class, PROP_ANGLE,
                                   g_param_spec_double ("angle", NULL,
                                                        _("Brush Angle"),
                                                        0.0, 180.0, 0.0,
                                                        GIMP_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));
}

static void
gimp_brush_generated_init (GimpBrushGenerated *brush)
{
  brush->shape        = GIMP_BRUSH_GENERATED_CIRCLE;
  brush->radius       = 5.0;
  brush->hardness     = 0.0;
  brush->aspect_ratio = 1.0;
  brush->angle        = 0.0;
}

static void
gimp_brush_generated_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GimpBrushGenerated *brush = GIMP_BRUSH_GENERATED (object);

  switch (property_id)
    {
    case PROP_SHAPE:
      gimp_brush_generated_set_shape (brush, g_value_get_enum (value));
      break;
    case PROP_RADIUS:
      gimp_brush_generated_set_radius (brush, g_value_get_double (value));
      break;
    case PROP_SPIKES:
      gimp_brush_generated_set_spikes (brush, g_value_get_int (value));
      break;
    case PROP_HARDNESS:
      gimp_brush_generated_set_hardness (brush, g_value_get_double (value));
      break;
    case PROP_ASPECT_RATIO:
      gimp_brush_generated_set_aspect_ratio (brush, g_value_get_double (value));
      break;
    case PROP_ANGLE:
      gimp_brush_generated_set_angle (brush, g_value_get_double (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_brush_generated_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GimpBrushGenerated *brush = GIMP_BRUSH_GENERATED (object);

  switch (property_id)
    {
    case PROP_SHAPE:
      g_value_set_enum (value, brush->shape);
      break;
    case PROP_RADIUS:
      g_value_set_double (value, brush->radius);
      break;
    case PROP_SPIKES:
      g_value_set_int (value, brush->spikes);
      break;
    case PROP_HARDNESS:
      g_value_set_double (value, brush->hardness);
      break;
    case PROP_ASPECT_RATIO:
      g_value_set_double (value, brush->aspect_ratio);
      break;
    case PROP_ANGLE:
      g_value_set_double (value, brush->angle);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_brush_generated_dirty (GimpData *data)
{
  GimpBrushGenerated *brush  = GIMP_BRUSH_GENERATED (data);
  GimpBrush          *gbrush = GIMP_BRUSH (brush);

  if (gbrush->mask)
    temp_buf_free (gbrush->mask);

  gbrush->mask = gimp_brush_generated_calc (brush,
                                            brush->shape,
                                            brush->radius,
                                            brush->spikes,
                                            brush->hardness,
                                            brush->aspect_ratio,
                                            brush->angle,
                                            &gbrush->x_axis,
                                            &gbrush->y_axis);

  GIMP_DATA_CLASS (parent_class)->dirty (data);
}

static const gchar *
gimp_brush_generated_get_extension (GimpData *data)
{
  return GIMP_BRUSH_GENERATED_FILE_EXTENSION;
}

static GimpData *
gimp_brush_generated_duplicate (GimpData *data)
{
  GimpBrushGenerated *brush = GIMP_BRUSH_GENERATED (data);

  return gimp_brush_generated_new (gimp_object_get_name (brush),
                                   brush->shape,
                                   brush->radius,
                                   brush->spikes,
                                   brush->hardness,
                                   brush->aspect_ratio,
                                   brush->angle);
}

static void
gimp_brush_generated_transform_size (GimpBrush *gbrush,
                                     gdouble    scale,
                                     gdouble    aspect_ratio,
                                     gdouble    angle,
                                     gint      *width,
                                     gint      *height)
{
  GimpBrushGenerated *brush = GIMP_BRUSH_GENERATED (gbrush);
  gint                half_width;
  gint                half_height;
  gdouble             ratio;

  if (aspect_ratio == 0.0)
    {
      ratio = brush->aspect_ratio;
    }
  else
    {
      ratio = MIN (fabs (aspect_ratio) + 1, 20);
      /* Since generated brushes are symmetric the dont have input
       * for aspect ratios  < 1.0. its same as rotate by 90 degrees and
       * 1 / ratio. So we fix the input up for this case.   */

      if (aspect_ratio < 0.0)
        {
          angle = angle + 0.25;
        }
    }




  gimp_brush_generated_get_half_size (brush,
                                      brush->shape,
                                      brush->radius * (scale / 2),
                                      brush->spikes,
                                      brush->hardness,
                                      ratio,
                                      (brush->angle + 360 * angle),
                                      &half_width, &half_height,
                                      NULL, NULL, NULL, NULL);

  *width  = half_width  * 2 + 1;
  *height = half_height * 2 + 1;
}

static TempBuf *
gimp_brush_generated_transform_mask (GimpBrush *gbrush,
                                     gdouble    scale,
                                     gdouble    aspect_ratio,
                                     gdouble    angle,
                                     gdouble    hardness)
{
  GimpBrushGenerated *brush  = GIMP_BRUSH_GENERATED (gbrush);
  gdouble             ratio;

  if (aspect_ratio == 0.0)
    {
      ratio = brush->aspect_ratio;
    }
  else
    {
      ratio = MIN (fabs (aspect_ratio) + 1, 20);
      /* Since generated brushes are symmetric the dont have input
       * for aspect ratios  < 1.0. its same as rotate by 90 degrees and
       * 1 / ratio. So we fix the input up for this case.   */

      if (aspect_ratio < 0.0)
        {
          angle = angle + 0.25;
        }
   }


  return gimp_brush_generated_calc (brush,
                                    brush->shape,
                                    brush->radius * (scale / 2),
                                    brush->spikes,
                                    brush->hardness * hardness,
                                    ratio,
                                    (brush->angle + 360 * angle),
                                    NULL, NULL);
}


/*  the actual brush rendering functions  */

static gdouble
gauss (gdouble f)
{
  /* this aint' a real gauss function */
  if (f < -0.5)
    {
      f = -1.0 - f;
      return (2.0 * f*f);
    }

  if (f < 0.5)
    return (1.0 - 2.0 * f*f);

  f = 1.0 - f;
  return (2.0 * f*f);
}

/* set up lookup table */
static guchar *
gimp_brush_generated_calc_lut (gdouble radius,
                               gdouble hardness)
{
  guchar  *lookup;
  gint     length;
  gint     x;
  gdouble  d;
  gdouble  sum;
  gdouble  exponent;
  gdouble  buffer[OVERSAMPLING];

  length = OVERSAMPLING * ceil (1 + sqrt (2 * SQR (ceil (radius + 1.0))));

  lookup = g_malloc (length);
  sum = 0.0;

  if ((1.0 - hardness) < 0.0000004)
    exponent = 1000000.0;
  else
    exponent = 0.4 / (1.0 - hardness);

  for (x = 0; x < OVERSAMPLING; x++)
    {
      d = fabs ((x + 0.5) / OVERSAMPLING - 0.5);

      if (d > radius)
        buffer[x] = 0.0;
      else
        buffer[x] = gauss (pow (d / radius, exponent));

      sum += buffer[x];
    }

  for (x = 0; d < radius || sum > 0.00001; d += 1.0 / OVERSAMPLING)
    {
      sum -= buffer[x % OVERSAMPLING];

      if (d > radius)
        buffer[x % OVERSAMPLING] = 0.0;
      else
        buffer[x % OVERSAMPLING] = gauss (pow (d / radius, exponent));

      sum += buffer[x % OVERSAMPLING];
      lookup[x++] = RINT (sum * (255.0 / OVERSAMPLING));
    }

  while (x < length)
    {
      lookup[x++] = 0;
    }

  return lookup;
}

static TempBuf *
gimp_brush_generated_calc (GimpBrushGenerated      *brush,
                           GimpBrushGeneratedShape  shape,
                           gfloat                   radius,
                           gint                     spikes,
                           gfloat                   hardness,
                           gfloat                   aspect_ratio,
                           gfloat                   angle,
                           GimpVector2             *xaxis,
                           GimpVector2             *yaxis)
{
  guchar      *centerp;
  guchar      *lookup;
  guchar       a;
  gint         half_width  = 0;
  gint         half_height = 0;
  gint         x, y;
  gdouble      c, s, cs, ss;
  GimpVector2  x_axis;
  GimpVector2  y_axis;
  TempBuf     *mask;

  gimp_brush_generated_get_half_size (brush,
                                      shape,
                                      radius,
                                      spikes,
                                      hardness,
                                      aspect_ratio,
                                      angle,
                                      &half_width, &half_height,
                                      &s, &c, &x_axis, &y_axis);

  mask = temp_buf_new (half_width  * 2 + 1,
                       half_height * 2 + 1,
                       1, half_width, half_height, NULL);

  centerp = temp_buf_get_data (mask) + half_height * mask->width + half_width;

  lookup = gimp_brush_generated_calc_lut (radius, hardness);

  cs = cos (- 2 * G_PI / spikes);
  ss = sin (- 2 * G_PI / spikes);

  /* for an even number of spikes compute one half and mirror it */
  for (y = (spikes % 2 ? -half_height : 0); y <= half_height; y++)
    {
      for (x = -half_width; x <= half_width; x++)
        {
          gdouble d  = 0;
          gdouble tx = c * x - s * y;
          gdouble ty = fabs (s * x + c * y);

          if (spikes > 2)
            {
              gdouble angle = atan2 (ty, tx);

              while (angle > G_PI / spikes)
                {
                  gdouble sx = tx;
                  gdouble sy = ty;

                  tx = cs * sx - ss * sy;
                  ty = ss * sx + cs * sy;

                  angle -= 2 * G_PI / spikes;
                }
            }

          ty *= aspect_ratio;

          switch (shape)
            {
            case GIMP_BRUSH_GENERATED_CIRCLE:
              d = sqrt (SQR (tx) + SQR (ty));
              break;
            case GIMP_BRUSH_GENERATED_SQUARE:
              d = MAX (fabs (tx), fabs (ty));
              break;
            case GIMP_BRUSH_GENERATED_DIAMOND:
              d = fabs (tx) + fabs (ty);
              break;
            }

          if (d < radius + 1)
            a = lookup[(gint) RINT (d * OVERSAMPLING)];
          else
            a = 0;

          centerp[y * mask->width + x] = a;

          if (spikes % 2 == 0)
            centerp[-1 * y * mask->width - x] = a;
        }
    }

  g_free (lookup);

  if (xaxis)
    *xaxis = x_axis;

  if (yaxis)
    *yaxis = y_axis;

  return mask;
}

/* This function is shared between gimp_brush_generated_transform_size and
 * gimp_brush_generated_calc, therefore we provide a bunch of optional
 * pointers for returnvalues.
 */
static void
gimp_brush_generated_get_half_size (GimpBrushGenerated      *gbrush,
                                    GimpBrushGeneratedShape  shape,
                                    gfloat                   radius,
                                    gint                     spikes,
                                    gfloat                   hardness,
                                    gfloat                   aspect_ratio,
                                    gdouble                  angle_in_degrees,
                                    gint                    *half_width,
                                    gint                    *half_height,
                                    gdouble                 *_s,
                                    gdouble                 *_c,
                                    GimpVector2             *_x_axis,
                                    GimpVector2             *_y_axis)
{
  gdouble      c, s;
  gdouble      short_radius;
  GimpVector2  x_axis;
  GimpVector2  y_axis;

  /* Since floatongpoint is not really accurate,
   * we need to round to limit the errors.
   * Errors in some border cases resulted in
   * different height and width reported for
   * the same input value on calling procedure side.
   * This became problem at the rise of dynamics that
   * allows for any angle to turn up.
   **/

  angle_in_degrees = ROUND (angle_in_degrees * 1000.0) / 1000.0;

  s = sin (gimp_deg_to_rad (angle_in_degrees));
  c = cos (gimp_deg_to_rad (angle_in_degrees));

  short_radius = radius / aspect_ratio;

  x_axis.x =        c * radius;
  x_axis.y = -1.0 * s * radius;
  y_axis.x =        s * short_radius;
  y_axis.y =        c * short_radius;

  switch (shape)
    {
    case GIMP_BRUSH_GENERATED_CIRCLE:
      *half_width  = ceil (sqrt (x_axis.x * x_axis.x + y_axis.x * y_axis.x));
      *half_height = ceil (sqrt (x_axis.y * x_axis.y + y_axis.y * y_axis.y));
      break;

    case GIMP_BRUSH_GENERATED_SQUARE:
      *half_width  = ceil (fabs (x_axis.x) + fabs (y_axis.x));
      *half_height = ceil (fabs (x_axis.y) + fabs (y_axis.y));
      break;

    case GIMP_BRUSH_GENERATED_DIAMOND:
      *half_width  = ceil (MAX (fabs (x_axis.x), fabs (y_axis.x)));
      *half_height = ceil (MAX (fabs (x_axis.y), fabs (y_axis.y)));
      break;
    }

  if (spikes > 2)
    {
      /* could be optimized by respecting the angle */
      *half_width = *half_height = ceil (sqrt (radius * radius +
                                               short_radius * short_radius));
      y_axis.x = s * radius;
      y_axis.y = c * radius;
    }

  /*  These will typically be set then this function is called by
   *  gimp_brush_generated_calc, which needs the values in its algorithms.
   */
  if (_s != NULL)
    *_s = s;

  if (_c != NULL)
    *_c = c;

  if (_x_axis != NULL)
    *_x_axis = x_axis;

  if (_y_axis != NULL)
    *_y_axis = y_axis;
}


/*  public functions  */

GimpData *
gimp_brush_generated_new (const gchar             *name,
                          GimpBrushGeneratedShape  shape,
                          gfloat                   radius,
                          gint                     spikes,
                          gfloat                   hardness,
                          gfloat                   aspect_ratio,
                          gfloat                   angle)
{
  GimpBrushGenerated *brush;

  g_return_val_if_fail (name != NULL, NULL);
  g_return_val_if_fail (*name != '\0', NULL);

  brush = g_object_new (GIMP_TYPE_BRUSH_GENERATED,
                        "name",         name,
                        "mime-type",    "application/x-gimp-brush-generated",
                        "spacing",      20.0,
                        "shape",        shape,
                        "radius",       radius,
                        "spikes",       spikes,
                        "hardness",     hardness,
                        "aspect-ratio", aspect_ratio,
                        "angle",        angle,
                        NULL);

  return GIMP_DATA (brush);
}

GimpBrushGeneratedShape
gimp_brush_generated_set_shape (GimpBrushGenerated      *brush,
                                GimpBrushGeneratedShape  shape)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush),
                        GIMP_BRUSH_GENERATED_CIRCLE);

  if (brush->shape != shape)
    {
      brush->shape = shape;

      g_object_notify (G_OBJECT (brush), "shape");
      gimp_data_dirty (GIMP_DATA (brush));
    }

  return brush->shape;
}

gfloat
gimp_brush_generated_set_radius (GimpBrushGenerated *brush,
                                 gfloat              radius)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  radius = CLAMP (radius, 0.0, 32767.0);

  if (brush->radius != radius)
    {
      brush->radius = radius;

      g_object_notify (G_OBJECT (brush), "radius");
      gimp_data_dirty (GIMP_DATA (brush));
    }

  return brush->radius;
}

gint
gimp_brush_generated_set_spikes (GimpBrushGenerated *brush,
                                 gint                spikes)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1);

  spikes = CLAMP (spikes, 2, 20);

  if (brush->spikes != spikes)
    {
      brush->spikes = spikes;

      g_object_notify (G_OBJECT (brush), "spikes");
      gimp_data_dirty (GIMP_DATA (brush));
    }

  return brush->spikes;
}

gfloat
gimp_brush_generated_set_hardness (GimpBrushGenerated *brush,
                                   gfloat              hardness)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  hardness = CLAMP (hardness, 0.0, 1.0);

  if (brush->hardness != hardness)
    {
      brush->hardness = hardness;

      g_object_notify (G_OBJECT (brush), "hardness");
      gimp_data_dirty (GIMP_DATA (brush));
    }

  return brush->hardness;
}

gfloat
gimp_brush_generated_set_aspect_ratio (GimpBrushGenerated *brush,
                                       gfloat              ratio)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  ratio = CLAMP (ratio, 1.0, 1000.0);

  if (brush->aspect_ratio != ratio)
    {
      brush->aspect_ratio = ratio;

      g_object_notify (G_OBJECT (brush), "aspect-ratio");
      gimp_data_dirty (GIMP_DATA (brush));
    }

  return brush->aspect_ratio;
}

gfloat
gimp_brush_generated_set_angle (GimpBrushGenerated *brush,
                                gfloat              angle)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  if (angle < 0.0)
    angle = -1.0 * fmod (angle, 180.0);
  else if (angle > 180.0)
    angle = fmod (angle, 180.0);

  if (brush->angle != angle)
    {
      brush->angle = angle;

      g_object_notify (G_OBJECT (brush), "angle");
      gimp_data_dirty (GIMP_DATA (brush));
    }

  return brush->angle;
}

GimpBrushGeneratedShape
gimp_brush_generated_get_shape (const GimpBrushGenerated *brush)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush),
                        GIMP_BRUSH_GENERATED_CIRCLE);

  return brush->shape;
}

gfloat
gimp_brush_generated_get_radius (const GimpBrushGenerated *brush)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  return brush->radius;
}

gint
gimp_brush_generated_get_spikes (const GimpBrushGenerated *brush)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1);

  return brush->spikes;
}

gfloat
gimp_brush_generated_get_hardness (const GimpBrushGenerated *brush)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  return brush->hardness;
}

gfloat
gimp_brush_generated_get_aspect_ratio (const GimpBrushGenerated *brush)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  return brush->aspect_ratio;
}

gfloat
gimp_brush_generated_get_angle (const GimpBrushGenerated *brush)
{
  g_return_val_if_fail (GIMP_IS_BRUSH_GENERATED (brush), -1.0);

  return brush->angle;
}
