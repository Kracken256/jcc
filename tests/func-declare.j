struct uuid_t {
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

func main(): int {
    `printf("Hello, World!\n");`
    return 0
}
