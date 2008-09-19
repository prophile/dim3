/*********************************************************************
*
* dim3 Animator
* By Brian Barnes
* Application Definitions
*
**********************************************************************/

//
// inteface defs
//

#define tool_button_size				36
#define tool_count						11

#define model_view_min_size				512

#define texture_palette_height			64
#define texture_palette_row_count		2
#define texture_palette_texture_size	32

#define info_palette_height				14

//
// Menus
//

#define applemenu						128
#define filemenu						129
#define editmenu						130
#define modelmenu						131
#define viewmenu						132
#define meshmenu						133
#define vertexmenu						134
#define bonemenu						135
#define posemenu						136
#define animatemenu						137
#define hitboxmenu						138

#define modeltransformmenu				141
#define meshimportmenu					140
#define meshtransformmenu				141
#define poseblendingmenu				142

#define kCommandAbout					FOUR_CHAR_CODE('abot')

#define kCommandNew						FOUR_CHAR_CODE('newB')
#define kCommandOpen					FOUR_CHAR_CODE('opnB')
#define kCommandClose					FOUR_CHAR_CODE('clse')
#define kCommandSave					FOUR_CHAR_CODE('savB')

#define kCommandUndo					FOUR_CHAR_CODE('undo')

#define kCommandSettings				FOUR_CHAR_CODE('sets')
#define kCommandSpecialBones			FOUR_CHAR_CODE('spbn')
#define kCommandPrepareModel			FOUR_CHAR_CODE('pmdl')
#define kCommandCalcBoxes				FOUR_CHAR_CODE('cbox')
#define kCommandCalcNormals				FOUR_CHAR_CODE('cnrm')
#define kCommandScaleAll				FOUR_CHAR_CODE('scal')
#define kCommandFlipXAll				FOUR_CHAR_CODE('fxal')
#define kCommandFlipZAll				FOUR_CHAR_CODE('fzal')
#define kCommandFlipYAll				FOUR_CHAR_CODE('fyal')
#define kCommandSwapXZAll				FOUR_CHAR_CODE('spal')
#define kCommandSwapYZAll				FOUR_CHAR_CODE('syal')
#define kCommandCenterXZAll				FOUR_CHAR_CODE('ctal')
#define kCommandFloorYAll				FOUR_CHAR_CODE('fral')

#define kCommandFront					FOUR_CHAR_CODE('vfnt')
#define kCommandLeft					FOUR_CHAR_CODE('vlft')
#define kCommandRight					FOUR_CHAR_CODE('vrgt')
#define kCommandBack					FOUR_CHAR_CODE('vbck')
#define kCommandTop						FOUR_CHAR_CODE('vtop')
#define kCommandBottom					FOUR_CHAR_CODE('vbtm')

#define kCommandNewMesh					FOUR_CHAR_CODE('nmsh')
#define kCommandDuplicateMesh			FOUR_CHAR_CODE('dpmh')
#define kCommandCopyMesh				FOUR_CHAR_CODE('cmsh')
#define kCommandDeleteMesh				FOUR_CHAR_CODE('dmsh')
#define kCommandImportOBJ				FOUR_CHAR_CODE('opOB')
#define kCommandImportLWO				FOUR_CHAR_CODE('opLW')
#define kCommandImportC4DXML			FOUR_CHAR_CODE('opC4')
#define kCommandInsertXML				FOUR_CHAR_CODE('opDM')
#define kCommandScale					FOUR_CHAR_CODE('scle')
#define kCommandFlipX					FOUR_CHAR_CODE('flpx')
#define kCommandFlipZ					FOUR_CHAR_CODE('flpz')
#define kCommandFlipY					FOUR_CHAR_CODE('flpy')
#define kCommandFlipU					FOUR_CHAR_CODE('flpu')
#define kCommandFlipV					FOUR_CHAR_CODE('flpv')
#define kCommandSwapXZ					FOUR_CHAR_CODE('spxz')
#define kCommandSwapYZ					FOUR_CHAR_CODE('spyz')
#define kCommandCenterXZ				FOUR_CHAR_CODE('ctxz')
#define kCommandFloorY					FOUR_CHAR_CODE('flry')

#define kCommandVertexSelectAll			FOUR_CHAR_CODE('vsla')
#define kCommandVertexSelectNotAttached	FOUR_CHAR_CODE('vsna')
#define kCommandVertexNudge				FOUR_CHAR_CODE('vnud')
#define kCommandVertexScale				FOUR_CHAR_CODE('vscl')
#define kCommandVertexRotate			FOUR_CHAR_CODE('vrot')
#define kCommandVertexInvertNormals		FOUR_CHAR_CODE('vinm')
#define kCommandVertexHideSelected		FOUR_CHAR_CODE('vhsl')
#define kCommandVertexHideNonSelected	FOUR_CHAR_CODE('vhns')
#define kCommandVertexShowAll			FOUR_CHAR_CODE('vsal')
#define kCommandVertexDelete			FOUR_CHAR_CODE('vdel')

#define kCommandNewBone					FOUR_CHAR_CODE('nbne')
#define kCommandSetBone					FOUR_CHAR_CODE('sbn2')
#define kCommandNudgeBone				FOUR_CHAR_CODE('bnud')
#define kCommandDeleteBone				FOUR_CHAR_CODE('dbne')
#define kCommandSelectVertexNearBone	FOUR_CHAR_CODE('svbn')

#define kCommandNewPose					FOUR_CHAR_CODE('npse')
#define kCommandDupPose					FOUR_CHAR_CODE('dpse')
#define kCommandPreviousPose			FOUR_CHAR_CODE('prvp')
#define kCommandNextPose				FOUR_CHAR_CODE('nxtp')
#define kCommandClearPose				FOUR_CHAR_CODE('cpse')
#define kCommandDeletePose				FOUR_CHAR_CODE('kpse')
#define kCommandGoToBoneMoveParent		FOUR_CHAR_CODE('pmbn')

#define kCommandBlendSetSkipAll			FOUR_CHAR_CODE('bsas')
#define kCommandBlendSetSkipNone		FOUR_CHAR_CODE('bsns')
#define kCommandBlendFlipSkip			FOUR_CHAR_CODE('bfsk')

#define kCommandNewAnimate				FOUR_CHAR_CODE('nani')
#define kCommandDupAnimate				FOUR_CHAR_CODE('dani')
#define kCommandDeleteAnimate			FOUR_CHAR_CODE('kani')
#define kCommandResetTimeAnimate		FOUR_CHAR_CODE('artm')
#define kCommandPlayAnimate				FOUR_CHAR_CODE('plya')
#define kCommandPlayBlendAnimate		FOUR_CHAR_CODE('plyb')

#define kCommandNewHitBox				FOUR_CHAR_CODE('nhbx')
#define kCommandDeleteHitBox			FOUR_CHAR_CODE('dhbx')

//
// Misc UI Constants
//

#define kMeshNameDBColumn				1050

#define kVertexPosDBColumn				1060
#define kVertexBoneDBColumn				1061

#define kBoneTagDBColumn				1070
#define kBonePosDBColumn				1071

#define kPoseNameDBColumn				1080
#define kPoseBoneDBColumn				1081
#define kPoseBoneMoveDBColumn			1082
#define kPoseBoneAccDBColumn			1083
#define kPoseBoneBlendDBColumn			1084

#define kAnimateNameDBColumn			1090
#define kAnimatePoseDBColumn			1091
#define kAnimateSwayMoveDBColumn		1092
#define kAnimateOtherDBColumn			1093

#define kHitBoxNameDBColumn				1100

//
// Draw Types
//

#define dt_model						0
#define dt_mesh							1
#define dt_bones						2
#define dt_mesh_bones					3

//
// Drag Modes
//

#define drag_bone_mode_rotate			0
#define drag_bone_mode_stretch			1

//
// Drag Handle Types
//

#define drag_handle_none				-1
#define drag_handle_x					0
#define drag_handle_y					1
#define drag_handle_z					2

//
// import Settings
//

#define import_scale_factor				100.0f
