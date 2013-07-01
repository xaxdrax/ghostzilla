@if NOT "%MOZILLA_SOURCE_DIR%" == "" goto dirSetOK
@echo [ghzil] Step 5: ERROR: RUN step1-setEnvironment.bat first!
@goto end
:dirSetOK

@pushd .
cd %MOZILLA_SOURCE_DIR%
@echo [ghzil] Step 5: rebuilding Mozilla
make -f client.mk build

@if errorlevel 1 goto step_error
@echo [ghzil] Step 5: rebuilding Mozilla complete.
@goto end

:step_error
@echo [ghzil] Step 5: error rebuilding Mozilla

:end
@popd
