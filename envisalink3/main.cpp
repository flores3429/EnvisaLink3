#include <stdio.h>
#include <stdlib.h>
#include "../libel3/libel3.h"

int main(int argc, char **argv)
{
	CEnvisalink3 el3;

	//EL3_STATUS status = el3.Open("envisalink", EL3_PORT, "user");
	EL3_STATUS status = el3.Open("192.168.1.130", EL3_PORT, "user");
	//EL3_STATUS status = el3.Open("192.168.1.1", 22, "root");
	if( status != EL3_OK )
	{
		printf("Error: Could not open Envisalink3 module. Verify that the module is present on the network and that the path name is correct.\n");
		printf("Code = %u\n", status);
		exit(1);
	}

	return 0;
}