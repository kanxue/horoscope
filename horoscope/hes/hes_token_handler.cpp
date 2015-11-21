
#include "horoscope/hes/hes_token_handler.h"

#include "common/encoding/json_to_pb.h"
#include "common/encoding/pb_to_json.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/storage/hesapierrno.h"
#include "horoscope/storage/storage_mysql_client.h"

#include "horoscope/hes/hes.pb.h"

HesTokenHandler::HesTokenHandler(HttpServer* server)
    : HesHandler("/token", server)
{}

HesTokenHandler::~HesTokenHandler()
{}

int HesTokenHandler::Process(const HttpRequest* request, std::string* output)
{
    const std::string& http_body = request->http_body();
    std::string client_ip;
    request->headers().Get("X-Real-IP", &client_ip);
    hes::TokenRequest token_request;
    std::string error;
    if (!JsonToProtoMessage(http_body, &token_request, &error)) {
        LOG(ERROR)
            << "invalid json format. error [" << error
            << "] input [" << http_body << "]";
        return API_INVALID_JSON_FORMAT;
    }

    StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    std::string token;
    int ret = mysql_client.CheckUserPwd(
        token_request.username(), token_request.password(),
        client_ip, &token);
    if (ret != 0) {
        return ret;
    }

    hes::TokenResponse token_response;
    token_response.set_token(token);
    token_response.set_expired(mysql_client.GetTokenExpiredSeconds());
    if (!ProtoMessageToJson(token_response, output, &error, false)) {
        LOG(ERROR) << "API_SYS_ERR pb2json failed.";
        return API_SYS_ERR;
    }

    return 0;
}

