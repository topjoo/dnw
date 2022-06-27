@REM ==============================================================================================
@REM === signed firmware for MICOM binary 
@REM === by SangWook.JOO at 2017.12.12
@REM ==============================================================================================
@REM
@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- signature.sign 생성한다.
@REM ----------------------------------------------------------------------------------------------
Openssl dgst -sha256 -sign private_key.pem -out signature.sign RBox302K6_APP.bin

@REM
@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- signature.sign 파일 과 RBox302K6_APP.bin 더하여 JCR1110_SGN.bin 파일 생성
@REM -- 2nd file, RBox302K6_APP.bin 의 시작위치는 0x200 번지
@REM ----------------------------------------------------------------------------------------------
AttachHdr.exe --join 0x200 --size little -i signature.sign  RBox302K6_APP.bin -o JCR1110_SGN.bin

@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- Header 를 붙인다.
@REM -- HW Revision : STM32F302K8U6 
@REM -- Model Name  : JCR1110-OBD2
@REM -- Checksum    : input file에 대한 crc16 값 (즉, JCR1110_SGN.bin 파일에 대한 crc16 값)
@REM -- Version     : QL14A
@REM ----------------------------------------------------------------------------------------------
AttachHdr.exe -b JCR1110-OBD2  -m STM32F302K8U6 --cinfo crc16  --input JCR1110_SGN.bin  --output JCR1110.OBD  -v QL14A

@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- input file에 대한 checksum file을 만든다.
@REM ----------------------------------------------------------------------------------------------
AttachHdr.exe -i JCR1110.OBD  -o JCR1110.CRC  --checksum crc16


@REM ----------------------------------------------------------------------------------------------
@REM -- ST LINK 를 이용하여 download 하여야 한다. 반드시~~~
@REM -- Merge files (Boot image & Application image) for ST LINK
@REM -- 공장 생산용 Binary
@REM -- Bootloader 와 signed firmware인 Application을 1개의 파일로 묶는다.  
@REM -- signed firmware application의 위치는 0x4800 위치에 있다.
@REM ----------------------------------------------------------------------------------------------
@REM

AttachHdr.exe --join 4800    -i Rbox_BOOT.bin  JCR1110_SGN.bin        -o RBoxMerge_QL14A.bin

@REM
@REM
pause

