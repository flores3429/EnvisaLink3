#ifndef __LIBEL3_H__
#define __LIBEL3_H__

#include "libel3_defs.h"

enum EL3_STATUS
{
	EL3_OK,
	EL3_INVALID_PARAM,
	EL3_BAD_CREDENTIALS,
	EL3_TIMEOUT,
	EL3_UNEXPECTED_RESPONSE,
	EL3_UNKNOWN
};

//
// Forward decl
//
class QTcpSocket;

static const UINT16 EL3_PORT = 4025;

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//                                   CEnvisalink3
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

class CEnvisalink3
{
public:
	CEnvisalink3();
	~CEnvisalink3();

	//
	// Main API
	//
	EL3_STATUS  Open(const char *host, unsigned short port, const char *passwd);

	void Close();
	bool IsOpen();
	int  SendCommand(unsigned command, const UINT8 *pData, unsigned nDataBytes);
	int  GetResponse(unsigned &cmd, UINT8 **ppData);
	bool VerifyCkSum(const UINT8 *packet, int size);
	UINT8 CalculateCksum(const UINT8 *data, int size);

	//int EncodePacket(unsigned command, BYTE *pData, unsigned nData);
	//int DecodePacket(const UINT8 *packet, unsigned packet_size, unsigned &cmd, UINT8 **ppData, unsigned &data_size);

protected:
	unsigned AsciiToHex(const UINT8 *data, int size);

protected:
	QTcpSocket *m_TcpSock;

	static const unsigned MAX_DATA = 8;
	static const UINT16   CRLF = 0x0a0d;
	static const unsigned NUM_CMD_BYTES   = 3;
	static const unsigned MAX_DATA_BYTES  = 8;
	static const unsigned CKSUM_SIZE      = 2;
	static const unsigned CRLF_SIZE       = 2;
	static const unsigned MAX_PACKET_SIZE = NUM_CMD_BYTES + MAX_DATA_BYTES + CRLF_SIZE + CKSUM_SIZE;
};


#endif
