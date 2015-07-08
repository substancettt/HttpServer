#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>

#include "weixin.h"
#include "SHA1.h"
#include "tinyxml2.h"

using namespace std;

static string sToken;

void setToken(const char * token)
{
	sToken.assign(token);
}

int isTokenValidationUrl(const char * query)
{
	if (NULL == query)
	{
		return false;
	}
	else
	{
		string sTemp;
		sTemp.assign(query);
		if (string::npos == sTemp.find("echostr"))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

int getXmlField(const string & sPostData, const string & sField, string & sValue)
{
    tinyxml2::XMLDocument xmlDoc;
    if(tinyxml2::XML_SUCCESS != xmlDoc.Parse(sPostData.c_str(), sPostData.size()))
    {
        return -1;
    }

    tinyxml2::XMLElement * xmlElement = xmlDoc.FirstChildElement("xml");
    if(NULL == xmlElement)
    {
        return -1;
    }

    tinyxml2::XMLElement * msgElement = xmlElement->FirstChildElement(sField.c_str());
    if(NULL == msgElement)
    {
        return -1;
    }

    const char * pText = msgElement->GetText();
    if(NULL == pText)
    {
        return -1;
    }

    sValue = pText;
    return 0;
}

int responseMsg(const char * content)
{
	string sToUserName;
	string sFromUserName;
	string sCreateTime;
	string sMsgType;
	string sContent;
	string sMsgId;

	if (NULL == content)
	{
		WX_LOG(("ERROR: Invalid Content."));
		return 1;
	}
	else
	{
		string sPostData;
		sPostData.assign(content);
		getXmlField(sPostData, "ToUserName", sToUserName);
		getXmlField(sPostData, "FromUserName", sFromUserName);
		getXmlField(sPostData, "CreateTime", sCreateTime);
		getXmlField(sPostData, "MsgType", sMsgType);
		getXmlField(sPostData, "Content", sContent);
		getXmlField(sPostData, "MsgId", sMsgId);

		WX_LOG(("INFO: Msg: ToUserName is %s.", sToUserName.c_str()));
		WX_LOG(("INFO: Msg: FromUserName is %s.", sFromUserName.c_str()));
		WX_LOG(("INFO: Msg: CreateTime is %s.", sCreateTime.c_str()));
		WX_LOG(("INFO: Msg: MsgType is %s.", sMsgType.c_str()));
		WX_LOG(("INFO: Msg: Content is %s.", sContent.c_str()));
		WX_LOG(("INFO: Msg: MsgId is %s.", sMsgId.c_str()));
	}

    return 0;
}

int getTokenValue(char ** query, const char * token, string & value)
{
	size_t posToken = 0;
	size_t posEqualSign = 0;
	size_t posAndSign = 0;
	string sTemp;

	sTemp.assign(*query);
	WX_LOG(("INFO: sTemp is %s, %s.", *query, sTemp.c_str()));
	posToken = sTemp.find("signature");
	if ((NULL == token) && (string::npos == posToken))
	{
		WX_LOG(("ERROR: Invalid Query: %s.", *query));
		return Return_Code_Invalid_Token;
	}
	else
	{
		posEqualSign = sTemp.find("=");
		if (string::npos == posEqualSign)
		{
			WX_LOG(("ERROR: Invalid Query: %s.", *query));
			return Return_Code_Invalid_Equal_Sign;
		}
		posAndSign = sTemp.find("&");
		if (string::npos == posAndSign)
		{
			if (posEqualSign == sTemp.find_last_of("="))
			{
				value = sTemp.substr(posEqualSign + 1);
			}
			else
			{
				WX_LOG(("ERROR: Invalid Query: %s.", *query));
				return Return_Code_Invalid_And_Sign;
			}
		}
		else
		{
			value = sTemp.substr(posEqualSign + 1, posAndSign - posEqualSign - 1);
		}
		WX_LOG(("INFO: %s is %s.", token, value.c_str()));
	}

	*query = *query + posAndSign + 1;
	return Return_Code_OK;
}

string toHexString(const char * pInData)
{
	string hexStr;
	unsigned int len = strlen(pInData);
	for (int i = 0; i < len; i++)
	{
		unsigned char highVal = (pInData[i] & 0xF0) >> 4;
		unsigned char lowVal = pInData[i] & 0x0F;
		if (highVal >= 0xA)
		{
			highVal = 'a' + (highVal - 0xA);
		}
		else
		{
			highVal = '0' + highVal;
		}
		if (lowVal >= 0xA)
		{
			lowVal = 'a' + (lowVal - 0xA);
		}
		else
		{
			lowVal = '0' + lowVal;
		}
		hexStr += highVal;
		hexStr += lowVal;
	}

	return hexStr;

}

bool validateSignature(string sSignature, string sTimeStamp, string sNonce)
{
	char sha1[21] = {0};

	string sSig;

    std::vector< std::string > vecStr;
    vecStr.push_back(sToken);
    vecStr.push_back(sTimeStamp);
    vecStr.push_back(sNonce);
    sort(vecStr.begin(), vecStr.end());
    string sStr = vecStr[0] + vecStr[1] + vecStr[2];

	CSHA1 *newCSHA1 = new CSHA1();
	newCSHA1->Reset();
	newCSHA1->Update((const unsigned char*)(sStr.c_str()), sStr.length());
	newCSHA1->Final();
	newCSHA1->GetHash((unsigned char *)sha1);

	sSig = toHexString(sha1);
	WX_LOG(("sSig is %s.\r\n", sSig.c_str()));

	if (0 == strcmp(sSignature.c_str(), sSig.c_str()))
	{
		WX_LOG(("Valid signature !"));
		return true;
	}
	else
	{
		WX_LOG(("Valid failed !"));
		return false;
	}

}

const char * wx_validate(const char * query)
{
	int ret = Return_Code_Invalid_Url;
	string signature;
	string timestamp;
	string nonce;
	string echostr;
	char * pos = (char *)query;

	if (NULL == query)
	{
		WX_LOG(("ERROR: Invalid URL."));
		return NULL;
	}

	ret = getTokenValue(&pos, "signature", signature);
	if (Return_Code_OK != ret)
	{
		return NULL;
	}

	ret = getTokenValue(&pos, "echostr", echostr);
	if (Return_Code_OK != ret)
	{
		return NULL;
	}

	ret = getTokenValue(&pos, "timestamp", timestamp);
	if (Return_Code_OK != ret)
	{
		return NULL;
	}

	ret = getTokenValue(&pos, "nonce", nonce);
	if (Return_Code_OK != ret)
	{
		return NULL;
	}

	if (validateSignature(signature, timestamp, nonce))
	{
		return echostr.c_str();
	}
	else
	{
		return NULL;
	}
}
