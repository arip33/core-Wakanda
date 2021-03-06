var 
	charsetMap,
	reverseCharsetMap,
	flags,
	commands;
/*********Command constants*************/
exports.commands = {
COM_SLEEP: 					0x00,
COM_QUIT: 					0x01,
COM_INIT_DB:	 			0x02,
COM_QUERY: 					0x03,
COM_FIELD_LIST: 			0x04,
COM_CREATE_DB: 				0x05,
COM_DROP_DB: 				0x06,
COM_REFRESH: 				0x07,
COM_SHUTDOWN: 				0x08,
COM_STATISTICS: 			0x09,
COM_PROCESS_INFO: 			0x0a,
COM_CONNECT: 				0x0b,
COM_PROCESS_KILL: 			0x0c,
COM_DEBUG: 					0x0d,
COM_PING:					0x0e,
COM_TIME: 					0x0f,
COM_DELAYED_INSERT: 		0x10,
COM_CHANGE_USER: 			0x11,
COM_BINLOG_DUMP: 			0x12,
COM_TABLE_DUMP: 			0x13,
COM_CONNECT_OUT: 			0x14,
COM_REGISTER_SLAVE: 		0x15,
COM_STMT_PREPARE: 			0x16,
COM_STMT_EXECUTE: 			0x17,
COM_STMT_SEND_LONG_DATA: 	0x18,
COM_STMT_CLOSE: 			0x19,
COM_STMT_RESET: 			0x1a,
COM_SET_OPTION: 			0x1b,
COM_STMT_FETCH: 			0x1c
};

exports.serverStatus = {
 SERVER_STATUS_IN_TRANS : 1,
SERVER_STATUS_AUTOCOMMIT : 2, 
SERVER_MORE_RESULTS_EXISTS : 8, 
SERVER_QUERY_NO_GOOD_INDEX_USED : 16,
SERVER_QUERY_NO_INDEX_USED : 32,
SERVER_QUERY_WAS_SLOW : 2048,
SERVER_STATUS_CURSOR_EXISTS : 64
};

exports.flags = flags = {

		CLIENT_LONG_PASSWORD	: 1	,
		CLIENT_FOUND_ROWS	: 2	,
		CLIENT_LONG_FLAG	: 4	,
		CLIENT_CONNECT_WITH_DB	: 8	,
		CLIENT_NO_SCHEMA	: 16	,
		CLIENT_COMPRESS		: 32	,
		CLIENT_ODBC		: 64	,
		CLIENT_LOCAL_FILES	: 128	,
		CLIENT_IGNORE_SPACE	: 256	,
		CLIENT_PROTOCOL_41	: 512	,
		CLIENT_INTERACTIVE	: 1024	,
		CLIENT_SSL              : 2048	,
		CLIENT_IGNORE_SIGPIPE   : 4096    ,
		CLIENT_TRANSACTIONS	: 8192	,
		CLIENT_RESERVED         : 16384   ,
		CLIENT_SECURE_CONNECTION : 32768  ,
		CLIENT_MULTI_STATEMENTS : 65536   ,
		CLIENT_MULTI_RESULTS    : 131072  

},
exports.defaultFlag = 

		flags.CLIENT_LONG_PASSWORD
		| flags.CLIENT_FOUND_ROWS
		| flags.CLIENT_LONG_FLAG
		| flags.CLIENT_CONNECT_WITH_DB
		| flags.CLIENT_ODBC
		| flags.CLIENT_LOCAL_FILES
		| flags.CLIENT_IGNORE_SPACE
		| flags.CLIENT_PROTOCOL_41
		| flags.CLIENT_INTERACTIVE
		| flags.CLIENT_IGNORE_SIGPIPE
		| flags.CLIENT_TRANSACTIONS
		| flags.CLIENT_RESERVED
		| flags.CLIENT_SECURE_CONNECTION
		| flags.CLIENT_MULTI_STATEMENTS
		| flags.CLIENT_MULTI_RESULTS;

/************Languages/Charsets****************/
	
//The server_language: current server character set number. The complete list of character set numbers are in the end of this file
exports.charsetMap = charsetMap = {
        BIG5_CHINESE_CI:        1,
        LATIN2_CZECH_CS:        2,
        DEC8_SWEDISH_CI:        3,
        CP850_GENERAL_CI:       4,
        LATIN1_GERMAN1_CI:      5,
        HP8_ENGLISH_CI:         6,
        KOI8R_GENERAL_CI:       7,
        LATIN1_SWEDISH_CI:      8,
        LATIN2_GENERAL_CI:      9,
        SWE7_SWEDISH_CI:        10,
        ASCII_GENERAL_CI:       11,
        UJIS_JAPANESE_CI:       12,
        SJIS_JAPANESE_CI:       13,
        CP1251_BULGARIAN_CI:    14,
        LATIN1_DANISH_CI:       15,
        HEBREW_GENERAL_CI:      16,
        TIS620_THAI_CI:         18,
        EUCKR_KOREAN_CI:        19,
        LATIN7_ESTONIAN_CS:     20,
        LATIN2_HUNGARIAN_CI:    21,
        KOI8U_GENERAL_CI:       22,
        CP1251_UKRAINIAN_CI:    23,
        GB2312_CHINESE_CI:      24,
        GREEK_GENERAL_CI:       25,
        CP1250_GENERAL_CI:      26,
        LATIN2_CROATIAN_CI:     27,
        GBK_CHINESE_CI:         28,
        CP1257_LITHUANIAN_CI:   29,
        LATIN5_TURKISH_CI:      30,
        LATIN1_GERMAN2_CI:      31,
        ARMSCII8_GENERAL_CI:    32,
        UTF8_GENERAL_CI:        33,
        CP1250_CZECH_CS:        34,
        UCS2_GENERAL_CI:        35,
        CP866_GENERAL_CI:       36,
        KEYBCS2_GENERAL_CI:     37,
        MACCE_GENERAL_CI:       38,
        MACROMAN_GENERAL_CI:    39,
        CP852_GENERAL_CI:       40,
        LATIN7_GENERAL_CI:      41,
        LATIN7_GENERAL_CS:      42,
        MACCE_BIN:              43,
        CP1250_CROATIAN_CI:     44,
        LATIN1_BIN:             47,
        LATIN1_GENERAL_CI:      48,
        LATIN1_GENERAL_CS:      49,
        CP1251_BIN:             50,
        CP1251_GENERAL_CI:      51,
        CP1251_GENERAL_CS:      52,
        MACROMAN_BIN:           53,
        CP1256_GENERAL_CI:      57,
        CP1257_BIN:             58,
        CP1257_GENERAL_CI:      59,
        BINARY:                 63,
        ARMSCII8_BIN:           64,
        ASCII_BIN:              65,
        CP1250_BIN:             66,
        CP1256_BIN:             67,
        CP866_BIN:              68,
        DEC8_BIN:               69,
        GREEK_BIN:              70,
        HEBREW_BIN:             71,
        HP8_BIN:                72,
        KEYBCS2_BIN:            73,
        KOI8R_BIN:              74,
        KOI8U_BIN:              75,
        LATIN2_BIN:             77,
        LATIN5_BIN:             78,
        LATIN7_BIN:             79,
        CP850_BIN:              80,
        CP852_BIN:              81,
        SWE7_BIN:               82,
        UTF8_BIN:               83,
        BIG5_BIN:               84,
        EUCKR_BIN:              85,
        GB2312_BIN:             86,
        GBK_BIN:                87,
        SJIS_BIN:               88,
        TIS620_BIN:             89,
        UCS2_BIN:               90,
        UJIS_BIN:               91,
        GEOSTD8_GENERAL_CI:     92,
        GEOSTD8_BIN:            93,
        LATIN1_SPANISH_CI:      94,
        CP932_JAPANESE_CI:      95,
        CP932_BIN:              96,
        EUCJPMS_JAPANESE_CI:    97,
        EUCJPMS_BIN:            98,
        CP1250_POLISH_CI:       99,
        UCS2_UNICODE_CI:        128,
        UCS2_ICELANDIC_CI:      129,
        UCS2_LATVIAN_CI:        130,
        UCS2_ROMANIAN_CI:       131,
        UCS2_SLOVENIAN_CI:      132,
        UCS2_POLISH_CI:         133,
        UCS2_ESTONIAN_CI:       134,
        UCS2_SPANISH_CI:        135,
        UCS2_SWEDISH_CI:        136,
        UCS2_TURKISH_CI:        137,
        UCS2_CZECH_CI:          138,
        UCS2_DANISH_CI:         139,
        UCS2_LITHUANIAN_CI:     140,
        UCS2_SLOVAK_CI:         141,
        UCS2_SPANISH2_CI:       142,
        UCS2_ROMAN_CI:          143,
        UCS2_PERSIAN_CI:        144,
        UCS2_ESPERANTO_CI:      145,
        UCS2_HUNGARIAN_CI:      146,
        UTF8_UNICODE_CI:        192,
        UTF8_ICELANDIC_CI:      193,
        UTF8_LATVIAN_CI:        194,
        UTF8_ROMANIAN_CI:       195,
        UTF8_SLOVENIAN_CI:      196,
        UTF8_POLISH_CI:         197,
        UTF8_ESTONIAN_CI:       198,
        UTF8_SPANISH_CI:        199,
        UTF8_SWEDISH_CI:        200,
        UTF8_TURKISH_CI:        201,
        UTF8_CZECH_CI:          202,
        UTF8_DANISH_CI:         203,
        UTF8_LITHUANIAN_CI:     204,
        UTF8_SLOVAK_CI:         205,
        UTF8_SPANISH2_CI:       206,
        UTF8_ROMAN_CI:          207,
        UTF8_PERSIAN_CI:        208,
        UTF8_ESPERANTO_CI:      209,
        UTF8_HUNGARIAN_CI:      210
    };

reverseCharsetMap = {};

Object.getOwnPropertyNames(charsetMap).forEach(

    function addToReverseCharsetMap(name){
        reverseCharsetMap[charsetMap[name]] = name;
    }
);

exports.reverseCharsetMap = reverseCharsetMap;