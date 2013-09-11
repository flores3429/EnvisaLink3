#include "libel3_defs.h"

struct EL3_COMMAND_DESCRIPTOR
{
	const char *symbol;
	unsigned MinDataBytes;
	unsigned MaxDataBytes;
};

extern EL3_COMMAND_DESCRIPTOR g_El3Commands[];

static const unsigned EL3CMD_POLL                             = 0;
static const unsigned EL3CMD_STATUS_REPORT                    = 1;
static const unsigned EL3CMD_DUMP_ZONE_TIMERS                 = 8;
static const unsigned EL3CMD_NETWORK_LOGIN                    = 5;
static const unsigned EL3CMD_SET_TIME_AND_DATE                = 10;
static const unsigned EL3CMD_COMMAND_OUTPUT_CONTROL           = 20;
static const unsigned EL3CMD_PARTITION_ARM_CONTROL_AWAY       = 30;
static const unsigned EL3CMD_PARTITION_ARM_CONTROL_STAY       = 31;
static const unsigned EL3CMD_PARTITION_ARM_CONTROL_ZERO_DELAY = 32;
static const unsigned EL3CMD_PARTITION_ARM_CONTROL_WITH_CODE  = 33;
static const unsigned EL3CMD_PARTITION_DISARM_CONTROL         = 40;
static const unsigned EL3CMD_TIME_STAMP_CONTROL               = 55;
static const unsigned EL3CMD_TIME_BROADCAST_CONTROL           = 56;
static const unsigned EL3CMD_TEMPERATURE_BROADCAST_CONTROL    = 57;
static const unsigned EL3CMD_TRIGGER_PANIC_ALARM              = 60;
static const unsigned EL3CMD_SEND_KEYSTROKE_STRING            = 71;
static const unsigned EL3CMD_KEEP_ALIVE                       = 74;

static const unsigned EL3CMD_CODE_SEND                        = 200
	;
static const unsigned EL3CMD_COMMAND_ACKNOWLEDGE              = 500;
static const unsigned EL3CMD_COMMAND_ERROR                    = 501;
static const unsigned EL3CMD_SYSTEM_ERROR                     = 502;
static const unsigned EL3CMD_LOGIN_INTERACTION                = 505;
static const unsigned EL3CMD_KEYPAD_LED_STATE                 = 510;
static const unsigned EL3CMD_KEYPAD_LED_FLASH_STATE           = 511;