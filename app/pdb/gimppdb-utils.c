/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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

#include <string.h>

#include <gegl.h>

#include "pdb-types.h"

#include "core/gimp.h"
#include "core/gimpbrushgenerated.h"
#include "core/gimpcontainer.h"
#include "core/gimpdatafactory.h"
#include "core/gimpdrawable.h"
#include "core/gimpimage.h"
#include "core/gimpitem.h"

#include "text/gimptextlayer.h"

#include "vectors/gimpvectors.h"

#include "gimppdb-utils.h"
#include "gimppdberror.h"

#include "gimp-intl.h"


static GimpObject *
gimp_pdb_get_data_factory_item (GimpDataFactory *data_factory,
                                const gchar     *name)
{
  GimpObject *gimp_object;

  gimp_object = gimp_container_get_child_by_name (gimp_data_factory_get_container (data_factory), name);

  if (! gimp_object)
    gimp_object = gimp_container_get_child_by_name (gimp_data_factory_get_container_obsolete (data_factory), name);

  return gimp_object;
}


GimpBrush *
gimp_pdb_get_brush (Gimp         *gimp,
                    const gchar  *name,
                    gboolean      writable,
                    GError      **error)
{
  GimpBrush *brush;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty brush name"));
      return NULL;
    }

  brush = (GimpBrush *) gimp_pdb_get_data_factory_item (gimp->brush_factory, name);

  if (! brush)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Brush '%s' not found"), name);
    }
  else if (writable && ! gimp_data_is_writable (GIMP_DATA (brush)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Brush '%s' is not editable"), name);
      return NULL;
    }

  return brush;
}

GimpBrush *
gimp_pdb_get_generated_brush (Gimp         *gimp,
                              const gchar  *name,
                              gboolean      writable,
                              GError      **error)
{
  GimpBrush *brush;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  brush = gimp_pdb_get_brush (gimp, name, writable, error);

  if (! brush)
    return NULL;

  if (! GIMP_IS_BRUSH_GENERATED (brush))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Brush '%s' is not a generated brush"), name);
      return NULL;
    }

  return brush;
}

GimpDynamics *
gimp_pdb_get_dynamics (Gimp         *gimp,
                       const gchar  *name,
                       gboolean      writable,
                       GError      **error)
{
  GimpDynamics *dynamics;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty paint dynamics name"));
      return NULL;
    }

  dynamics = (GimpDynamics *) gimp_pdb_get_data_factory_item (gimp->dynamics_factory, name);

  if (! dynamics)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Paint dynamics '%s' not found"), name);
    }
  else if (writable && ! gimp_data_is_writable (GIMP_DATA (dynamics)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Paint dynamics '%s' is not editable"), name);
      return NULL;
    }

  return dynamics;
}

GimpPattern *
gimp_pdb_get_pattern (Gimp         *gimp,
                      const gchar  *name,
                      GError      **error)
{
  GimpPattern *pattern;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty pattern name"));
      return NULL;
    }

  pattern = (GimpPattern *) gimp_pdb_get_data_factory_item (gimp->pattern_factory, name);

  if (! pattern)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Pattern '%s' not found"), name);
    }

  return pattern;
}

GimpGradient *
gimp_pdb_get_gradient (Gimp         *gimp,
                       const gchar  *name,
                       gboolean      writable,
                       GError      **error)
{
  GimpGradient *gradient;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty gradient name"));
      return NULL;
    }

  gradient = (GimpGradient *) gimp_pdb_get_data_factory_item (gimp->gradient_factory, name);

  if (! gradient)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Gradient '%s' not found"), name);
    }
  else if (writable && ! gimp_data_is_writable (GIMP_DATA (gradient)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Gradient '%s' is not editable"), name);
      return NULL;
    }

  return gradient;
}

GimpPalette *
gimp_pdb_get_palette (Gimp         *gimp,
                      const gchar  *name,
                      gboolean      writable,
                      GError      **error)
{
  GimpPalette *palette;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty palette name"));
      return NULL;
    }

  palette = (GimpPalette *) gimp_pdb_get_data_factory_item (gimp->palette_factory, name);

  if (! palette)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Palette '%s' not found"), name);
    }
  else if (writable && ! gimp_data_is_writable (GIMP_DATA (palette)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Palette '%s' is not editable"), name);
      return NULL;
    }

  return palette;
}

GimpFont *
gimp_pdb_get_font (Gimp         *gimp,
                   const gchar  *name,
                   GError      **error)
{
  GimpFont *font;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty font name"));
      return NULL;
    }

  font = (GimpFont *)
    gimp_container_get_child_by_name (gimp->fonts, name);

  if (! font)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Font '%s' not found"), name);
    }

  return font;
}

GimpBuffer *
gimp_pdb_get_buffer (Gimp         *gimp,
                     const gchar  *name,
                     GError      **error)
{
  GimpBuffer *buffer;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty buffer name"));
      return NULL;
    }

  buffer = (GimpBuffer *)
    gimp_container_get_child_by_name (gimp->named_buffers, name);

  if (! buffer)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Named buffer '%s' not found"), name);
    }

  return buffer;
}

GimpPaintInfo *
gimp_pdb_get_paint_info (Gimp         *gimp,
                         const gchar  *name,
                         GError      **error)
{
  GimpPaintInfo *paint_info;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! name || ! strlen (name))
    {
      g_set_error_literal (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
			   _("Invalid empty paint method name"));
      return NULL;
    }

  paint_info = (GimpPaintInfo *)
    gimp_container_get_child_by_name (gimp->paint_info_list, name);

  if (! paint_info)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Paint method '%s' does not exist"), name);
    }

  return paint_info;
}

gboolean
gimp_pdb_item_is_attached (GimpItem  *item,
                           GimpImage *image,
                           gboolean   writable,
                           GError   **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (image == NULL || GIMP_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (! gimp_item_is_attached (item))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) cannot be used because it has not "
                     "been added to an image"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }

  if (image && image != gimp_item_get_image (item))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) cannot be used because it is "
                     "attached to another image"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }

  if (writable)
    return gimp_pdb_item_is_writable (item, error);

  return TRUE;
}

gboolean
gimp_pdb_item_is_in_tree (GimpItem   *item,
                          GimpImage  *image,
                          gboolean    writable,
                          GError    **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (image == NULL || GIMP_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (! gimp_pdb_item_is_attached (item, image, writable, error))
    return FALSE;

  if (! gimp_item_get_tree (item))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) cannot be used because it is not "
                     "a direct child of an item tree"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }

  return TRUE;
}

gboolean
gimp_pdb_item_is_in_same_tree (GimpItem   *item,
                               GimpItem   *item2,
                               GimpImage  *image,
                               GError    **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (GIMP_IS_ITEM (item2), FALSE);
  g_return_val_if_fail (image == NULL || GIMP_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (! gimp_pdb_item_is_in_tree (item, image, FALSE, error) ||
      ! gimp_pdb_item_is_in_tree (item2, image, FALSE, error))
    return FALSE;

  if (gimp_item_get_tree (item) != gimp_item_get_tree (item2))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Items '%s' (%d) and '%s' (%d) cannot be used "
                     "because they are not part of the same item tree"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item),
                   gimp_object_get_name (item2),
                   gimp_item_get_ID (item2));
      return FALSE;
    }

  return TRUE;
}

gboolean
gimp_pdb_item_is_not_ancestor (GimpItem  *item,
                               GimpItem  *not_descendant,
                               GError   **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (GIMP_IS_ITEM (not_descendant), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (gimp_viewable_is_ancestor (GIMP_VIEWABLE (item),
                                 GIMP_VIEWABLE (not_descendant)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) must not be an ancestor of "
                     "'%s' (%d)"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item),
                   gimp_object_get_name (not_descendant),
                   gimp_item_get_ID (not_descendant));
      return FALSE;
    }

  return TRUE;
}

gboolean
gimp_pdb_item_is_floating (GimpItem  *item,
                           GimpImage *dest_image,
                           GError   **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (GIMP_IS_IMAGE (dest_image), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (! g_object_is_floating (item))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) has already been added to an image"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }
  else if (gimp_item_get_image (item) != dest_image)
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Trying to add item '%s' (%d) to wrong image"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }

  return TRUE;
}

gboolean
gimp_pdb_item_is_writable (GimpItem  *item,
                           GError   **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (gimp_item_is_content_locked (item))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) cannot be modified because its "
                     "contents are locked"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }

  return TRUE;
}

gboolean
gimp_pdb_item_is_group (GimpItem  *item,
                        GError   **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (! gimp_viewable_get_children (GIMP_VIEWABLE (item)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) cannot be used because it is "
                     "not a group item"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }

  return TRUE;
}

gboolean
gimp_pdb_item_is_not_group (GimpItem  *item,
                            GError   **error)
{
  g_return_val_if_fail (GIMP_IS_ITEM (item), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (gimp_viewable_get_children (GIMP_VIEWABLE (item)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Item '%s' (%d) cannot be modified because it "
                     "is a group item"),
                   gimp_object_get_name (item),
                   gimp_item_get_ID (item));
      return FALSE;
    }

  return TRUE;
}

gboolean
gimp_pdb_layer_is_text_layer (GimpLayer  *layer,
                              gboolean    writable,
                              GError    **error)
{
  g_return_val_if_fail (GIMP_IS_LAYER (layer), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (! gimp_item_is_text_layer (GIMP_ITEM (layer)))
    {
      g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                   _("Layer '%s' (%d) cannot be used because it is not "
                     "a text layer"),
                   gimp_object_get_name (layer),
                   gimp_item_get_ID (GIMP_ITEM (layer)));

      return FALSE;
    }

  return gimp_pdb_item_is_attached (GIMP_ITEM (layer), NULL, writable, error);
}

static const gchar *
gimp_pdb_enum_value_get_nick (GType enum_type,
                              gint  value)
{
  GEnumClass  *enum_class;
  GEnumValue  *enum_value;
  const gchar *nick;

  enum_class = g_type_class_ref (enum_type);
  enum_value = g_enum_get_value (enum_class, value);

  nick = enum_value->value_nick;

  g_type_class_unref (enum_class);

  return nick;
}

gboolean
gimp_pdb_image_is_base_type (GimpImage          *image,
                             GimpImageBaseType   type,
                             GError            **error)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (gimp_image_base_type (image) == type)
    return TRUE;

  g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
               _("Image '%s' (%d) is of type '%s', "
                 "but an image of type '%s' is expected"),
               gimp_image_get_display_name (image),
               gimp_image_get_ID (image),
               gimp_pdb_enum_value_get_nick (GIMP_TYPE_IMAGE_BASE_TYPE,
                                             gimp_image_base_type (image)),
               gimp_pdb_enum_value_get_nick (GIMP_TYPE_IMAGE_BASE_TYPE, type));

  return FALSE;
}

gboolean
gimp_pdb_image_is_not_base_type (GimpImage          *image,
                                 GimpImageBaseType   type,
                                 GError            **error)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (gimp_image_base_type (image) != type)
    return TRUE;

  g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
               _("Image '%s' (%d) is already of type '%s'"),
               gimp_image_get_display_name (image),
               gimp_image_get_ID (image),
               gimp_pdb_enum_value_get_nick (GIMP_TYPE_IMAGE_BASE_TYPE, type));

  return FALSE;
}

GimpStroke *
gimp_pdb_get_vectors_stroke (GimpVectors  *vectors,
                             gint          stroke_ID,
                             gboolean      writable,
                             GError      **error)
{
  GimpStroke *stroke = NULL;

  g_return_val_if_fail (GIMP_IS_VECTORS (vectors), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! gimp_pdb_item_is_not_group (GIMP_ITEM (vectors), error))
    return NULL;

  if (! writable || gimp_pdb_item_is_writable (GIMP_ITEM (vectors), error))
    {
      stroke = gimp_vectors_stroke_get_by_ID (vectors, stroke_ID);

      if (! stroke)
        g_set_error (error, GIMP_PDB_ERROR, GIMP_PDB_INVALID_ARGUMENT,
                     _("Vectors object %d does not contain stroke with ID %d"),
                     gimp_item_get_ID (GIMP_ITEM (vectors)), stroke_ID);
    }

  return stroke;
}
