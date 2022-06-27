#ifndef __USBTXRX_H__
#define __USBTXRX_H__

void MenuUsbTransmit(HWND hwnd, BYTE funcKey);
void MenuUsbTransmitHistory(HWND hwnd, int history, BYTE funcKey);
void MenuUBOOTHistory(HWND hwnd, int history);
void MenuUBOOTHistory2(HWND hwnd, int history);
void MenuUBOOT(HWND hwnd);
void MenuUBOOT2(HWND hwnd, BYTE funcKey);
void MenuUsbReceive(HWND hwnd);
void MenuUsbStatus(HWND hwnd);
int IsUsbConnected(void);
WORD ROM_MenuUBOOT2(HWND hwnd, int Fileindex, BYTE funcKey);
BOOL RAM_MenuUsbTransmit(HWND hwnd, int Fileindex, BYTE funcKey);

#endif //__USBTXRX_H__

