/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Application/UI defines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// menu definitions
//

#define app_menu_apple						128
#define app_menu_file						129
#define app_menu_edit						130
#define app_menu_view						131
#define app_menu_map						132
#define app_menu_mesh						133
#define app_menu_polygon					134
#define app_menu_vertex						135
#define app_menu_groups						136

#define kCommandAbout						FOUR_CHAR_CODE('abot')
#define kCommandPreference					FOUR_CHAR_CODE('pref')

#define kCommandNew							FOUR_CHAR_CODE('new ')
#define kCommandOpen						FOUR_CHAR_CODE('open')
#define kCommandClose						FOUR_CHAR_CODE('clse')
#define kCommandSave						FOUR_CHAR_CODE('save')
#define kCommandAutoGenerateMap				FOUR_CHAR_CODE('auto')

#define kCommandUndo						FOUR_CHAR_CODE('undo')
#define kCommandDuplicate					FOUR_CHAR_CODE('pdup')
#define kCommandDelete						FOUR_CHAR_CODE('pdel')

#define kCommandView3Panel					FOUR_CHAR_CODE('vw01')
#define kCommandView4Panel					FOUR_CHAR_CODE('vw02')
#define kCommandViewTopOnly					FOUR_CHAR_CODE('vw03')
#define kCommandViewForwardOnly				FOUR_CHAR_CODE('vw04')
#define kCommandViewPerspective				FOUR_CHAR_CODE('vwps')
#define kCommandViewOrtho					FOUR_CHAR_CODE('vwot')
#define kCommandViewUVLayer1				FOUR_CHAR_CODE('uvl1')
#define kCommandViewUVLayer2				FOUR_CHAR_CODE('uvl2')
#define kCommandViewUVLayer3				FOUR_CHAR_CODE('uvl3')

#define kCommandMapSettings					FOUR_CHAR_CODE('mset')
#define kCommandMapRaiseY					FOUR_CHAR_CODE('mrsy')
#define kCommandMapLowerY					FOUR_CHAR_CODE('mlwy')
#define kCommandMapCenter					FOUR_CHAR_CODE('mctr')
#define kCommandMapResetUV					FOUR_CHAR_CODE('mpuv')
#define kCommandMapOptimize					FOUR_CHAR_CODE('mopt')
#define kCommandRun							FOUR_CHAR_CODE('mrun')

#define kCommandMeshCombine					FOUR_CHAR_CODE('pcmb')
#define kCommandMeshSplit					FOUR_CHAR_CODE('pspt')
#define kCommandMeshTesselate				FOUR_CHAR_CODE('ptsl')
#define kCommandMeshResize					FOUR_CHAR_CODE('mrsz')
#define kCommandMeshReposition				FOUR_CHAR_CODE('mrps')
#define kCommandMeshFlipX					FOUR_CHAR_CODE('pfpx')
#define kCommandMeshFlipY					FOUR_CHAR_CODE('pfpy')
#define kCommandMeshFlipZ					FOUR_CHAR_CODE('pfpz')
#define kCommandMeshRotateX					FOUR_CHAR_CODE('prtx')
#define kCommandMeshRotateY					FOUR_CHAR_CODE('prty')
#define kCommandMeshRotateZ					FOUR_CHAR_CODE('prtz')
#define kCommandMeshFreeRotate				FOUR_CHAR_CODE('prtf')
#define kCommandMeshRaiseY					FOUR_CHAR_CODE('prsy')
#define kCommandMeshLowerY					FOUR_CHAR_CODE('plry')
#define kCommandMeshSelectAllPoly			FOUR_CHAR_CODE('msll')
#define kCommandMeshSnapToGrid				FOUR_CHAR_CODE('msnp')
#define kCommandMeshSnapClosestVertex		FOUR_CHAR_CODE('mscv')
#define kCommandMeshResetUV					FOUR_CHAR_CODE('mruv')
#define kCommandMeshWholeUV					FOUR_CHAR_CODE('mrwv')
#define kCommandMeshSingleUV				FOUR_CHAR_CODE('mrss')

#define kCommandPolygonHole					FOUR_CHAR_CODE('fhl1')
#define kCommandPolygonSnapToGrid			FOUR_CHAR_CODE('psnp')
#define kCommandPolygonRotateUV				FOUR_CHAR_CODE('pyru')
#define kCommandPolygonFlipU				FOUR_CHAR_CODE('pyfu')
#define kCommandPolygonFlipV				FOUR_CHAR_CODE('pyfv')
#define kCommandPolygonResetUV				FOUR_CHAR_CODE('pyuv')
#define kCommandPolygonWholeUV				FOUR_CHAR_CODE('pywv')
#define kCommandPolygonSingleUV				FOUR_CHAR_CODE('pyss')

#define kCommandVertexSnapToGrid			FOUR_CHAR_CODE('vsnp')

#define kCommandGroups						FOUR_CHAR_CODE('grrp')
#define kCommandGroupMovements				FOUR_CHAR_CODE('mmov')
