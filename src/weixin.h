#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



enum Error_Code
{
	Error_Invalid_Url = 1,
	Error_Invalid_Signature_Token = 2,
	Error_Invalid_Signature_Equal_Sign = 2,
	Error_Invalid_Signature_And_Sign = 2,
	Error_Code = 3
};

int wx_validate(const char *uri);
