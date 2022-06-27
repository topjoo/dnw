// vt100 Escape Seqence /////////////////////////////

#include <ctype.h>
#include <stdlib.h>

#include "ProcessEscSeq.h"

ProcessEscSeq::ProcessEscSeq()
{
	m_stEscSeq = STESC_NULL;

	m_stCodeProcess = CPST_WAIT;

	m_numParam = 0;
	m_pstrNumParam = m_nParamStr[0];
}


int ProcessEscSeq::IsEscOn()
{
	if (m_stEscSeq == STESC_ON)
		return 1;
    return 0;
}

int ProcessEscSeq::EscOn()
{
	if (m_stEscSeq == STESC_ON)
		return -1;
    m_stEscSeq = STESC_ON;
	m_stCodeProcess = CPST_WAIT;
	return 0;
}

int ProcessEscSeq::EscOff()
{
	if (m_stEscSeq == STESC_NULL)
		return -1;
    m_stEscSeq = STESC_NULL;
	m_stCodeProcess = CPST_WAIT;
	return 0;
}

int ProcessEscSeq::ProcessEscCode(char ch)
{

	switch (m_stCodeProcess) {
	case CPST_WAIT:
	case CPST_START : // 1
	   m_numParam = 0;
	   m_pstrNumParam = m_nParamStr[0];
       *m_pstrNumParam = 0;
       if (isdigit((int)ch)) {
          m_escFunCode = ch;
          this->EscOff();
	   } else if (isalpha((int) ch)) {
          m_escFunCode = ch;
          this->EscOff();
	   } else if (ch == '[') {
		  m_numParam = 0;
		  m_pstrNumParam = m_nParamStr[0];
          *m_pstrNumParam = 0;
          m_stCodeProcess = CPST_PMSTART;
	   } else {
          m_escFunCode = 0;
          this->EscOff();
	      return 1;
	   }
	   break;
	case CPST_PMSTART :
       if (isdigit((int)ch)) {
          *m_pstrNumParam++ = ch;
          *m_pstrNumParam = 0;
	   } else if (isalpha((int) ch)) {
          m_escFunCode = ch;
          this->EscOff();
	   } else if (ch == ';') {
		  m_nParam[m_numParam] = atoi((const char *)m_nParamStr[m_numParam]);
		  m_numParam++;
		  m_pstrNumParam = m_nParamStr[m_numParam];
          *m_pstrNumParam = 0;
	   } else {
          m_escFunCode = 0;
          this->EscOff();
	      return 1;
	   }
	   break;
	default:
       m_escFunCode = 0;
       this->EscOff();
	   return 1;
	}
	return 0;
}

int ProcessEscSeq::GetNumParamLength()
{
    return m_numParam;
}

int ProcessEscSeq::GetNumParam(int pnum)
{
	if (pnum >= m_numParam)
		return -1;
    return m_nParam[pnum];
}



