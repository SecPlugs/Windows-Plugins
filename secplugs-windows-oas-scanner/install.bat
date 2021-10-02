echo "Installing Secplugs Windows File Watcher"

set INSTALLDIR=C:\Program Files\Secplugs\
if not exist %INSTALLDIR% (
	mkdir %INSTALLDIR%
)

xcopy %~dp0\* "%INSTALLDIR%"  /f

%INSTALLDIR%SecplugsWindowsScanner.exe /registerService
net start SecplugsWindowsScanner
