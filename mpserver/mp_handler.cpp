#include "horoscope/mpserver/mp_handler.h"

#include "common/base/scoped_ptr.h"
#include "common/web/url.h"
#include "thirdparty/tinyxml/tinyxml.h"

#include "horoscope/mpserver/base_processor.h"

MpHandler::MpHandler(HttpServer* server)
    : BaseHandler("/mp_handler", server)
{}

MpHandler::~MpHandler()
{}

void MpHandler::Process(
    const std::string& uri,
    const std::string& input,
    std::string* output)
{
    scoped_ptr<BaseProcessor> auto_delete_processor(
        GenerateProcessor(uri, input, output));

    if (auto_delete_processor.get()) {
        auto_delete_processor->Run();
    }
}
