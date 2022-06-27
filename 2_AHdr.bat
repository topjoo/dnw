@REM ----------------------------------------------------------------
@REM AttachHdr Option
@REM  -b or --board   : BoardName   (16 Bytes)
@REM  -m or --model   : ModelName   (16 Bytes)
@REM  -c or --cinfo   : BuildDate   (16 Bytes - Current DeskTop Date and Time)
@REM  -v or --version : VersionName (16 Bytes)
@REM
@REM  * 각각 정해진 Bytes수 (16Byte*4) = 64 Byte 까지만 적용된다.
@REM ----------------------------------------------------------------
@REM Attaching Header
@REM
@REM
@REM=== Convert motorolA Hex to Bin ======
@REM ah.exe -i ROMP.hex  -o ROMP.BIN  --motorola
@REM
@REM=== Convert Intel Hex to Bin ======
@REM ah.exe -i ROMP.hex  -o ROMP.BIN  --intel
@REM
@REM
@REM --- COPY binary ------
@REM copy  ..\..\RBox302K8_APP\EWARM\RBox302K6_APP\Exe\RBox302K6_APP.bin    JCR1110_RevC.bin
@REM
@REM
@REM
@REM --- Attach Header to binary ----
@REM
@REM ah.exe -b JCR1110-RevC -m STM32F302K8U6 --cinfo date  --input RBox302K6_APP.bin  --output JCR1110.OBD  -v QD03A
@REM
@REM ah.exe -b JCR1110-RevC -m STM32F302K8U6 --cinfo crc16   --input RBox302K6_APP.bin  --output JCR1110.OBD  -v QD03A
@REM
@REM ah.exe -b JCR1110-RevC -m STM32F302K8U6 --cinfo CRC64   --input RBox302K6_APP.bin  --output JCR1110.OBD  -v QD03A
@REM
@REM ah.exe -b JCR1110-RevC -m STM32F302K8U6 --cinfo CRC32   --input RBox302K6_APP.bin  --output JCR1110.OBD  -v QD03A
@REM
@REM
@REM --- Create CRC files for JCR1110.OBD file.
@REM ah.exe -i JCR1110.OBD  -o JCR1110.CRC  --checksum CRC16
@REM=== 
@REM 
@REM 
@REM=== 
@REM ah.exe -d 64 -i JCR1110.OBD -o JCR1110_RevC_detach.bin  
@REM ah.exe --detach 64  -i JCR1110.OBD -o JCR1110_RevC_detach.bin  
@REM
@REM=== 
@REM 
@REM copy JCR1110.CRC  ..\..\images\
@REM copy JCR1110.OBD  ..\..\images\
@REM 
@REM 
@REM -- merge files (Boot image & App image)
@REM ah.exe --join 2800 -i Rbox_boot.bin  Rbox_App.bin  -o JCR1110_ALL.bin
@REM 
@REM 
@REM 
@REM -- checksum ---
ah.exe -i @.@  -o SHA256sum.txt   --checksum SHA256
ah.exe -i @.zip  -a MS5sum.txt --checksum MD5
ah.exe -i A@.zip -g              --checksum SHA1

@REM=== 
@REM
pause
