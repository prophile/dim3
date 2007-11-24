# Microsoft Developer Studio Project File - Name="dim3Server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dim3Server - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dim3Server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dim3Server.mak" CFG="dim3Server - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dim3Server - Win32 Release" (based on "Win32 (x86) Console Application")
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
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\win32Includes\pthreads" /I "..\win32Includes\GLEW" /I "..\win32Includes\ZLib" /I "..\win32Includes\SDL" /I "..\win32Includes\OpenAL" /I "..\win32Includes\JS" /I "..\win32Includes\PNG" /I "..\..\dim3Server\Headers" /I "..\..\dim3BaseUtility\Headers" /I "..\..\dim3Common\Headers" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "D3_PCH" /YX"dim3server.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\..\dim3\dim3 Server.exe"
# Begin Target

# Name "dim3Server - Win32 Release"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "Headers"

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

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\GLU32.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\WSOCK32.LIB"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\OPENGL32.LIB"
# End Source File
# End Group
# Begin Group "pthreads"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32Includes\pthreads\pthread.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\pthreads\sched.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\pthreads\semaphore.h
# End Source File
# Begin Source File

SOURCE=..\win32Includes\pthreads\pthreadVC2.lib
# End Source File
# End Group
# Begin Group "GLEW"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32Includes\GLEW\glew.h
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
# End Group
# Begin Group "dim3 Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dim3BaseUtility\Release\dim3BaseUtility.lib
# End Source File
# End Group
# End Group
# Begin Group "dim3Server"

# PROP Default_Filter ""
# Begin Group "ServerHeaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Server\Headers\dim3server.h
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Headers\serverprivate.h
# End Source File
# End Group
# Begin Group "ServerSource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Server\Sources\broadcast.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\clients.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\host.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\main.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\players.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\server_query.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\server_query_info.c
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\dim3Server\Sources\status.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\ui_mac.c
# End Source File
# Begin Source File

SOURCE=..\..\dim3Server\Sources\ui_win32.c
# End Source File
# End Group
# End Group
# End Target
# End Project
