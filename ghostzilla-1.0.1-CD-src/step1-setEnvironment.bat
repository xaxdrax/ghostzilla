; you MUST edit these variables!
set MOZILLA_SOURCE_DIR=c:\MyWork\ghostzilla\src-1.0.1-open\mozilla
set MOZ_TOOLS=c:\MyWork\ghostzilla\tools\moztools
set CYGWIN_DIR=c:\MyWork\ghostzilla\tools\cygwin
set MSVC60_DIR=c:\Program Files\Microsoft Visual Studio
set WINDOWS_DIR=c:\windows

@echo [ghzil] Step 1: Setting Mozilla/Ghostzilla environment variables.

@rem ghostzilla/mozilla stuff
set GHOSTZILLA_SOURCE_DIR=%MOZILLA_SOURCE_DIR%\ghostzilla
set OS_TARGET=WIN95
set MOZCONFIG=%GHOSTZILLA_SOURCE_DIR%\config\.mozconfig
set BUILD_PSM2=1
set GHDIR=%MOZILLA_SOURCE_DIR%

@rem path
set PATH=
set PATH=%MOZ_TOOLS%\bin;%CYGWIN_DIR%\bin
set PATH=%PATH%;%WINDOWS_DIR%\system32;%WINDOWS_DIR%;%WINDOWS_DIR%\System32\Wbem;%WINDOWS_DIR%\System32\Wbemset;%WINDOWS_DIR%\COMMAND
set PATH=%PATH%;%MSVC60_DIR%\Common\Tools\WinNT
set PATH=%PATH%;%MSVC60_DIR%\Common\MSDev98\Bin
set PATH=%PATH%;%MSVC60_DIR%\Common\Tools
set PATH=%PATH%;%MSVC60_DIR%\VC98\bin

@rem msvc6.0 stuff
set INCLUDE=%MSVC60_DIR%\VC98\atl\include;%MSVC60_DIR%\VC98\mfc\include;%MSVC60_DIR%\VC98\include
set LIB=%MSVC60_DIR%\VC98\mfc\lib;%MSVC60_DIR%\VC98\lib
set MSDevDir=%MSVC60_DIR%\Common\MSDev98

@rem nmake only stuff (not used)
set MOZ_SRC=%MOZILLA_SOURCE_DIR%
set MOZ_BITS=32
set MOZ_DEBUG=
set WINOS=%OS_TARGET%
set _MSC_VER=1200
set DISABLE_TESTS=1

@echo [ghzil] Step 1: Setting Mozilla/Ghostzilla environment variables complete.
