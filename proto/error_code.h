#ifndef PROTO_ERROR_CODE_H_
#define PROTO_ERROR_CODE_H_

#include <map>
#include <string>

enum ErrorCode {
    Ok = 0,

    System_Error                   = 1000,

    // 10000 ~ 10999 http server error.
    Invalid_Request_Type           = 10000,
    Invalid_Request_Length         = 10001,
    Invalid_Request_Format         = 10002,
}; // enum ErrorCode

std::string ErrorMsg(int error_code);

#define AddErrorMsg(code, msg) msg_map[code] = msg

std::string ErrorMsg(int error_code)
{
    static std::map<int, std::string> msg_map;
    if (msg_map.empty()) {
        AddErrorMsg(Ok, "ok");
        AddErrorMsg(System_Error, "system error");
        AddErrorMsg(Invalid_Request_Type, "invalid request type");
        AddErrorMsg(Invalid_Request_Length, "invalid request length");
        AddErrorMsg(Invalid_Request_Format, "invalid request format");
    }

    std::map<int, std::string>::iterator it = msg_map.find(error_code);
    if (it != msg_map.end())
        return it->second;
    else
        return "unkown error";
}

#endif // PROTO_ERROR_CODE_H_

