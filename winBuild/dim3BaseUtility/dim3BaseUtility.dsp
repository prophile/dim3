# Microsoft Developer Studio Project File - Name="dim3BaseUtility" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

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
!MESSAGE "dim3BaseUtility - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DIM3BASEUTILITY_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Z7 /O2 /I "..\win32Includes\pthreads" /I "..\win32Includes\GLEW" /I "..\win32Includes\ZLib" /I "..\win32Includes\SDL" /I "..\win32Includes\OpenAL" /I "..\win32Includes\JS" /I "..\win32Includes\PNG" /I "..\..\dim3BaseUtility\Headers" /I "..\..\dim3Common\Headers" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "D3_PCH" /YX"dim3baseutility.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\dim3\dim3BaseUtility.dll"
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

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\OPENGL32.LIB"
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
# Begin Group "png"

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
# End Group
# Begin Group "dim3BaseUtility"

# PROP Default_Filter ""
# Begin Group "win32Code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32Code\dim3BaseUtility.c
# End Source File
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

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\glow_map.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\normal_map.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\pngutility.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\reduce.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\shaders.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Bitmap\textures.c
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Network\network.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Network\queue.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Network\socket.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Network\socket_udp.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3BaseUtility\Sources_Network\xml_network.c
# End Source File
# End Group
# End Group
# End Target
# End Project
