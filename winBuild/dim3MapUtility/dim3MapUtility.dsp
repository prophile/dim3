# Microsoft Developer Studio Project File - Name="dim3MapUtility" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

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
!MESSAGE "dim3MapUtility - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
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
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DIM3MAPUTILITY_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\win32Includes\pthreads" /I "..\win32Includes\GLEW" /I "..\win32Includes\ZLib" /I "..\win32Includes\SDL" /I "..\win32Includes\OpenAL" /I "..\win32Includes\JS" /I "..\win32Includes\PNG" /I "..\..\dim3MapUtility\Headers" /I "..\..\dim3Common\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "D3_PCH" /YX"dim3MapUtility.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\dim3\dim3MapUtility.dll"
# Begin Target

# Name "dim3MapUtility - Win32 Release"
# Begin Group "common"

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
# Begin Group "OS Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\OPENGL32.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\GLU32.LIB"
# End Source File
# End Group
# Begin Group "pthreads"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32Includes\pthreads\pthread.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\pthreads\pthreadVC2.lib
# End Source File
# End Group
# Begin Group "glew"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32Includes\GLEW\glew.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\GLEW\wglew.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\GLEW\glew32.lib
# End Source File
# End Group
# Begin Group "PNG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32Includes\PNG\png.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\ZLib\zlib.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\PNG\libpng.lib
# End Source File
# Begin Source File

SOURCE=..\win32Includes\ZLib\zdll.lib
# End Source File
# End Group
# Begin Group "dim3 Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dim3BaseUtility\Release\dim3BaseUtility.lib
# End Source File
# End Group
# End Group
# Begin Group "dim3MapUtility"

# PROP Default_Filter ""
# Begin Group "win32Code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32Code\dim3MapUtility.c
# End Source File
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
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate_blocking.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\auto_generate_utility.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read_v1.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_read_v2.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\file_map_shader.c
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

SOURCE=..\..\dim3MapUtility\Sources\map_main.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\movements.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\nodes.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_changes.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_light_spot.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_liquid.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_liquid_update.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_mesh.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_mesh_update.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_segment_list.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_sight_path.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\portal_vertex_list.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\prepare.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\prepare_curve.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\prepare_polygon.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\primitives.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\segments.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3MapUtility\Sources\spots.c
# End Source File
# End Group
# End Group
# End Target
# End Project
