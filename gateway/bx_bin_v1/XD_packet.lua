--old_sensor
XD_packet0 = { 0x7E, 0x45,    0x03, 0x84, 0x02,
    0x01, 0x32, 0x33,
    0xF1, 0xB1,  0x2D, 0x1A,  0x36, 0x21,  0x23, 0x49,  0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xDD, 0x00,
    0x7E}

--rain_sensor
XD_packet1 = { 0x7E, 0x45,    0x03, 0x84, 0x02,
    0x02,   0x32,   0x33,
    0xF4, 0x05,   0x0C, 0x05,   0x06, 0x03,   0x05, 0x04,   0x07, 0x00,   0x7B, 0x7B,   0x0A, 0x03,   0x01, --解析数据
    0x00, 0x00, --保留
    0x7E}

--TH_sensor
XD_packet2 = { 0x7E, 0x45,    0x03, 0x84, 0x02,
    0x03,   0x32, 0x33,
    0x0F, 0x00, 0x38,   0x2E, 0x5E,   0x0F, 0x00, 0x79,   0x24, 0x2F,   0x0F, 0x01, 0x17,   0x30, 0x57,   0x01, 0x47,  --解析数据
    0x7E}

--salt_sensor
XD_packet3 = { 0x7E, 0x45,    0x03, 0x84, 0x02,
    0x04,   0x80, 0x11,
    0x0F, 0x00, 0x38,   0x00, 0x21,   0x02, 0x03, 0x23,   0x7F, 0xF8,   0x00, 0x01, 0x25,   0x45, 0xF4,   0x01, 0x37,  --解析数据
    0x7E}

--shock_sensor
XD_packet4 = { 0x7E, 0x45,    0x03, 0x84, 0x02,
    0x05,   0x02, 0x10,
    0x01, 0xF5,   0x00, 0x00,   0x01, 0x1F, 0x03, 0x23, 0x05, 0x65,  0x00, 0x01, 0x25, 0x0C, 0x65, 0x00, 0x00, --解析数据
    0x7E}
