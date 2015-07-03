#include <string>
#include <string.h>
#include "weixin.h"

#define WX_LOG(x) do { printf("%-20s ", __func__); printf x; putchar('\n'); \
  fflush(stdout); } while(0)

using namespace std;

int wx_validate(const char *uri)
{
	size_t posToken;
	size_t posEqualSign;
	size_t posAndSign;
	string signature;
	string sTemp;

	if (NULL == uri)
	{
		WX_LOG(("ERROR: Invalid URL."));
		return Error_Invalid_Url;
	}

	sTemp.assign(uri);
	posToken = sTemp.find("signature");
	if (string::npos == posToken)
	{
		WX_LOG(("ERROR: Invalid URL: %s.", uri));
		return Error_Invalid_Signature_Token;
	}
	else
	{
		posEqualSign = sTemp.find("=");
		if (string::npos == posEqualSign)
		{
			WX_LOG(("ERROR: Invalid URL: %s.", uri));
			return Error_Invalid_Signature_Equal_Sign;
		}
		posAndSign = sTemp.find("&");
		if (string::npos == posAndSign)
		{
			WX_LOG(("ERROR: Invalid URL: %s.", uri));
			return Error_Invalid_Signature_And_Sign;
		}
		signature = sTemp.substr(posEqualSign, posAndSign - posEqualSign);
	}
}
