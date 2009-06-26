# Microsoft Developer Studio Project File - Name="dim3MapUtility" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dim3MapUtility - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dim3MapUtility.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dim3MapUtility.mak" CFG="dim3MapUtility - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dim3MapUtility - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\win32Includes\GLEW" /I "..\win32Includes\ZLib" /I "..\win32Includes\SDL" /I "..\win32Includes\JS" /I "..\win32Includes\PNG" /I "..\..\dim3MapUtility\Headers" /I "..\..\dim3Common\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "D3_PCH" /YX"dim3maputility.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\win32Includes\dim3\\dim3MapUtility.lib"
# Begin Target

# Name "dim3MapUtility - Win32 Release"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Common\Headers\baseutility.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Common\Headers\mapimport.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Common\Headers\maputility.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Common\Headers\os_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Common\Headers\win32_os_defs.h
# End Source File
# End Group
# Begin Group "dim3 Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32Includes\dim3\dim3BaseUtility.lib
# End Source File
# End Group
# End Group
# Begin Group "dim3MapUtility"

# PROP Default_Filter ""
# Begin Group "win32Code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32Code\dim3MapUtility.def
# End Source File
# End Group
# Begin Group "MapHeaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3MapUtility\Headers\dim3maputility.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Headers\mapprivate.h
# End Source File
# End Group
# Begin Group "MapSources"

# PROP Default_Filter ""
# Begin Group "AutoGenerate"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate_blocking.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate_door_ramp_step.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate_light_spot.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate_second_story.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate_utility.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read_v1.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read_v1_curves.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read_v2.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read_v3.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_texture.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_write.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\groups.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\map_frames.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\map_liquid.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\map_liquid_update.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\map_main.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\map_mesh.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\map_mesh_update.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\movements.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\nodes.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\prepare.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\spots.c
# End Source File
# End Group
# End Group
# End Target
# End Project
