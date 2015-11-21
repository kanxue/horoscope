
#ifndef STORAGE_HESAPIERRNO_H_
#define STORAGE_HESAPIERRNO_H_

#include <string>
#include <map>

enum HesApiErrNo {
    API_OK = 0,
    API_SYS_ERR = -1,

    // invalid args.
    API_INVALID_USERNAME = 10001,
    API_INVALID_PASSWORD = 10002,
    API_INVALID_TOKEN = 10003,
    API_INVALID_JSON_FORMAT = 10004,

    // logic failed.
    API_TOKEN_DIFF_CLIENT_IP = 20001,
    API_TOKEN_EXPIRED = 20002,
}; // enum HesApiErrNo

std::string HesApiErrMsg(int code);

#endif // STORAGE_HESAPIERRNO_H_

