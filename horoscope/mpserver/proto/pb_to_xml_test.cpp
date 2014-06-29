#include <iostream>

#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/test.pb.h"

int main(int argc, char** argv)
{
    TestB message;
    message.set_b_1("b_1");
    message.mutable_b_2()->set_a_1("b_2_a_1");
    for (int idx = 0; idx < 3; ++idx) {
        TestA* a = message.add_b_3();
        a->set_a_1("b_3_a_1");
        a->set_a_2((idx % 2) == 0);
        a->set_a_3(idx);
    }

    std::string xml, error;
    bool ret = ProtoMessageToXmlWithRoot(message, &xml, &error);
    if (ret) {
        std::cout << xml << std::endl;
    } else {
        std::cout << "convert failed. error [" << error << "]\n";
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

