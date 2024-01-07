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

/*
    new: (uuidString: string): uuid_t {}

    copy: (uuid: uuid_t): uuid_t {}

    toString: (): string {}

    cmp: (other: uuid_t): int {}
*/
}

func hello(id: uuid_t, name: string[] ): void

`void println(string s) {
    printf("%s\n", s);
}`

`void print(string s) {
    printf("%s", s);
}`

func main(): int {
    `println("This is J++!");`
    return 0
}
