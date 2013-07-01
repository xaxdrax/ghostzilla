@if NOT "%MOZILLA_SOURCE_DIR%" == "" goto dirSetOK
@echo [ghzil] Step 3: ERROR: RUN step1-setEnvironment.bat first!
@goto end
:dirSetOK

@echo [ghzil] Step 3: building Ghostzilla DLL

@set DISTDIR=%MOZILLA_SOURCE_DIR%\dist

@set GHCDOPTION=

@set CCDEBUGOPTIONS=
@set LINKDEBUGOPTIONS=
@rem set CCDEBUGOPTIONS= -DDEBUG -Fddll\obj\ghostzilla.pdb -MDd -D_DEBUG  -Zi
@rem set LINKDEBUGOPTIONS=/PDB:dll\obj\ghostzilla.pdb /DEBUG /DEBUGTYPE:CV

@if not exist %DISTDIR%\bin\mozilla mkdir %DISTDIR%\bin\mozilla
@if not exist dll\obj mkdir dll\obj

cl %GHCDOPTION% %CCDEBUGOPTIONS% -Fodll\obj\ghostzilla.obj -c -W3 -nologo -Gy -DMOZILLA_CLIENT -D_WINDOWS=1 -D_WIN32=1 -DWIN32=1 -DXP_PC=1 -DXP_WIN=1 -DXP_WIN32=1 -DHW_THREADS=1 -DWINVER=0x400 -DMSVC4=1 -DSTDC_HEADERS=1 -DNO_X11=1 -D_X86_=1 -DD_INO=d_ino dll\ghostzilla.cpp
@if errorlevel 1 goto err

lib -NOLOGO -OUT:dll\obj\ghostzilla.lib dll\obj\ghostzilla.obj
@if errorlevel 1 goto err

link /NOLOGO /DLL /OUT:dll\obj\ghostzilla.dll %LINKDEBUGOPTIONS% /SUBSYSTEM:WINDOWS dll\obj\ghostzilla.obj kernel32.lib user32.lib gdi32.lib winmm.lib wsock32.lib advapi32.lib comctl32.lib comdlg32.lib uuid.lib ole32.lib shell32.lib oleaut32.lib
@if errorlevel 1 goto err


copy dll\obj\ghostzilla.dll %DISTDIR%\bin
@if errorlevel 1 goto err

copy dll\obj\ghostzilla.lib %DISTDIR%\lib
@if errorlevel 1 goto err

@echo [ghzil] Step 3: building Ghostzilla DLL complete.
@goto end

:err
@echo.
@echo [ghzil] Step 3: error building and copying Ghostzilla DLL
@echo.
:end