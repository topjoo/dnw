@REM
DEL /F /S /Q .\*.pch
DEL /F /S /Q .\*.pdb
DEL /F /S /Q .\*.iobj
DEL /F /S /Q .\*.ipdb

DEL /F /S /Q .\x64\Release\*.obj
DEL /F /S /Q .\x64\Release\*.iobj
DEL /F /S /Q .\x64\Release\*.ipdb
DEL /F /S /Q .\x64\Release\*.tlog
DEL /F /S /Q .\x64\Release\*.res
del /F /S /Q .\*.ncb
del /F /S /Q .\*.sdf
del /F /S /Q .\*.map

del /F /S /Q .\Release\*.bsc
del /F /S /Q .\Release\*.obj
del /F /S /Q .\Release\*.sbr
del /F /S /Q .\Release\*.manifest
del /F /S /Q .\Release\*.res

rmdir /S /Q .\.vs
rmdir /S /Q .\x64\Release\*tlog
rmdir /S /Q .\x64\Release\MakeBin.tlog
rmdir /S /Q .\x64\Debug

rmdir /S /Q .\Release
rmdir /S /Q .\Debug
rmdir /S /Q .\ipch
rmdir /S /Q .\Debug
rmdir /S /Q .\Release\LOG
