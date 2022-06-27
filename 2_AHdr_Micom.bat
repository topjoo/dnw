@REM ==============================================================================================
@REM === signed firmware for MICOM binary 
@REM === by SangWook.JOO at 2017.12.12
@REM ==============================================================================================
@REM
@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- signature.sign �����Ѵ�.
@REM ----------------------------------------------------------------------------------------------
Openssl dgst -sha256 -sign private_key.pem -out signature.sign RBox302K6_APP.bin

@REM
@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- signature.sign ���� �� RBox302K6_APP.bin ���Ͽ� JCR1110_SGN.bin ���� ����
@REM -- 2nd file, RBox302K6_APP.bin �� ������ġ�� 0x200 ����
@REM ----------------------------------------------------------------------------------------------
AttachHdr.exe --join 0x200 --size little -i signature.sign  RBox302K6_APP.bin -o JCR1110_SGN.bin

@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- Header �� ���δ�.
@REM -- HW Revision : STM32F302K8U6 
@REM -- Model Name  : JCR1110-OBD2
@REM -- Checksum    : input file�� ���� crc16 �� (��, JCR1110_SGN.bin ���Ͽ� ���� crc16 ��)
@REM -- Version     : QL14A
@REM ----------------------------------------------------------------------------------------------
AttachHdr.exe -b JCR1110-OBD2  -m STM32F302K8U6 --cinfo crc16  --input JCR1110_SGN.bin  --output JCR1110.OBD  -v QL14A

@REM
@REM ----------------------------------------------------------------------------------------------
@REM -- input file�� ���� checksum file�� �����.
@REM ----------------------------------------------------------------------------------------------
AttachHdr.exe -i JCR1110.OBD  -o JCR1110.CRC  --checksum crc16


@REM ----------------------------------------------------------------------------------------------
@REM -- ST LINK �� �̿��Ͽ� download �Ͽ��� �Ѵ�. �ݵ��~~~
@REM -- Merge files (Boot image & Application image) for ST LINK
@REM -- ���� ����� Binary
@REM -- Bootloader �� signed firmware�� Application�� 1���� ���Ϸ� ���´�.  
@REM -- signed firmware application�� ��ġ�� 0x4800 ��ġ�� �ִ�.
@REM ----------------------------------------------------------------------------------------------
@REM

AttachHdr.exe --join 4800    -i Rbox_BOOT.bin  JCR1110_SGN.bin        -o RBoxMerge_QL14A.bin

@REM
@REM
pause

