@REM ----------------------------------------------------------------
@REM AttachHdr Option
@REM  -b : BoardName   (16 Bytes)
@REM  -m : ModelName   (16 Bytes)
@REM  -c : BuildDate   (16 Bytes - Current DeskTop Date and Time)
@REM  -v : VersionName (16 Bytes)
@REM
@REM  * ���� ������ Bytes�� (16Byte) ������ ����ȴ�.
@REM ----------------------------------------------------------------
@REM Attaching Header
@REM
@REM
@REM=== Convert Intel Hex to Bin ======
AttachHdr.exe -i micom_mnav_ib_navi.hex       -o ib_avn_int_L7_10th.bin  --intel --length A0000
@REM
AttachHdr.exe -i micom_mnav_ib_navi-AUO.hex   -o ib_avn_int_L7_60th.bin  --intel --length A0000
@REM
@REM=== 13CY Micom V850 ===
@REM AttachHdr.exe -b LAN3320KT -m MICOM_13CY_V850 -c -i ROMP.BIN -o LAN3320KT_ROMP.LGE  -v 13E-NA20A
@REM
@REM copy romp.hex    LAN3320KT_NaviBox_13E-NA20A.hex
@REM
@REM AttachHdr.exe -i LAN3320KT_ROMP.LGE  -o LAN3320KT_ROMP.CRC  -G crc16
@REM=== 
@REM=========   LAN3320KT  - Lexus Model without DMB RSE ==============
@REM=== 
pause
