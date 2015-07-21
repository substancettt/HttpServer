#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>

#include "boost/foreach.hpp"
#include "weixin.h"
#include "SHA1.h"

using namespace std;

static string sToken;

void WeixinInterface::setToken(const string & token)
{
	sToken = token;
}

int WeixinInterface::isTokenValidationUrl(const char * query)
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

int WeixinInterface::getXmlField(const string & sPostData, const string & sField, string & sValue)
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

int WeixinInterface::setFieldInXml(tinyxml2::XMLDocument * pDoc,
				tinyxml2::XMLNode* pXmlNode,
				const char * pcFieldName,
				const string & value,
				bool bIsCdata)
{
    if(!pDoc || !pXmlNode || !pcFieldName)
    {
        return -1;
    }

    tinyxml2::XMLElement * pFiledElement = pDoc->NewElement(pcFieldName);
    if(NULL == pFiledElement)
    {
        return -1;
    }

    tinyxml2::XMLText * pText = pDoc->NewText(value.c_str());
    if(NULL == pText)
    {
        return -1;
    }

    pText->SetCData(bIsCdata);
    pFiledElement->LinkEndChild(pText);

    pXmlNode->LinkEndChild(pFiledElement);
    return 0;
}

int WeixinInterface::genTextMsgXml(const string & sToUserName,
				const string & sFromUserName,
				const string & sCreateTime,
				const string & sContent,
				const string & sMsgId,
				string & sResult)
{
    tinyxml2::XMLPrinter oPrinter;
    tinyxml2::XMLNode* pXmlNode = NULL;
    tinyxml2::XMLDocument * pDoc = new tinyxml2::XMLDocument();
    if(NULL == pDoc)
    {
        return -1;
    }

    pXmlNode = pDoc->InsertEndChild(pDoc->NewElement("xml"));
    if(NULL == pXmlNode)
    {
        DELETE_PTR(pDoc);
        return -1;
    }

    if(0 != setFieldInXml(pDoc, pXmlNode, "ToUserName", sToUserName, true))
    {
        DELETE_PTR(pDoc);
        return -1;
    }

    if(0 != setFieldInXml(pDoc, pXmlNode, "FromUserName", sFromUserName,true))
    {
        DELETE_PTR(pDoc);
        return -1;
    }

    if(0 != setFieldInXml(pDoc, pXmlNode, "CreateTime", sCreateTime, true))
    {
        DELETE_PTR(pDoc);
        return -1;
    }

    if(0 != setFieldInXml(pDoc, pXmlNode, "MsgType", "text", true))
    {
        DELETE_PTR(pDoc);
        return -1;
    }

    if(0 != setFieldInXml(pDoc, pXmlNode, "Content", sContent, true))
    {
        DELETE_PTR(pDoc);
        return -1;
    }

    if(0 != setFieldInXml(pDoc, pXmlNode, "MsgId", "9234567890abcdef", true))
    {
        DELETE_PTR(pDoc);
        return -1;
    }

    pDoc->Accept(&oPrinter);
    sResult = oPrinter.CStr();

    DELETE_PTR(pDoc);
    return 0;
}


unsigned int WeixinInterface::mapMsgType(const string & sType)
{
	map <string, Msg_Type>::iterator iter = dMsgTypeMap.find(sType);
	if (iter != dMsgTypeMap.end())
	{
		eMsgType = iter->second;
		return iter->second;
	}
	else
	{
		return Msg_Type_Invalid;
	}
}

void WeixinInterface::parseCommonPart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "ToUserName", sToUserName);
		getXmlField(sPostData, "FromUserName", sFromUserName);
		getXmlField(sPostData, "CreateTime", sCreateTime);
		getXmlField(sPostData, "MsgType", sMsgType);

		getXmlField(sPostData, "MsgId", sMsgId);

		WX_LOG(("INFO: Msg: ToUserName is %s.", sToUserName.c_str()));
		WX_LOG(("INFO: Msg: FromUserName is %s.", sFromUserName.c_str()));
		WX_LOG(("INFO: Msg: CreateTime is %s.", sCreateTime.c_str()));
		WX_LOG(("INFO: Msg: MsgType is %s.", sMsgType.c_str()));
		WX_LOG(("INFO: Msg: MsgId is %s.", sMsgId.c_str()));

		WX_LOG(("INFO: Msg: MsgType value is %d.", mapMsgType(sMsgType)));
	}

}

void WeixinInterface::parseTextPart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "Content", sContent);
		WX_LOG(("INFO: Msg: Content is %s.", sContent.c_str()));
	}
}

void WeixinInterface::parseImagePart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "PicUrl", sPicUrl);
		getXmlField(sPostData, "MediaId", sMediaId);
		WX_LOG(("INFO: Msg: PicUrl is %s.", sPicUrl.c_str()));
		WX_LOG(("INFO: Msg: MediaId is %s.", sMediaId.c_str()));
	}
}

void WeixinInterface::parseVoicePart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "MediaId", sMediaId);
		getXmlField(sPostData, "Format", sFormat);
		getXmlField(sPostData, "Recognition", sRecognition);
		WX_LOG(("INFO: Msg: MediaId is %s.", sMediaId.c_str()));
		WX_LOG(("INFO: Msg: Format is %s.", sFormat.c_str()));
		WX_LOG(("INFO: Msg: Recognition is %s.", sRecognition.c_str()));
	}
}

void WeixinInterface::parseVideoPart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "MediaId", sMediaId);
		getXmlField(sPostData, "ThumbMediaId", sThumbMediaId);
		WX_LOG(("INFO: Msg: MediaId is %s.", sMediaId.c_str()));
		WX_LOG(("INFO: Msg: ThumbMediaId is %s.", sThumbMediaId.c_str()));
	}
}

void WeixinInterface::parseLocationPart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "Location_X", sLocation_X);
		getXmlField(sPostData, "Location_Y", sLocation_Y);
		getXmlField(sPostData, "Scale", sScale);
		getXmlField(sPostData, "Label", sLabel);

		WX_LOG(("INFO: Msg: Location_X is %s.", sLocation_X.c_str()));
		WX_LOG(("INFO: Msg: Location_Y is %s.", sLocation_Y.c_str()));
		WX_LOG(("INFO: Msg: Scale is %s.", sScale.c_str()));
		WX_LOG(("INFO: Msg: Label is %s.", sLabel.c_str()));
	}
}

void WeixinInterface::parseLinkPart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "Title", sTitle);
		getXmlField(sPostData, "Description", sDescription);
		getXmlField(sPostData, "Url", sUrl);

		WX_LOG(("INFO: Msg: Title is %s.", sTitle.c_str()));
		WX_LOG(("INFO: Msg: Description is %s.", sDescription.c_str()));
		WX_LOG(("INFO: Msg: Url is %s.", sUrl.c_str()));
	}
}

void WeixinInterface::parseEventPart()
{
	if (sPostData.empty())
	{
		WX_LOG(("ERROR: Invalid Content."));
		return;
	}
	else
	{
		getXmlField(sPostData, "Event", sEvent);
		getXmlField(sPostData, "EventKey", sEventKey);

		WX_LOG(("INFO: Msg: Event is %s.", sEvent.c_str()));
		WX_LOG(("INFO: Msg: EventKey is %s.", sEventKey.c_str()));
	}
}

const char * WeixinInterface::wx_parseMsg(const char * content, size_t len)
{
	string response = "";

	if (NULL == content)
	{
		WX_LOG(("ERROR: Invalid Content."));
		return NULL;
	}
	else
	{
		sPostData.clear();
		sPostData.assign(content, len);
		WX_LOG(("INFO: Msg Content is\r\n%s\r\n", sPostData.c_str()));

		parseCommonPart();

		switch (eMsgType)
		{
			case Msg_Type_Text:
				parseTextPart();
				break;
			case Msg_Type_Image:
				parseImagePart();
				break;
			case Msg_Type_Voice:
				parseVoicePart();
				break;
			case Msg_Type_Video:
				parseVideoPart();
				break;
			case Msg_Type_Location:
				parseLocationPart();
				break;
			case Msg_Type_Link:
				parseLinkPart();
				break;
			case Msg_Type_Event:
				parseEventPart();
				break;
			case Msg_Type_Invalid:
			default:
				WX_LOG(("INFO: Invalid Message Type[%u].\r\n", eMsgType));
				break;
		}

		sContent = "It's a " + sMsgType + " Message. How niubi you are!";
		genTextMsgXml(sFromUserName,
				sToUserName,
				sCreateTime,
				sContent,
				sMsgId,
				response);
	}

    return response.c_str();
}

int WeixinInterface::getTokenValue(char ** query, const char * token, string & value)
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

string WeixinInterface::toHexString(const char * pInData)
{
	string hexStr;
	unsigned int len = strlen(pInData);
	for (unsigned int i = 0; i < len; i++)
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

bool WeixinInterface::validateSignature(string sSignature, string sTimeStamp, string sNonce)
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

const char * WeixinInterface::wx_validate(const char * query)
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

void WeixinInterface::wx_init(Options opt)
{
	dMsgTypeMap.insert(pair<string, Msg_Type>("text", Msg_Type_Text));
	dMsgTypeMap.insert(pair<string, Msg_Type>("image", Msg_Type_Image));
	dMsgTypeMap.insert(pair<string, Msg_Type>("voice", Msg_Type_Voice));
	dMsgTypeMap.insert(pair<string, Msg_Type>("video", Msg_Type_Video));
	dMsgTypeMap.insert(pair<string, Msg_Type>("location", Msg_Type_Location));
	dMsgTypeMap.insert(pair<string, Msg_Type>("link", Msg_Type_Link));
	dMsgTypeMap.insert(pair<string, Msg_Type>("event", Msg_Type_Event));

	setToken(opt.token);
}
