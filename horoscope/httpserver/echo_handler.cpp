
#include "horoscope/httpserver/echo_handler.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"
#include "common/base/string/string_number.h"
#include "common/net/http/http_connection.h"
#include "common/net/http/http_request.h"
#include "common/net/http/http_response.h"

#include "thirdparty/glog/logging.h"

EchoHandler::EchoHandler()
{}

EchoHandler::~EchoHandler()
{}

void EchoHandler::HandleMessage(HttpConnection* http_connection)
{
    const HttpRequest& request = http_connection->http_request();
    const std::string& body = request.http_body();
    HttpResponse* response = http_connection->mutable_http_response();

    LOG(INFO) << "Get [" << request.uri()
              << "] body_size: " << body.size()
              << " body [" << body << "]";

    std::string resp_content = "<p>" + request.uri() + "<p><Body:{" + body + "}";

    response->set_status(HttpResponse::Status_OK);
    response->AddHeader("Content-Type", "text/html");
    response->AddHeader(
        "Content-Length", NumberToString(resp_content.size()));
    response->mutable_http_body()->assign("Body:{" + body + "}");
}

void EchoHandler::HandleHeaders(HttpConnection* http_connection)
{
    LOG(INFO) << "HandleHeaders";
}

void EchoHandler::HandleBodyPacket(
    HttpConnection* http_connection, const StringPiece& string_piece)
{
    LOG(INFO) << "HandleBodyPacket";
}

int EchoHandler::DetectBodyPacketSize(
    HttpConnection* http_connection, const StringPiece& data)
{
    LOG(INFO) << "DetectBodyPacketSize";
    std::string value;
    HttpResponse* http_response = http_connection->mutable_http_response();
    // Header field "Content-Length" exists.
    if (http_response->GetHeader("Content-Length", &value)) {
        int content_length = 0;
        if (StringToNumber(StringTrim(value), &content_length)) {
            LOG(INFO) << "content_length: " << content_length;
            return content_length;
        }
        return -1;
    }
    else if (http_response->GetHeader("Transfer-Encoding", &value)) {
        if (strcasecmp(value.c_str(), "chunked") == 0) {
            const char* begin = data.data();
            const char* p = strstr(begin, "\r\n");
            if (p != NULL) {
                int chunk_size = 0;
                if (sscanf(begin, "%x", &chunk_size) != 1) { // NOLINT(runtime/printf)
                    return -1;
                }
                begin = p + 2;
                if (chunk_size == 0) {
                    return begin + 2 - data.data();
                } else {
                    // "\r\n" is appended to the end of chunk
                    return chunk_size + 2 + (begin - data.data());
                }
            } else {
                // not receive a complete chunk
                return 0;
            }
        }
    }
    return 0;
}

SimpleEchoHandler::SimpleEchoHandler()
{}

SimpleEchoHandler::~SimpleEchoHandler()
{}

void SimpleEchoHandler::HandleGet(
    const HttpRequest* request,
    HttpResponse* response,
    Closure<void>* done)
{
    LOG(INFO) << "request " << (void*)request
              << " response " << (void*)response
              << " done " << (void*)done;
    std::string uri = request->uri();
    std::string content = request->ToString();
    std::string resp_content;
    StringAppend(&resp_content, "<html>");
    StringAppend(&resp_content, "<p>Uri<p>", uri, "<p>");
    StringAppend(&resp_content, "<p>Header<p>",
                 request->headers().ToString(), "<p>");
    StringAppend(&resp_content, "<p>Content<p>",
                 request->http_body(), "<p>");
    StringAppend(&resp_content, "<p>Hello from boxer<p>");
    StringAppend(&resp_content, "</html>");
    response->SetHeader("Content-Type", "text/html");
    response->SetHeader("Content-Length",
                        NumberToString(resp_content.size()));
    response->set_body(resp_content);

    done->Run();
}
