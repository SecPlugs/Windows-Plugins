set SOURCE=%~dp0
set RELEASE=%~dp0\x64\Release
set DEST=%SOURCE%\Installer
rmdir /Q /S %DEST%
mkdir %DEST%
xcopy %RELEASE%\*.dll %DEST%
xcopy %RELEASE%\*.pdb %DEST%
xcopy %RELEASE%\*.exe %DEST%
xcopy %SOURCE%\install.bat %DEST%
xcopy %SOURCE%\config.json %DEST%
