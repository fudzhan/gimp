# The GIMP -- an image manipulation program
# Copyright (C) 1999-2003 Manish Singh <yosh@gimp.org>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUTFILE ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# autogenerated by enumgen.pl

package Gimp::CodeGen::enums;

%enums = (
    GimpUnit =>
	{ contig => 1,
	  header => 'libgimpbase/gimpbasetypes.h',
	  symbols => [ qw(GIMP_UNIT_PIXEL GIMP_UNIT_INCH GIMP_UNIT_MM
			  GIMP_UNIT_POINT GIMP_UNIT_PICA GIMP_UNIT_END) ],
	  mapping => { GIMP_UNIT_PIXEL => '0',
		       GIMP_UNIT_INCH => '1',
		       GIMP_UNIT_MM => '2',
		       GIMP_UNIT_POINT => '3',
		       GIMP_UNIT_PICA => '4',
		       GIMP_UNIT_END => '5' }
	},
    GimpPDBArgType =>
	{ contig => 1,
	  header => 'libgimpbase/gimpbasetypes.h',
	  symbols => [ qw(GIMP_PDB_INT32 GIMP_PDB_INT16 GIMP_PDB_INT8
			  GIMP_PDB_FLOAT GIMP_PDB_STRING GIMP_PDB_INT32ARRAY
			  GIMP_PDB_INT16ARRAY GIMP_PDB_INT8ARRAY
			  GIMP_PDB_FLOATARRAY GIMP_PDB_STRINGARRAY
			  GIMP_PDB_COLOR GIMP_PDB_REGION GIMP_PDB_DISPLAY
			  GIMP_PDB_IMAGE GIMP_PDB_LAYER GIMP_PDB_CHANNEL
			  GIMP_PDB_DRAWABLE GIMP_PDB_SELECTION
			  GIMP_PDB_BOUNDARY GIMP_PDB_PATH GIMP_PDB_PARASITE
			  GIMP_PDB_STATUS GIMP_PDB_END) ],
	  mapping => { GIMP_PDB_INT32 => '0',
		       GIMP_PDB_INT16 => '1',
		       GIMP_PDB_INT8 => '2',
		       GIMP_PDB_FLOAT => '3',
		       GIMP_PDB_STRING => '4',
		       GIMP_PDB_INT32ARRAY => '5',
		       GIMP_PDB_INT16ARRAY => '6',
		       GIMP_PDB_INT8ARRAY => '7',
		       GIMP_PDB_FLOATARRAY => '8',
		       GIMP_PDB_STRINGARRAY => '9',
		       GIMP_PDB_COLOR => '10',
		       GIMP_PDB_REGION => '11',
		       GIMP_PDB_DISPLAY => '12',
		       GIMP_PDB_IMAGE => '13',
		       GIMP_PDB_LAYER => '14',
		       GIMP_PDB_CHANNEL => '15',
		       GIMP_PDB_DRAWABLE => '16',
		       GIMP_PDB_SELECTION => '17',
		       GIMP_PDB_BOUNDARY => '18',
		       GIMP_PDB_PATH => '19',
		       GIMP_PDB_PARASITE => '20',
		       GIMP_PDB_STATUS => '21',
		       GIMP_PDB_END => '22' }
	},
    GimpPDBProcType =>
	{ contig => 1,
	  header => 'libgimpbase/gimpbasetypes.h',
	  symbols => [ qw(GIMP_INTERNAL GIMP_PLUGIN GIMP_EXTENSION
			  GIMP_TEMPORARY) ],
	  mapping => { GIMP_INTERNAL => '0',
		       GIMP_PLUGIN => '1',
		       GIMP_EXTENSION => '2',
		       GIMP_TEMPORARY => '3' }
	},
    GimpPDBStatusType =>
	{ contig => 1,
	  header => 'libgimpbase/gimpbasetypes.h',
	  symbols => [ qw(GIMP_PDB_EXECUTION_ERROR GIMP_PDB_CALLING_ERROR
			  GIMP_PDB_PASS_THROUGH GIMP_PDB_SUCCESS
			  GIMP_PDB_CANCEL) ],
	  mapping => { GIMP_PDB_EXECUTION_ERROR => '0',
		       GIMP_PDB_CALLING_ERROR => '1',
		       GIMP_PDB_PASS_THROUGH => '2',
		       GIMP_PDB_SUCCESS => '3',
		       GIMP_PDB_CANCEL => '4' }
	},
    GimpMessageHandlerType =>
	{ contig => 1,
	  header => 'libgimpbase/gimpbasetypes.h',
	  symbols => [ qw(GIMP_MESSAGE_BOX GIMP_CONSOLE GIMP_ERROR_CONSOLE) ],
	  mapping => { GIMP_MESSAGE_BOX => '0',
		       GIMP_CONSOLE => '1',
		       GIMP_ERROR_CONSOLE => '2' }
	},
    GimpStackTraceMode =>
	{ contig => 1,
	  header => 'libgimpbase/gimpbasetypes.h',
	  symbols => [ qw(GIMP_STACK_TRACE_NEVER GIMP_STACK_TRACE_QUERY
			  GIMP_STACK_TRACE_ALWAYS) ],
	  mapping => { GIMP_STACK_TRACE_NEVER => '0',
		       GIMP_STACK_TRACE_QUERY => '1',
		       GIMP_STACK_TRACE_ALWAYS => '2' }
	},
    GimpLayerModeEffects =>
	{ contig => 1,
	  header => 'base/base-enums.h',
	  symbols => [ qw(GIMP_NORMAL_MODE GIMP_DISSOLVE_MODE
			  GIMP_BEHIND_MODE GIMP_MULTIPLY_MODE
			  GIMP_SCREEN_MODE GIMP_OVERLAY_MODE
			  GIMP_DIFFERENCE_MODE GIMP_ADDITION_MODE
			  GIMP_SUBTRACT_MODE GIMP_DARKEN_ONLY_MODE
			  GIMP_LIGHTEN_ONLY_MODE GIMP_HUE_MODE
			  GIMP_SATURATION_MODE GIMP_COLOR_MODE
			  GIMP_VALUE_MODE GIMP_DIVIDE_MODE GIMP_DODGE_MODE
			  GIMP_BURN_MODE GIMP_HARDLIGHT_MODE
			  GIMP_SOFTLIGHT_MODE GIMP_GRAIN_EXTRACT_MODE
			  GIMP_GRAIN_MERGE_MODE GIMP_COLOR_ERASE_MODE) ],
	  mapping => { GIMP_NORMAL_MODE => '0',
		       GIMP_DISSOLVE_MODE => '1',
		       GIMP_BEHIND_MODE => '2',
		       GIMP_MULTIPLY_MODE => '3',
		       GIMP_SCREEN_MODE => '4',
		       GIMP_OVERLAY_MODE => '5',
		       GIMP_DIFFERENCE_MODE => '6',
		       GIMP_ADDITION_MODE => '7',
		       GIMP_SUBTRACT_MODE => '8',
		       GIMP_DARKEN_ONLY_MODE => '9',
		       GIMP_LIGHTEN_ONLY_MODE => '10',
		       GIMP_HUE_MODE => '11',
		       GIMP_SATURATION_MODE => '12',
		       GIMP_COLOR_MODE => '13',
		       GIMP_VALUE_MODE => '14',
		       GIMP_DIVIDE_MODE => '15',
		       GIMP_DODGE_MODE => '16',
		       GIMP_BURN_MODE => '17',
		       GIMP_HARDLIGHT_MODE => '18',
		       GIMP_SOFTLIGHT_MODE => '19',
		       GIMP_GRAIN_EXTRACT_MODE => '20',
		       GIMP_GRAIN_MERGE_MODE => '21',
		       GIMP_COLOR_ERASE_MODE => '22' }
	},
    GimpTransferMode =>
	{ contig => 1,
	  header => 'base/base-enums.h',
	  symbols => [ qw(GIMP_SHADOWS GIMP_MIDTONES GIMP_HIGHLIGHTS) ],
	  mapping => { GIMP_SHADOWS => '0',
		       GIMP_MIDTONES => '1',
		       GIMP_HIGHLIGHTS => '2' }
	},
    GimpConvolutionType =>
	{ contig => 1,
	  header => 'base/base-enums.h',
	  symbols => [ qw(GIMP_NORMAL_CONVOL GIMP_ABSOLUTE_CONVOL
			  GIMP_NEGATIVE_CONVOL) ],
	  mapping => { GIMP_NORMAL_CONVOL => '0',
		       GIMP_ABSOLUTE_CONVOL => '1',
		       GIMP_NEGATIVE_CONVOL => '2' }
	},
    GimpChannelLutType =>
	{ contig => 1,
	  header => 'base/base-enums.h',
	  symbols => [ qw(GIMP_VALUE_LUT GIMP_RED_LUT GIMP_GREEN_LUT
			  GIMP_BLUE_LUT GIMP_ALPHA_LUT) ],
	  mapping => { GIMP_VALUE_LUT => '0',
		       GIMP_RED_LUT => '1',
		       GIMP_GREEN_LUT => '2',
		       GIMP_BLUE_LUT => '3',
		       GIMP_ALPHA_LUT => '4' }
	},
    GimpHueRange =>
	{ contig => 1,
	  header => 'base/base-enums.h',
	  symbols => [ qw(GIMP_ALL_HUES GIMP_RED_HUES GIMP_YELLOW_HUES
			  GIMP_GREEN_HUES GIMP_CYAN_HUES GIMP_BLUE_HUES
			  GIMP_MAGENTA_HUES) ],
	  mapping => { GIMP_ALL_HUES => '0',
		       GIMP_RED_HUES => '1',
		       GIMP_YELLOW_HUES => '2',
		       GIMP_GREEN_HUES => '3',
		       GIMP_CYAN_HUES => '4',
		       GIMP_BLUE_HUES => '5',
		       GIMP_MAGENTA_HUES => '6' }
	},
    GimpAddMaskType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_ADD_WHITE_MASK GIMP_ADD_BLACK_MASK
			  GIMP_ADD_ALPHA_MASK GIMP_ADD_ALPHA_MASK_TRANSFER
			  GIMP_ADD_SELECTION_MASK GIMP_ADD_COPY_MASK) ],
	  mapping => { GIMP_ADD_WHITE_MASK => '0',
		       GIMP_ADD_BLACK_MASK => '1',
		       GIMP_ADD_ALPHA_MASK => '2',
		       GIMP_ADD_ALPHA_MASK_TRANSFER => '3',
		       GIMP_ADD_SELECTION_MASK => '4',
		       GIMP_ADD_COPY_MASK => '5' }
	},
    GimpBlendMode =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_FG_BG_RGB_MODE GIMP_FG_BG_HSV_MODE
			  GIMP_FG_TRANSPARENT_MODE GIMP_CUSTOM_MODE) ],
	  mapping => { GIMP_FG_BG_RGB_MODE => '0',
		       GIMP_FG_BG_HSV_MODE => '1',
		       GIMP_FG_TRANSPARENT_MODE => '2',
		       GIMP_CUSTOM_MODE => '3' }
	},
    GimpBucketFillMode =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_FG_BUCKET_FILL GIMP_BG_BUCKET_FILL
			  GIMP_PATTERN_BUCKET_FILL) ],
	  mapping => { GIMP_FG_BUCKET_FILL => '0',
		       GIMP_BG_BUCKET_FILL => '1',
		       GIMP_PATTERN_BUCKET_FILL => '2' }
	},
    GimpChannelOps =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_CHANNEL_OP_ADD GIMP_CHANNEL_OP_SUBTRACT
			  GIMP_CHANNEL_OP_REPLACE GIMP_CHANNEL_OP_INTERSECT) ],
	  mapping => { GIMP_CHANNEL_OP_ADD => '0',
		       GIMP_CHANNEL_OP_SUBTRACT => '1',
		       GIMP_CHANNEL_OP_REPLACE => '2',
		       GIMP_CHANNEL_OP_INTERSECT => '3' }
	},
    GimpChannelType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_RED_CHANNEL GIMP_GREEN_CHANNEL
			  GIMP_BLUE_CHANNEL GIMP_GRAY_CHANNEL
			  GIMP_INDEXED_CHANNEL GIMP_ALPHA_CHANNEL) ],
	  mapping => { GIMP_RED_CHANNEL => '0',
		       GIMP_GREEN_CHANNEL => '1',
		       GIMP_BLUE_CHANNEL => '2',
		       GIMP_GRAY_CHANNEL => '3',
		       GIMP_INDEXED_CHANNEL => '4',
		       GIMP_ALPHA_CHANNEL => '5' }
	},
    GimpConvertDitherType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_NO_DITHER GIMP_FS_DITHER
			  GIMP_FSLOWBLEED_DITHER GIMP_FIXED_DITHER) ],
	  mapping => { GIMP_NO_DITHER => '0',
		       GIMP_FS_DITHER => '1',
		       GIMP_FSLOWBLEED_DITHER => '2',
		       GIMP_FIXED_DITHER => '3' }
	},
    GimpFillType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_FOREGROUND_FILL GIMP_BACKGROUND_FILL
			  GIMP_WHITE_FILL GIMP_TRANSPARENT_FILL GIMP_NO_FILL) ],
	  mapping => { GIMP_FOREGROUND_FILL => '0',
		       GIMP_BACKGROUND_FILL => '1',
		       GIMP_WHITE_FILL => '2',
		       GIMP_TRANSPARENT_FILL => '3',
		       GIMP_NO_FILL => '4' }
	},
    GimpGradientType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_GRADIENT_LINEAR GIMP_GRADIENT_BILINEAR
			  GIMP_GRADIENT_RADIAL GIMP_GRADIENT_SQUARE
			  GIMP_GRADIENT_CONICAL_SYMMETRIC
			  GIMP_GRADIENT_CONICAL_ASYMMETRIC
			  GIMP_GRADIENT_SHAPEBURST_ANGULAR
			  GIMP_GRADIENT_SHAPEBURST_SPHERICAL
			  GIMP_GRADIENT_SHAPEBURST_DIMPLED
			  GIMP_GRADIENT_SPIRAL_CLOCKWISE
			  GIMP_GRADIENT_SPIRAL_ANTICLOCKWISE) ],
	  mapping => { GIMP_GRADIENT_LINEAR => '0',
		       GIMP_GRADIENT_BILINEAR => '1',
		       GIMP_GRADIENT_RADIAL => '2',
		       GIMP_GRADIENT_SQUARE => '3',
		       GIMP_GRADIENT_CONICAL_SYMMETRIC => '4',
		       GIMP_GRADIENT_CONICAL_ASYMMETRIC => '5',
		       GIMP_GRADIENT_SHAPEBURST_ANGULAR => '6',
		       GIMP_GRADIENT_SHAPEBURST_SPHERICAL => '7',
		       GIMP_GRADIENT_SHAPEBURST_DIMPLED => '8',
		       GIMP_GRADIENT_SPIRAL_CLOCKWISE => '9',
		       GIMP_GRADIENT_SPIRAL_ANTICLOCKWISE => '10' }
	},
    GimpImageBaseType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_RGB GIMP_GRAY GIMP_INDEXED) ],
	  mapping => { GIMP_RGB => '0',
		       GIMP_GRAY => '1',
		       GIMP_INDEXED => '2' }
	},
    GimpImageType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_RGB_IMAGE GIMP_RGBA_IMAGE GIMP_GRAY_IMAGE
			  GIMP_GRAYA_IMAGE GIMP_INDEXED_IMAGE
			  GIMP_INDEXEDA_IMAGE) ],
	  mapping => { GIMP_RGB_IMAGE => '0',
		       GIMP_RGBA_IMAGE => '1',
		       GIMP_GRAY_IMAGE => '2',
		       GIMP_GRAYA_IMAGE => '3',
		       GIMP_INDEXED_IMAGE => '4',
		       GIMP_INDEXEDA_IMAGE => '5' }
	},
    GimpOrientationType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_ORIENTATION_HORIZONTAL
			  GIMP_ORIENTATION_VERTICAL GIMP_ORIENTATION_UNKNOWN) ],
	  mapping => { GIMP_ORIENTATION_HORIZONTAL => '0',
		       GIMP_ORIENTATION_VERTICAL => '1',
		       GIMP_ORIENTATION_UNKNOWN => '2' }
	},
    GimpRepeatMode =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_REPEAT_NONE GIMP_REPEAT_SAWTOOTH
			  GIMP_REPEAT_TRIANGULAR) ],
	  mapping => { GIMP_REPEAT_NONE => '0',
		       GIMP_REPEAT_SAWTOOTH => '1',
		       GIMP_REPEAT_TRIANGULAR => '2' }
	},
    GimpConvertPaletteType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_MAKE_PALETTE GIMP_REUSE_PALETTE
			  GIMP_WEB_PALETTE GIMP_MONO_PALETTE
			  GIMP_CUSTOM_PALETTE) ],
	  mapping => { GIMP_MAKE_PALETTE => '0',
		       GIMP_REUSE_PALETTE => '1',
		       GIMP_WEB_PALETTE => '2',
		       GIMP_MONO_PALETTE => '3',
		       GIMP_CUSTOM_PALETTE => '4' }
	},
    GimpMaskApplyMode =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_MASK_APPLY GIMP_MASK_DISCARD) ],
	  mapping => { GIMP_MASK_APPLY => '0',
		       GIMP_MASK_DISCARD => '1' }
	},
    GimpMergeType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_EXPAND_AS_NECESSARY GIMP_CLIP_TO_IMAGE
			  GIMP_CLIP_TO_BOTTOM_LAYER GIMP_FLATTEN_IMAGE) ],
	  mapping => { GIMP_EXPAND_AS_NECESSARY => '0',
		       GIMP_CLIP_TO_IMAGE => '1',
		       GIMP_CLIP_TO_BOTTOM_LAYER => '2',
		       GIMP_FLATTEN_IMAGE => '3' }
	},
    GimpOffsetType =>
	{ contig => 1,
	  header => 'core/core-enums.h',
	  symbols => [ qw(GIMP_OFFSET_BACKGROUND GIMP_OFFSET_TRANSPARENT) ],
	  mapping => { GIMP_OFFSET_BACKGROUND => '0',
		       GIMP_OFFSET_TRANSPARENT => '1' }
	},
    SizeType =>
	{ contig => 1,
	  header => 'core/core-types.h',
	  symbols => [ qw(GIMP_PIXELS GIMP_POINTS) ],
	  mapping => { GIMP_PIXELS => '0',
		       GIMP_POINTS => '1' }
	},
    GimpCloneType =>
	{ contig => 1,
	  header => 'paint/paint-enums.h',
	  symbols => [ qw(GIMP_IMAGE_CLONE GIMP_PATTERN_CLONE) ],
	  mapping => { GIMP_IMAGE_CLONE => '0',
		       GIMP_PATTERN_CLONE => '1' }
	},
    GimpDodgeBurnType =>
	{ contig => 1,
	  header => 'paint/paint-enums.h',
	  symbols => [ qw(GIMP_DODGE GIMP_BURN) ],
	  mapping => { GIMP_DODGE => '0',
		       GIMP_BURN => '1' }
	},
    GimpConvolveType =>
	{ contig => 1,
	  header => 'paint/paint-enums.h',
	  symbols => [ qw(GIMP_BLUR_CONVOLVE GIMP_SHARPEN_CONVOLVE) ],
	  mapping => { GIMP_BLUR_CONVOLVE => '0',
		       GIMP_SHARPEN_CONVOLVE => '1' }
	},
    GimpPaintApplicationMode =>
	{ contig => 1,
	  header => 'paint/paint-enums.h',
	  symbols => [ qw(GIMP_PAINT_CONSTANT GIMP_PAINT_INCREMENTAL) ],
	  mapping => { GIMP_PAINT_CONSTANT => '0',
		       GIMP_PAINT_INCREMENTAL => '1' }
	},
    GimpBrushApplicationMode =>
	{ contig => 1,
	  header => 'paint/paint-enums.h',
	  symbols => [ qw(GIMP_BRUSH_HARD GIMP_BRUSH_SOFT) ],
	  mapping => { GIMP_BRUSH_HARD => '0',
		       GIMP_BRUSH_SOFT => '1' }
	},
    GimpRunMode =>
	{ contig => 1,
	  header => 'plug-in/plug-in-types.h',
	  symbols => [ qw(GIMP_RUN_INTERACTIVE GIMP_RUN_NONINTERACTIVE
			  GIMP_RUN_WITH_LAST_VALS) ],
	  mapping => { GIMP_RUN_INTERACTIVE => '0',
		       GIMP_RUN_NONINTERACTIVE => '1',
		       GIMP_RUN_WITH_LAST_VALS => '2' }
	}
);

foreach $e (values %enums) {
    $e->{info} = "";
    foreach (@{$e->{symbols}}) {
	$e->{info} .= "$_ ($e->{mapping}->{$_}), "
    }
    $e->{info} =~ s/, $//;
}

1;
