@if NOT "%MOZILLA_SOURCE_DIR%" == "" goto dirSetOK
@echo [ghzil] Step 2: ERROR: RUN step1-setEnvironment.bat first!
@goto end
:dirSetOK

@echo [ghzil] Step 2: building Mozilla
@pushd .
cd %MOZILLA_SOURCE_DIR%
make -f client.mk build

@if errorlevel 1 goto step_error
@echo [ghzil] Step 2: building Mozilla complete.
@goto end

:step_error
@echo [ghzil] Step 2: error building Mozilla

:end
@popd
