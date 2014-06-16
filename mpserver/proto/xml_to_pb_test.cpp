#include <iostream>

#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/mpserver/proto/test.pb.h"

int main(int argc, char** argv)
{
    std::string xml = "<xml><b_1><![CDATA[b_1]]></b_1><b_2><a_1><![CDATA[b_2_a_1]]></a_1></b_2><b_3><a_1><![CDATA[b_3_a_1]]></a_1><a_2>true</a_2><a_3>0</a_3></b_3><b_3><a_1><[CDATA[b_3_a_1]]></a_1><a_2>false</a_2><a_3>1</a_3></b_3><b_3><a_1><![CDATA[b_3_a_1]]></a_1><a_2>true</a_2><a_3>2</a_3></b_3></xml>";

    TestB message;
    std::string error;
    bool ret = XmlToProtoMessage(xml, &message, &error);
    if (ret) {
        std::cout << message.DebugString() << std::endl;
    } else {
        std::cout << "convert failed. error [" << error << "]\n";
    }

    return 0;
}

