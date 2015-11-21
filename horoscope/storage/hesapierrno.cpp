
#include "hesapierrno.h"

std::string HesApiErrMsg(int code)
{
    static std::map<int, std::string> g_errmsg;

    if (g_errmsg.empty()) {
#define INSERT_ERRMSG_MAP(key, value) g_errmsg[key] = value

        INSERT_ERRMSG_MAP(API_OK, "ok");
        INSERT_ERRMSG_MAP(API_SYS_ERR, "system error");

        INSERT_ERRMSG_MAP(API_INVALID_USERNAME, "invalid username");
        INSERT_ERRMSG_MAP(API_INVALID_PASSWORD, "invalid password");
        INSERT_ERRMSG_MAP(API_INVALID_TOKEN, "invalid token");
        INSERT_ERRMSG_MAP(API_INVALID_JSON_FORMAT, "invalid json format");

        INSERT_ERRMSG_MAP(API_TOKEN_DIFF_CLIENT_IP, "wrong token's client ip");
        INSERT_ERRMSG_MAP(API_TOKEN_EXPIRED, "token has expired");

#undef INSERT_ERRMSG_MAP
    }

    std::map<int, std::string>::iterator it = g_errmsg.find(code);
    if (it != g_errmsg.end()) return it->second;
    else return "undefined error";
}
