#ifndef PROTO_HOROSCOPE_DEF_H_
#define PROTO_HOROSCOPE_DEF_H_

enum CmdId {
    // 11 ~ 100 only for test.
    CmdId_Echo = 10,

    // 101 ~ 10000 for app.
    CmdId_Horoscopr_Attr = 101,
}; // enum CmdId

#endif // PROTO_HOROSCOPE_DEF_H_

