
#include "hes_verify_token_handler.h"

#include "common/encoding/json_to_pb.h"
#include "common/encoding/pb_to_json.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/storage/hesapierrno.h"
#include "horoscope/storage/storage_mysql_client.h"

#include "horoscope/hes/hes.pb.h"

HesVerifyTokenHandler::HesVerifyTokenHandler(HttpServer* server)
    : HesHandler("/verify", server)
{}

HesVerifyTokenHandler::~HesVerifyTokenHandler()
{}

int HesVerifyTokenHandler::Process(const HttpRequest* request, std::string* output)
{
    const std::string& http_body = request->http_body();
    std::string client_ip;
    request->headers().Get("X-Real-IP", &client_ip);
    hes::VerifyTokenRequest verify_token_request;
    std::string error;
    if (!JsonToProtoMessage(http_body, &verify_token_request, &error)) {
        LOG(ERROR)
            << "invalid json format. error [" << error
            << "] input [" << http_body << "]";
        return API_INVALID_JSON_FORMAT;
    }

    StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    uint32_t uid = 0;
    int ret = mysql_client.VerifyToken(
        verify_token_request.token(), client_ip, uid);
    if (ret != 0) {
        return ret;
    }

    hes::VerifyTokenResponse verify_token_response;
    verify_token_response.set_uid(uid);
    if (!ProtoMessageToJson(verify_token_response, output, &error, false)) {
        LOG(ERROR) << "API_SYS_ERR pb2json failed.";
        return API_SYS_ERR;
    }

    return 0;
}

