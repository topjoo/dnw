
// vt100 Escape Seqence /////////////////////////////

#ifndef _PROCESSESCSEQ_H_
#define _PROCESSESCSEQ_H_

#define ESC  27

//#define isNumChar(ch) {(((unsigned char) ch) >= '0' && ((unsigned char) ch) <= '9') ? 1 : 0}

class ProcessEscSeq {
public :

	ProcessEscSeq();

public :

  // define active ESC process state...
   #define STESC_NULL     0
   #define STESC_ON       1

	int  m_stEscSeq;

	int  IsEscOn();
	int  EscOn();
	int  EscOff();

    int  ProcessEscCode(char ch);

private:

#define CPST_WAIT    0
#define CPST_START   1
#define CPST_PMSTART 2  // esc [ process

	int m_stCodeProcess;

	int m_escFunCode;

#define SZPNUM_LENGATH 100
#define SZPNUM_PCNT    10
	int m_numParam;
	char m_nParamStr[SZPNUM_PCNT][SZPNUM_LENGATH];
	int  m_nParam[SZPNUM_PCNT];
	char *m_pstrNumParam;

	int GetNumParamLength();
	int GetNumParam(int pnum);


};

#endif