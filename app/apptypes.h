/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
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

#ifndef __APPTYPES_H__
#define __APPTYPES_H__


#include "undo_types.h"

/*  these will go into a general purpose library  */
#include "libgimp/gimpcolor.h"
#include "libgimp/gimpmatrix.h"
#include "libgimp/gimpvector.h"
#include "libgimp/gimpunit.h"

#include "libgimp/gimpuitypes.h"

#include "appenums.h"


/*  objects  */

typedef struct _GimpObject          GimpObject;

typedef struct _GimpSet             GimpSet;
typedef         guint               GimpSetHandlerId;

typedef struct _GimpList            GimpList;

typedef struct _GimpContext         GimpContext;

typedef struct _GimpDrawable        GimpDrawable;

typedef struct _GimpChannel         GimpChannel;
typedef         GimpChannel         Channel;            /* convenience */

typedef struct _GimpLayer           GimpLayer;
typedef struct _GimpLayerMask       GimpLayerMask;

typedef         GimpLayer           Layer;              /* convenience */
typedef         GimpLayerMask       LayerMask;          /* convenience */

typedef struct _GimpImage           GimpImage;
typedef         GimpImage           GImage;

typedef struct _GimpBrush	    GimpBrush;
typedef struct _GimpBrushList       GimpBrushList;
typedef struct _GimpBrushGenerated  GimpBrushGenerated;
typedef struct _GimpBrushPipe       GimpBrushPipe;


/*  widgets  */

typedef struct _HistogramWidget     HistogramWidget;


/*  other stuff  */

typedef struct _Argument            Argument;

typedef struct _BezierPoint         BezierPoint;
typedef struct _BezierSelect        BezierSelect;

typedef struct _BoundSeg            BoundSeg;

typedef struct _ColorNotebook       ColorNotebook;

typedef struct _DrawCore            DrawCore;

typedef struct _GDisplay            GDisplay;

typedef struct _GimpContextPreview  GimpContextPreview;

typedef struct _GimpHistogram       GimpHistogram;

typedef struct _GimpImageNewValues  GimpImageNewValues;

typedef struct _GimpLut             GimpLut;

typedef struct _GimpParasite        GimpParasite;

typedef struct _GimpProgress        GimpProgress;

typedef struct _GPattern            GPattern;

typedef struct _gradient_t          gradient_t;

typedef struct _Guide               Guide;

typedef         gpointer            ImageMap;

typedef struct _InfoDialog          InfoDialog;

typedef struct _PaintCore           PaintCore;

typedef struct _PaletteEntries      PaletteEntries;
typedef struct _PaletteEntry        PaletteEntry;

typedef struct _Path                Path;
typedef struct _PathPoint           PathPoint;
typedef struct _PathList            PathList;

typedef struct _PlugIn              PlugIn;
typedef struct _PlugInDef           PlugInDef;
typedef struct _PlugInProcDef       PlugInProcDef;

typedef struct _ParasiteList        ParasiteList;

typedef struct _PixelRegionIterator PixelRegionIterator;
typedef struct _PixelRegion         PixelRegion;
typedef struct _PixelRegionHolder   PixelRegionHolder;

typedef struct _ProcArg             ProcArg;
typedef struct _ProcRecord          ProcRecord;

typedef         guint32             Tattoo;

typedef struct _TempBuf             TempBuf;
typedef struct _TempBuf             MaskBuf;

typedef struct _Tile                Tile;
typedef struct _TileManager         TileManager;

typedef struct _Tool                Tool;
typedef struct _ToolInfo            ToolInfo;

typedef struct _ToolOptions         ToolOptions;

typedef struct _ScanConvertPoint    ScanConvertPoint;

typedef struct _Selection           Selection;

typedef struct _SelectionOptions    SelectionOptions;

typedef struct _TransformCore       TransformCore;


/*  some undo stuff  */

typedef struct _layer_undo          LayerUndo;

typedef struct _layer_mask_undo     LayerMaskUndo;

typedef struct _fs_to_layer_undo    FStoLayerUndo;

typedef         GSList              PathUndo;


/*  functions  */

typedef void (* TileValidateProc)     (TileManager *tm,
				       Tile        *tile);

typedef void (* ToolOptionsResetFunc) (void);

typedef void (* GimpProgressFunc)     (gint         ymin,
				       gint         ymax,
				       gint         curr_y,
				       gpointer     progress_data);

typedef void (* ImageMapApplyFunc)    (PixelRegion *srcPR,
				       PixelRegion *destPR,
				       gpointer     data);


#endif /* __APPTYPES_H__ */
