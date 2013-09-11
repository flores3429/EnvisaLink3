#include "libel3.h"
#include "libel3_defs.h"
#include "libel3_commands.h"
#include "libel3_log.h"

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

QCoreApplication *g_app;

CEnvisalink3::CEnvisalink3()
{
	m_TcpSock = nullptr;
}

CEnvisalink3::~CEnvisalink3()
{
	if( m_TcpSock != nullptr )
	{
		delete m_TcpSock;
		m_TcpSock = nullptr;
	}
}

UINT8 CEnvisalink3::CalculateCksum(const UINT8 *data, int size)
{
	UINT8 cksum = 0;

	//
	// Calculate 8-bit hex checksum from packet
	//
	for( int i=0; i<size; ++i )
		cksum += data[i];

	return cksum;
}

int CEnvisalink3::SendCommand(unsigned command, const UINT8 *pData, unsigned nDataBytes)
{
	int packet_size = NUM_CMD_BYTES + nDataBytes + CKSUM_SIZE + CRLF_SIZE;
	UINT8 packet[MAX_PACKET_SIZE];
	unsigned pos = 0;
	
	memcpy(&packet[pos], &command, NUM_CMD_BYTES);
	pos += NUM_CMD_BYTES;

	memcpy(&packet[pos], pData, nDataBytes);
	pos += nDataBytes;

	UINT8 cksum = CalculateCksum(packet, pos+1);
	memcpy(&packet[pos], &cksum, CKSUM_SIZE);
	pos += CKSUM_SIZE;

	memcpy(&packet[pos], &CRLF, CRLF_SIZE);
	pos += CRLF_SIZE;

	int sent = m_TcpSock->write((const char *)packet, packet_size);
	g_app->processEvents();

	if( sent != packet_size )
	{
		ErrPrintf("Unable to send packet of size %u\n\t", packet_size);
		LogData(packet, packet_size);
	}

	return sent;
}

int CEnvisalink3::GetResponse(unsigned &cmd, UINT8 **ppData)
{
	static UINT8 packet[MAX_PACKET_SIZE];

	QTime time;
	int bytes_read = 0;

	//
	// Read data bytes until we get the packet termination sequence
	//
	time.start();
	while( 1 )
	{
		g_app->processEvents();
		bytes_read += m_TcpSock->read((char *)&packet[bytes_read], 1);

		if( (bytes_read >= 2) &&  (*((UINT16 *)&packet[bytes_read-2]) == CRLF) )
		{
			break;
		}

		if( bytes_read > MAX_PACKET_SIZE )
		{
			ErrPrintf("Read %u bytes but never recieved the packet terminator!!\n");
			exit(1);
			break;
		}

		if( time.elapsed() > 10000 )
		{
			ErrPrintf("Timeout waiting for response!!\n");
			exit(1);
			break;
		}
	}

	if( !VerifyCkSum(packet, bytes_read) )
	{
		ErrPrintf("Checksum failed! Raw Bytes:\n\t");
		for( int i=0; i<bytes_read; ++i )
			ErrPrintf("0x%02x ", packet[i]);
		ErrPrintf("\n");
		return -1;
	}

	cmd = AsciiToHex(&packet[0], 3);

	*ppData = &packet[NUM_CMD_BYTES];

	int data_size = bytes_read - NUM_CMD_BYTES - CRLF_SIZE - CKSUM_SIZE;
	return data_size;
}

unsigned CEnvisalink3::AsciiToHex(const UINT8 *data, int size)
{
	char ascii[MAX_DATA];
	memcpy(ascii, data, size);
	ascii[size] = 0;

	unsigned hex = strtoul(ascii, nullptr, 16);
	return hex;
}

bool CEnvisalink3::VerifyCkSum(const UINT8 *packet, int size)
{
	UINT8 cksum_off = size - (CKSUM_SIZE + CRLF_SIZE);
	UINT8 cksum_c = 0;
	UINT8 cksum_a = 0;

	//
	// Calculate 8-bit hex checksum from packet
	//
	cksum_c = CalculateCksum(packet, cksum_off);

	//
	// Convert 2-byte ascii checksum embedded within packet
	//
	cksum_a = AsciiToHex(&packet[cksum_off], 2);

	//
	// Compare actual checksum with calucated checksum
	//
	return (cksum_a == cksum_c);
}

EL3_STATUS CEnvisalink3::Open(const char *host, unsigned short port, const char *passwd)
{
	EL3_STATUS status = EL3_OK;

	int argc = 0;
	g_app = new QCoreApplication(argc, nullptr);

	m_TcpSock = new QTcpSocket();
	m_TcpSock->connectToHost(host, port);
	if( !m_TcpSock->waitForConnected(10000) )
	{
		status = EL3_INVALID_PARAM;
		ErrPrintf("Unable to make TCP connection to %s:%u\n", host, port);
		exit(1);
	}

	DbgPrintf("TCP Connection made to %s:%u\n", host, port);

	unsigned cmd = 0;
	UINT8 *data = nullptr;

	//
	// Wait for login request from EL3
	//
	GetResponse(cmd, &data);
	if( cmd == EL3CMD_LOGIN_INTERACTION )
	{
		switch( data[0] )
		{
			case 3:
				DbgPrintf("Password request from EL3, code = %u.\n", data[0]);
				break;

			default:
				DbgPrintf("Unexpected response after connection established. Code = %u.\n", data[0]);
				status = EL3_UNEXPECTED_RESPONSE;
				return status;
		}
	}
	else
	{
		status = EL3_UNEXPECTED_RESPONSE;
		return status;
	}

	//
	// Send Login response
	//
	SendCommand(EL3CMD_NETWORK_LOGIN, (UINT8 *)passwd, strlen(passwd));

	GetResponse(cmd, &data);
	if( cmd != EL3CMD_COMMAND_ACKNOWLEDGE )
	{
		status = EL3_UNEXPECTED_RESPONSE;
	}

	//
	// See if the login worked...
	//
	GetResponse(cmd, &data);
	if( cmd == EL3CMD_LOGIN_INTERACTION )
	{
		switch( data[0] )
		{
			case 0:
				DbgPrintf("Login FAILED!!\n");
				status = EL3_BAD_CREDENTIALS;
				break;

			case 1:
				DbgPrintf("Login successfull!!\n");
				status = EL3_OK;
				break;

			case 2:
				DbgPrintf("Login timed out!!\n");
				status = EL3_TIMEOUT;
				break;

			case 3:
				DbgPrintf("Password request from EL3, code = %u.\n", data[0]);
				break;

			default:
				DbgPrintf("Unexpected response after login password sent. Code = %u.\n", data[0]);
				status = EL3_UNEXPECTED_RESPONSE;
				break;
		}
	}
	else
	{
		status = EL3_UNEXPECTED_RESPONSE;
	}

	return status;
}

