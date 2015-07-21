#include <string>
#include <map>
#include <stddef.h>

#include "tinyxml2.h"

using namespace std;

#define DEBUG_FLAG 1
#ifdef DEBUG_FLAG
#define WX_LOG(x) do { printf("%-20s ", __func__); printf x; putchar('\n'); \
  fflush(stdout); } while(0)
#else
#define WX_LOG(x)
#endif

#define DELETE_PTR(ptr) \
    if (NULL != (ptr)) {\
        delete (ptr);\
        (ptr) = NULL;\
    }


class WeixinInterface
{
	enum Error_Code
	{
		Return_Code_OK = 0,
		Return_Code_Invalid_Url = 1,
		Return_Code_Invalid_Token = 2,
		Return_Code_Invalid_Equal_Sign = 2,
		Return_Code_Invalid_And_Sign = 2,
		Return_Code_Code = 3
	};

	enum Msg_Type
	{
		Msg_Type_Text = 1,
		Msg_Type_Image,
		Msg_Type_Voice,
		Msg_Type_Video,
		Msg_Type_Location,
		Msg_Type_Link,
		Msg_Type_Event,
		Msg_Type_Invalid
	};

	enum Event_Type
	{
		Event_Type_Subscribe = 1,
		Event_Type_Unsubscribe,
		Event_Type_Click
	};

public:
	struct Options
	{
		string token;
	};

private:
	string sPostData;
	string sToUserName;
	string sFromUserName;
	string sCreateTime;
	string sMsgType;
	string sContent;
	string sMediaId;
	string sFormat;
	string sRecognition;
	string sThumbMediaId;
	string sPicUrl;
	string sLocation_X;
	string sLocation_Y;
	string sScale;
	string sLabel;
	string sTitle;
	string sDescription;
	string sUrl;
	string sEvent;
	string sEventKey;

	string sMsgId;

	Msg_Type eMsgType;
	Event_Type eEventType;

	map <string, Msg_Type> dMsgTypeMap;

public:
	void wx_init(Options opt);
	const char * wx_parseMsg(const char * content, size_t len);
	const char * wx_validate(const char * query);
	string & getToUserName();
	int isTokenValidationUrl(const char * query);

private:
	void setToken(const string & token);
	int getXmlField(const string & sPostData, const string & sField, string & sValue);
	int setFieldInXml(tinyxml2::XMLDocument * pDoc,
					tinyxml2::XMLNode* pXmlNode,
					const char * pcFieldName,
					const string & value,
					bool bIsCdata);
	int genTextMsgXml(const string & sToUserName,
					const string & sFromUserName,
					const string & sCreateTime,
					const string & sContent,
					const string & sMsgId,
					string & sResult);
	unsigned int mapMsgType(const string & sType);
	void parseCommonPart();
	void parseTextPart();
	void parseImagePart();
	void parseVoicePart();
	void parseVideoPart();
	void parseLocationPart();
	void parseLinkPart();
	void parseEventPart();
	int getTokenValue(char ** query, const char * token, string & value);
	string toHexString(const char * pInData);
	bool validateSignature(string sSignature, string sTimeStamp, string sNonce);
};
