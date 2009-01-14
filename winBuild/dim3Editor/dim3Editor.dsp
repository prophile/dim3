# Microsoft Developer Studio Project File - Name="dim3Editor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=dim3Editor - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dim3Editor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dim3Editor.mak" CFG="dim3Editor - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dim3Editor - Win32 Release" (based on "Win32 (x86) Application")
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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Z7 /O2 /I "..\win32Includes\pthreads" /I "..\win32Includes\GLEW" /I "..\win32Includes\ZLib" /I "..\win32Includes\SDL" /I "..\win32Includes\OpenAL" /I "..\win32Includes\JS" /I "..\win32Includes\PNG" /I "..\..\dim3Editor\Headers" /I "..\..\dim3MapUtility\Headers" /I "..\..\dim3ModelUtility\Headers" /I "..\..\dim3BaseUtility\Headers" /I "..\..\dim3Common\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "D3_PCH" /YX"dim3Editor.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\..\dim3\dim3 Editor.exe"
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "dim3Editor - Win32 Release"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Common\Headers\baseutility.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Common\Headers\maputility.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Common\Headers\modelutility.h
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

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\GLU32.LIB"
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
# Begin Source File

SOURCE=..\dim3ModelUtility\Release\dim3ModelUtility.lib
# End Source File
# Begin Source File

SOURCE=..\dim3MapUtility\Release\dim3MapUtility.lib
# End Source File
# End Group
# End Group
# Begin Group "dim3Editor"

# PROP Default_Filter ""
# Begin Group "EditorHeaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Editor\Headers\common_view.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Headers\dim3editor.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Headers\editor_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Headers\interface.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Headers\walk_view.h
# End Source File
# End Group
# Begin Group "EditorSources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_click_drag.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_click_piece.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_click_texture.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_compass.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_draw_piece.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_draw_select.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_models.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_movement.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Editor\Code\walk_view_swap.c
# End Source File
# End Group
# Begin Group "EditorWin32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32Code\dim3Editor.c
# End Source File
# End Group
# End Group
# End Target
# End Project
