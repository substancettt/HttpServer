#include <string>
#include <string.h>
#include "weixin.h"



using namespace std;

int getTokenValue(char * query, const char * token, string & value)
{
	size_t posToken = 0;
	size_t posEqualSign = 0;
	size_t posAndSign = 0;
	string sTemp;

	sTemp.assign(query);
	posToken = sTemp.find("signature");
	if ((NULL == token) && (string::npos == posToken))
	{
		WX_LOG(("ERROR: Invalid Query: %s.", query));
		return Return_Code_Invalid_Signature_Token;
	}
	else
	{
		posEqualSign = sTemp.find("=");
		if (string::npos == posEqualSign)
		{
			WX_LOG(("ERROR: Invalid Query: %s.", query));
			return Return_Code_Invalid_Signature_Equal_Sign;
		}
		posAndSign = sTemp.find("&");
		if (string::npos == posAndSign)
		{
			WX_LOG(("ERROR: Invalid Query: %s.", query));
			return Return_Code_Invalid_Signature_And_Sign;
		}
		value = sTemp.substr(posEqualSign, posAndSign - posEqualSign);
		WX_LOG(("INFO: %s is %s.", token, value.c_str()));
	}

	query = query + posAndSign;
	return Return_Code_OK;
}

int wx_validate(const char * query)
{
	int ret = Return_Code_OK;
	string signature;
	string echostr;
	string timestamp;
	string nonce;
	char * pos = (char *)query;

	if (NULL == query)
	{
		WX_LOG(("ERROR: Invalid URL."));
		return Return_Code_Invalid_Url;
	}

	ret = getTokenValue(pos, "signature", signature);
	if (ret > 0)
	{
		return ret;
	}

	ret = getTokenValue(pos, "echostr", echostr);
	if (ret > 0)
	{
		return ret;
	}

	ret = getTokenValue(pos, "timestamp", timestamp);
	if (ret > 0)
	{
		return ret;
	}

	ret = getTokenValue(pos, "nonce", nonce);
	if (ret > 0)
	{
		return ret;
	}
}
