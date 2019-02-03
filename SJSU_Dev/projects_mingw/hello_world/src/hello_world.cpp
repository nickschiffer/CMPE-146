#include <stdio.h>
#include "generated_can.h"
#include <assert.h>
#include <string.h>



void enableFlushAfterPrintf()
{
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stdin, 0, _IONBF, 0);
}

uint8_t bytes_sent[8];
bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    printf("Send MID %u  DLC %u\n", mid, dlc);
    memcpy(bytes_sent, bytes, 8);

    return true;
}

int main()
{
	enableFlushAfterPrintf();
	uint8_t bytes[8];

	IO_DEBUG_t to;
	IO_DEBUG_t from = { 0 };
	from.IO_DEBUG_test_enum            = IO_DEBUG_test2_enum_one;
	from.IO_DEBUG_test_float           = 1.3;
	from.IO_DEBUG_test_float_signed    = -123.6;
	from.IO_DEBUG_test_signed          = -123;
	from.IO_DEBUG_test_signed_minmax   = 120;
	from.IO_DEBUG_test_unsigned1       = 200;
	from.IO_DEBUG_test_unsigned_minmax = 50;

	dbc_encode_and_send_IO_DEBUG(&from);
	dbc_encode_IO_DEBUG(bytes, &from);
	dbc_decode_IO_DEBUG(&to, bytes, NULL);
	assert(0 == memcmp(bytes, bytes_sent, 8));

    printf("%u\n", to.IO_DEBUG_test_enum           );
    printf("%f\n", to.IO_DEBUG_test_float          );
    printf("%f\n", to.IO_DEBUG_test_float_signed   );
    printf("%i\n", to.IO_DEBUG_test_signed         );
    printf("%i\n", to.IO_DEBUG_test_signed_minmax  );
    printf("%u\n", to.IO_DEBUG_test_unsigned1      );
    printf("%u\n", to.IO_DEBUG_test_unsigned_minmax);

	assert(to.IO_DEBUG_test_enum             == IO_DEBUG_test2_enum_one);
	//assert(to.IO_DEBUG_test_float            == 1.5);
	//assert(to.IO_DEBUG_test_float_signed     == 123.6);
	assert(to.IO_DEBUG_test_signed           == -123);
	assert(to.IO_DEBUG_test_signed_minmax    == 120);
	assert(to.IO_DEBUG_test_unsigned1        == 200);
	assert(to.IO_DEBUG_test_unsigned_minmax  == 50);


    from.IO_DEBUG_test_enum            = IO_DEBUG_test2_enum_two;
    from.IO_DEBUG_test_float           = 1.8;
    from.IO_DEBUG_test_float_signed    = 123.4;
    from.IO_DEBUG_test_signed          = 123;
    from.IO_DEBUG_test_signed_minmax   = -120;
    from.IO_DEBUG_test_unsigned1       = 103;
    from.IO_DEBUG_test_unsigned_minmax = 123; // Over the max

    dbc_encode_and_send_IO_DEBUG(&from);
    dbc_encode_IO_DEBUG(bytes, &from);
    dbc_decode_IO_DEBUG(&to, bytes, NULL);

    printf("%u\n", to.IO_DEBUG_test_enum           );
    printf("%f\n", to.IO_DEBUG_test_float          );
    printf("%f\n", to.IO_DEBUG_test_float_signed   );
    printf("%i\n", to.IO_DEBUG_test_signed         );
    printf("%i\n", to.IO_DEBUG_test_signed_minmax  );
    printf("%u\n", to.IO_DEBUG_test_unsigned1      );
    printf("%u\n", to.IO_DEBUG_test_unsigned_minmax);

    assert(to.IO_DEBUG_test_enum             == IO_DEBUG_test2_enum_two);
    //assert(to.IO_DEBUG_test_float            == 1.5);
    //assert(to.IO_DEBUG_test_float_signed     == 123.6);
    assert(to.IO_DEBUG_test_signed           == 123);
    assert(to.IO_DEBUG_test_signed_minmax    == -120);
    assert(to.IO_DEBUG_test_unsigned1        == 103);
    assert(to.IO_DEBUG_test_unsigned_minmax  == 100);

    return 0;
}
