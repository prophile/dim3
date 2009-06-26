# Microsoft Developer Studio Project File - Name="dim3BaseUtility" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dim3BaseUtility - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dim3BaseUtility.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dim3BaseUtility.mak" CFG="dim3BaseUtility - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dim3BaseUtility - Win32 Release" (based on "Win32 (x86) Static Library")
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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\win32Includes\GLEW" /I "..\win32Includes\ZLib" /I "..\win32Includes\SDL" /I "..\win32Includes\JS" /I "..\win32Includes\PNG" /I "..\..\dim3BaseUtility\Headers" /I "..\..\dim3Common\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "D3_PCH" /YX"dim3BaseUtility.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\win32Includes\dim3\dim3BaseUtility.lib"
# Begin Target

# Name "dim3BaseUtility - Win32 Release"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Common\Headers\baseutility.h
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

SOURCE=..\win32Includes\GLEW\glew32s.lib
# End Source File
# End Group
# Begin Group "png"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32Includes\PNG\png.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\PNG\libpng.lib
# End Source File
# End Group
# End Group
# Begin Group "dim3BaseUtility"

# PROP Default_Filter ""
# Begin Group "win32Code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32Code\dim3BaseUtility.def
# End Source File
# End Group
# Begin Group "BaseHeaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Headers\baseprivate.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Headers\dim3baseutility.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\acceleration.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\angles.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\area.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\clip.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\distance.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\fast_trig.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\lines.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\matrix.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\polygons.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\random.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\rotations.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\strings.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\time.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Math\vector.c
# End Source File
# End Group
# Begin Group "File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_File\file_paths.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_File\file_search.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_File\zip.c
# End Source File
# End Group
# Begin Group "XML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Source_XML\xml.c
# End Source File
# End Group
# Begin Group "Bitmap"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\bitmap_main.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\bitmap_movie.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\pngutility.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\textures.c
# End Source File
# End Group
# End Group
# End Target
# End Project
