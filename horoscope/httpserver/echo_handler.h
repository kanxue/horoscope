#ifndef HTTPSERVER_ECHO_HANDLER_H_
#define HTTPSERVER_ECHO_HANDLER_H__

#include "common/net/http/http_handler.h"

class EchoHandler : public HttpHandler {
public:
    EchoHandler();

    virtual ~EchoHandler();

    virtual void HandleMessage(HttpConnection* http_connection);
    virtual void HandleHeaders(HttpConnection* http_connection);
    virtual void HandleBodyPacket(
        HttpConnection* http_connection, const StringPiece& string_piece);
    virtual int DetectBodyPacketSize(
        HttpConnection* http_connection,const StringPiece& data);
}; // class EchoHandler

class SimpleEchoHandler {
public:
    SimpleEchoHandler();
    virtual ~SimpleEchoHandler();

public:
    void HandleGet(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done);

}; // class SimpleEchoHandler

#endif // HTTPSERVER_ECHO_HANDLER_H__

