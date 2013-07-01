@if NOT "%MOZILLA_SOURCE_DIR%" == "" goto dirSetOK
@echo [ghzil] Step 6: ERROR: RUN step1-setEnvironment.bat first!
@goto end
:dirSetOK

@echo [ghzil] Step 5: Creating Ghostzilla distribution

rm -rf %GHOSTZILLA_SOURCE_DIR%\distribution
@mkdir %GHOSTZILLA_SOURCE_DIR%\distribution
@if errorlevel 1 goto step_error
@mkdir %GHOSTZILLA_SOURCE_DIR%\distribution\bin
@if errorlevel 1 goto step_error

xcopy /s /e %MOZILLA_SOURCE_DIR%\dist\bin %GHOSTZILLA_SOURCE_DIR%\distribution\bin
@if errorlevel 1 goto step_error

@if exist %GHOSTZILLA_SOURCE_DIR%\distribution\bin\mozilla.exe del %GHOSTZILLA_SOURCE_DIR%\distribution\bin\mozilla.exe

copy /y %GHOSTZILLA_SOURCE_DIR%\documentation\Instructions-source.html %GHOSTZILLA_SOURCE_DIR%\distribution\Instructions.html
@if errorlevel 1 goto step_error
copy /y %GHOSTZILLA_SOURCE_DIR%\documentation\license.txt %GHOSTZILLA_SOURCE_DIR%\distribution\license.txt
@if errorlevel 1 goto step_error
copy /y %GHOSTZILLA_SOURCE_DIR%\bin\Start-Ghostzilla-CD.exe %GHOSTZILLA_SOURCE_DIR%\distribution\Start-Ghostzilla-CD.exe
@if errorlevel 1 goto step_error
copy /y %GHOSTZILLA_SOURCE_DIR%\bin\dllui.dll %GHOSTZILLA_SOURCE_DIR%\distribution\bin
@if errorlevel 1 goto step_error
copy /y %GHOSTZILLA_SOURCE_DIR%\bin\bszip.dll %GHOSTZILLA_SOURCE_DIR%\distribution\bin
@if errorlevel 1 goto step_error

@echo [ghzil] Step 6: creating Ghostzilla distribution complete!
@echo                 the program is in distribution\ directory.
@goto end

:step_error
@echo [ghzil] Step 5: error creating Ghostzilla distribution

:end
@popd
