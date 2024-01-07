region uuid_t {
    @:serial_format('hex')
    @:endian('big')
    @:serial_suffix('-')
    a: dword = 0

    @:serial_format('hex')
    @:endian('big')
    @:serial_suffix('-')
    b: word = 0

    @:serial_format('hex')
    @:endian('big')
    @:serial_suffix('-')
    c: word = 0

    @:serial_format('hex')
    @:endian('big')
    @:serial_suffix('-')
    d: word = 0

    @:serial_format('hex')
    @:endian('big')
    e: dword = 0
    @:serial_format('hex')
    @:endian('big')
    f: word = 0
}

struct database_record_t {
    id: uuid_t
    name: string
    age: byte
    height: float
    weight: float
    is_married: bit
}

region database_t {
    records: database_record_t[10]
}

func main(): int {
    return 0
}
