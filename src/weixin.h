#include <string>
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
private:
	string sPostData;
	string sToUserName;
	string sFromUserName;
	string sCreateTime;
	string sMsgType;
	string sContent;
	string sTitle;
	string sDescription;
	string sUrl;
	string sLocation_X;
	string sMsgId;

	enum Error_Code
	{
		Return_Code_OK = 0,
		Return_Code_Invalid_Url = 1,
		Return_Code_Invalid_Token = 2,
		Return_Code_Invalid_Equal_Sign = 2,
		Return_Code_Invalid_And_Sign = 2,
		Return_Code_Code = 3
	};

public:
	const char * wx_parseMsg(const char * content, size_t len);
	const char * wx_validate(const char * query);
	void wx_setToken(const char * token);
	string & getToUserName();
	int isTokenValidationUrl(const char * query);

private:
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
	void parseCommonPart();
	int getTokenValue(char ** query, const char * token, string & value);
	string toHexString(const char * pInData);
	bool validateSignature(string sSignature, string sTimeStamp, string sNonce);
};
