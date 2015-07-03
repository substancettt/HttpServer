#include <string>
#include <string.h>
#include "weixin.h"



using namespace std;

int wx_validate(const char * query)
{
	size_t posToken;
	size_t posEqualSign;
	size_t posAndSign;
	string signature;
	string sTemp;

	if (NULL == query)
	{
		WX_LOG(("ERROR: Invalid URL."));
		return Error_Invalid_Url;
	}

	sTemp.assign(query);
	posToken = sTemp.find("signature");
	if (string::npos == posToken)
	{
		WX_LOG(("ERROR: Invalid Query: %s.", query));
		return Error_Invalid_Signature_Token;
	}
	else
	{
		posEqualSign = sTemp.find("=");
		if (string::npos == posEqualSign)
		{
			WX_LOG(("ERROR: Invalid Query: %s.", query));
			return Error_Invalid_Signature_Equal_Sign;
		}
		posAndSign = sTemp.find("&");
		if (string::npos == posAndSign)
		{
			WX_LOG(("ERROR: Invalid Query: %s.", query));
			return Error_Invalid_Signature_And_Sign;
		}
		signature = sTemp.substr(posEqualSign, posAndSign - posEqualSign);
		WX_LOG(("INFO: Signature is %s.", signature.c_str()));
	}
}
