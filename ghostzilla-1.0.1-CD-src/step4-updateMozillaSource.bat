@if NOT "%MOZILLA_SOURCE_DIR%" == "" goto dirSetOK
@echo [ghzil] Step 4: ERROR: RUN step1-setEnvironment.bat first!
@goto end
:dirSetOK

@echo [ghzil] Step 4: updating Mozilla source

pushd .
cd contrib
call updateAll.bat
popd

@echo [ghzil] Step 4: updating Mozilla source complete.

:end