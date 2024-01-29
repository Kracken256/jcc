subsystem std {
    subsystem io {
        struct Reader {
        }
    }
}

subsystem libmia {
    subsystem Format {
        region MIAHeader {
            magic: dword = 0
            version: word = 0
            progid: byte[16] 
            camid: byte[16]
            quality_index: byte = 0
            colormode: word = 0
            width: dword = 0
            height: dword = 0
            span: dword = 0
            pxsize: byte = 0
            compmode: byte = 0 
            usize: dword = 0
            oid: byte[16]
            pgid: byte[16]
            timestamp: qword = 0
            crc32: dword = 0
            reserved: byte[25]
        }
    }

    subsystem Parser: Format, ::std::io {
        struct Context {
            header: Format::MIAHeader
            src: ::std::io::Reader
        }
    }
}
