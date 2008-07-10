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
#define app_menu_portal						133
#define app_menu_pieces						134
#define app_menu_groups						137

#define kCommandAbout						FOUR_CHAR_CODE('abot')

#define kCommandNew							FOUR_CHAR_CODE('new ')
#define kCommandOpen						FOUR_CHAR_CODE('open')
#define kCommandClose						FOUR_CHAR_CODE('clse')
// #define kCommandImportOBJ					FOUR_CHAR_CODE('iobj')
#define kCommandImportHeightMap				FOUR_CHAR_CODE('ihmp')
#define kCommandAutoGenerateMap				FOUR_CHAR_CODE('atgn')
#define kCommandSave						FOUR_CHAR_CODE('save')

#define kCommandUndo						FOUR_CHAR_CODE('undo')
#define kCommandCut							FOUR_CHAR_CODE('cut ')
#define kCommandCopy						FOUR_CHAR_CODE('copy')
#define kCommandPaste						FOUR_CHAR_CODE('pste')
#define kCommandClear						FOUR_CHAR_CODE('cler')

#define kCommandView3Panel					FOUR_CHAR_CODE('vw01')
#define kCommandView4Panel					FOUR_CHAR_CODE('vw02')
#define kCommandViewPortal					FOUR_CHAR_CODE('vw03')
#define kCommandViewSitePath				FOUR_CHAR_CODE('vw04')
#define kCommandViewTopOnly					FOUR_CHAR_CODE('vw05')
#define kCommandViewForwardOnly				FOUR_CHAR_CODE('vw06')
#define kCommandViewPerspective				FOUR_CHAR_CODE('vwps')
#define kCommandViewOrtho					FOUR_CHAR_CODE('vwot')

#define kCommandMapSettings					FOUR_CHAR_CODE('mset')
#define kCommandMapCounts					FOUR_CHAR_CODE('cset')
#define kCommandMapMedia					FOUR_CHAR_CODE('medi')
#define kCommandAmbientLightSoundSettings	FOUR_CHAR_CODE('mamb')
#define kCommandSkySettings					FOUR_CHAR_CODE('msky')
#define kCommandRainSettings				FOUR_CHAR_CODE('mrin')
#define kCommandFogSettings					FOUR_CHAR_CODE('mfog')
#define kCommandAutoGenerateSightPaths		FOUR_CHAR_CODE('agsp')
#define kCommandRun							FOUR_CHAR_CODE('mrun')

#define kCommandPortalSettings				FOUR_CHAR_CODE('rset')
#define kCommandPortalDuplicate				FOUR_CHAR_CODE('rdup')
#define kCommandPortalDelete				FOUR_CHAR_CODE('rdel')
#define kCommandPortalGoToTop				FOUR_CHAR_CODE('topp')
#define kCommandPortalGoToBottom			FOUR_CHAR_CODE('botp')
#define kCommandPortalGoToSelection			FOUR_CHAR_CODE('selp')
#define kCommandClearPath					FOUR_CHAR_CODE('clrp')

#define kCommandMeshCombine					FOUR_CHAR_CODE('pcmb')
#define kCommandMeshTesselate				FOUR_CHAR_CODE('ptsl')
#define kCommandMeshAddLibrary				FOUR_CHAR_CODE('padd')
#define kCommandPieceDuplicate				FOUR_CHAR_CODE('pdup')
#define kCommandPieceDelete					FOUR_CHAR_CODE('pdel')
#define kCommandPieceMove					FOUR_CHAR_CODE('pmov')
#define kCommandPieceDuplicateMove			FOUR_CHAR_CODE('pdmv')

#define kCommandMeshResize					FOUR_CHAR_CODE('mrsz')
#define kCommandMeshFlipX					FOUR_CHAR_CODE('pfpx')
#define kCommandMeshFlipY					FOUR_CHAR_CODE('pfpy')
#define kCommandMeshFlipZ					FOUR_CHAR_CODE('pfpz')
#define kCommandMeshRotateX					FOUR_CHAR_CODE('prtx')
#define kCommandMeshRotateY					FOUR_CHAR_CODE('prty')
#define kCommandMeshRotateZ					FOUR_CHAR_CODE('prtz')
#define kCommandMeshFreeRotate				FOUR_CHAR_CODE('prtf')
#define kCommandMeshRaiseY					FOUR_CHAR_CODE('prsy')
#define kCommandMeshLowerY					FOUR_CHAR_CODE('plry')
#define kCommandMeshSnapToGrid				FOUR_CHAR_CODE('msnp')
#define kCommandMeshSnapClosestVertex		FOUR_CHAR_CODE('mscv')
#define kCommandMeshResetUV					FOUR_CHAR_CODE('mruv')

#define kCommandPolygonHole1				FOUR_CHAR_CODE('fhl1')
#define kCommandPolygonSnapToGrid			FOUR_CHAR_CODE('psnp')
#define kCommandPolygonRotateUV				FOUR_CHAR_CODE('pyru')
#define kCommandPolygonResetUV				FOUR_CHAR_CODE('pyuv')

#define kCommandVertexSnapToGrid			FOUR_CHAR_CODE('vsnp')

#define kCommandGroupAdd					FOUR_CHAR_CODE('gadd')
#define kCommandGroupDelete					FOUR_CHAR_CODE('gdel')
#define kCommandGroupClear					FOUR_CHAR_CODE('gclr')
#define kCommandGroupMovements				FOUR_CHAR_CODE('mmov')

#define kCommandMapRaiseY					FOUR_CHAR_CODE('mrsy')
#define kCommandMapLowerY					FOUR_CHAR_CODE('mlwy')
#define kCommandMapResetUV					FOUR_CHAR_CODE('mpuv')

#define kCommandPortalResize				FOUR_CHAR_CODE('rsiz')
#define kCommandPortalFlipHorizontal		FOUR_CHAR_CODE('flph')
#define kCommandPortalFlipVertical			FOUR_CHAR_CODE('flpv')
#define kCommandPortalRotate				FOUR_CHAR_CODE('rot ')
#define kCommandPortalRaiseY				FOUR_CHAR_CODE('rrsy')
#define kCommandPortalLowerY				FOUR_CHAR_CODE('rlry')
#define kCommandPortalResetUV				FOUR_CHAR_CODE('pruv')

#define kCommandPortalSplitHorizontal		FOUR_CHAR_CODE('rshz')
#define kCommandPortalSplitVertical			FOUR_CHAR_CODE('rsvt')

//
// views
//

#define vw_3_panel				0
#define vw_4_panel				1
#define vw_portal_only			2
#define vw_site_path_only		3
#define vw_top_only				4
#define vw_forward_only			5

//
// perspective
//

#define ps_perspective			0
#define ps_ortho				1

//
// movement directions
//

#define vm_dir_forward			0
#define vm_dir_side				1
#define vm_dir_top				2

//
// keyboard panel focus
//

#define kf_panel_forward		0
#define kf_panel_side			1
#define kf_panel_top			2
#define kf_panel_walk			3

//
// vertex modes
//

#define vertex_mode_none		0
#define vertex_mode_lock		1
#define vertex_mode_snap		2

//
// drag modes
//

#define drag_mode_mesh			0
#define drag_mode_polygon		1
#define drag_mode_vertex		2

//
// grid modes
//

#define grid_mode_none			0
#define grid_mode_small			1
#define grid_mode_medium		2
#define grid_mode_large			3

//
// selection types
//

#define mesh_piece				100
#define liquid_piece			101
#define node_piece				102
#define spot_piece				103
#define scenery_piece			104
#define light_piece				105
#define sound_piece				106
#define particle_piece			107

//
// magnify factors
//

#define magnify_factor_min		4
#define magnify_factor_max		210

//
// fovs
//

#define walk_view_forward_fov	45
#define walk_view_side_fov		45
#define top_view_fov			30

//
// walk view opengl settings
//

#define walk_view_near_z		(6*map_enlarge)
#define walk_view_far_z			(2000*map_enlarge)
#define walk_view_near_offset	(3*map_enlarge)

#define walk_view_handle_size	8.0f

//
// draw passing struct
//

typedef struct		{
						int						clip_y,portal_y;
						Rect					box;
						d3pnt					cpt;
						d3ang					ang;
						float					fov;
						bool					mesh_only,draw_portal,ignore_site_path,draw_light_circle,
												swap_on,clip_on;
					} editor_3D_view_setup;

