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

enum Error_Code
{
	Return_Code_OK = 0,
	Return_Code_Invalid_Url = 1,
	Return_Code_Invalid_Token = 2,
	Return_Code_Invalid_Equal_Sign = 2,
	Return_Code_Invalid_And_Sign = 2,
	Return_Code_Code = 3
};

void setToken(const char * token);
int isTokenValidationUrl(const char * query);
const char * wx_replyMsg(const char * content);
const char * wx_validate(const char * query);
