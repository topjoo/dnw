@REM ----------------------------------------------------------------
@REM MD5 checksum
@REM ----------------------------------------------------------------
@REM AttachHdr -i @.zip -o md5checksum.txt -M MD5
@REM AttachHdr -i @.zip -o md5checksum.txt -M SHA1
@REM 
@REM -- create MD5 for all files in current directory ---------------
@REM 
@REM AttachHdr -i @.* -o md5checksum.txt -M MD5 -z1
@REM
AttachHdr -i @.zip -o md5checksum.txt -M MD5 -z1
@REM
AttachHdr -i @.zip -o md5checksum.txt -M SHA256 -z1
@REM 
AttachHdr -i @.zip -o md5checksum.txt -M SHA512 -z1
@REM AttachHdr -i @.txt -a md5checksum.txt -M MD5 
@REM
pause