struct Uuid {
    a: dword = 0
    b: word = 0
    c: word = 0
    d: word = 0
    e: dword = 0
    f: word = 0

    new: (a: dword, b: word, c: word, d: word, e: dword, f: word) {
        let u: Uuid
        
        u.a = a
        u.b = b
        u.c = c
        u.d = d
        u.e = e
        u.f = f

        return u
    }

    new: (bytes: byte[16]) {
        let u: Uuid
        
        u.a = bytes[0..3] as dword
        u.b = bytes[4..5] as word
        u.c = bytes[6..7] as word
        u.d = bytes[8..9] as word
        u.e = bytes[10..13] as dword
        u.f = bytes[14..15] as word

        return u
    }

    // Generating methods
    gen1: (): Uuid {

    }

    gen2: (): Uuid {

    }
    
    gen3: (namespace: Uuid, name: string): Uuid {
        
    }

    gen4: (sec:bool = false): Uuid {
        let bytes: byte[16]
        
        sec ? Std.crypto.genbytes(bytes) : Std.genbytes(bytes)

        bytes[6] = (bytes[6] & 0x0F) | 0x40
        bytes[8] = (bytes[8] & 0x3F) | 0x80

        return bytes as Uuid
    }

    gen5: (namespace: Uuid, name: string): Uuid {
        let digest: byte[16]

        // truncation is automatic
        Std.crypto.sha1(namespace.bytes() + name.bytes(), digest)

        digest[6] = (digest[6] & 0x0F) | 0x50
        digest[8] = (digest[8] & 0x3F) | 0x80

        return digest as Uuid
    }

    // Serializing methods
    toString: (): string => Std.i2h(a, 8, 'big') + '-' + Std.i2h(b, 4, 'big') + '-' + Std.i2h(c, 4, 'big') + '-' + Std.i2h(d, 4, 'big') + '-' + Std.i2h(e, 8, 'big') + '-' + Std.i2h(f, 4, 'big')
    toJson: (): string => '{"Uuid":"' + toString() + '"}'
    toXml: (): string => '<Uuid>' + toString() + '</Uuid>'
    toYaml: (): string => 'Uuid: ' + toString()

    toBytes: (): byte[16] => Std.pack('big', a, b, c, d, e, f) as byte[16]

    // comparison methods
    eq: (other: Uuid): bool => a == other.a && b == other.b && c == other.c && d == other.d && e == other.e && f == other.f
    ne: (other: Uuid): bool => !eq(other)
    gt: (other: Uuid): bool => a > other.a || (a == other.a && b > other.b) || (a == other.a && b == other.b && c > other.c) || (a == other.a && b == other.b && c == other.c && d > other.d) || (a == other.a && b == other.b && c == other.c && d == other.d && e > other.e) || (a == other.a && b == other.b && c == other.c && d == other.d && e == other.e && f > other.f)
    gte: (other: Uuid): bool => eq(other) || gt(other)
    lt: (other: Uuid): bool => !gte(other)
    lte: (other: Uuid): bool => !gt(other)

    // test if uuid is nil
    not: (): Uuid => a == 0 && b == 0 && c == 0 && d == 0 && e == 0 && f == 0
    is: (): bool => !not()
}

subsystem Std
{
    subsystem Convert {
        func jsonToXml(json: string): string! {
            return 'xml'
        }

        func xmlToJson(xml: string): string! {
            return 'json'
        }

        func jsonToYaml(json: string): string! {
            return 'yaml'
        }

        func yamlToJson(yaml: string): string! {
            return 'json'
        }

        func xmlToYaml(xml: string): string! {
            return 'yaml'
        }

        func yamlToXml(yaml: string): string! {
            return 'xml'
        }
    }

    subsystem Serialize {
        func serialize(T: Object, format: estring[
            'json',
            'xml',
            'yaml'
            'string'
        ] = 'json'): string! {
            switch format {
                case 'json':
                    return toJson(T)
                case 'xml':
                    return toXml(T)
                case 'yaml':
                    return toYaml(T)
                case 'string':
                    break
                
                // impossible
            }

            let [method: routine, found: bool] = Reflect.getMethod(T, 'toString', '():string')

            !found ? abort 'serialize: no toString method on type "' + T.typename() + '"'

            return method(T) as string
        }

        func toJson(T: Object): string! {
            let [method: routine, found: ool] = Reflect.getMethod(T, 'toJson', '():string')

            !found ? abort 'toJson: no toJson method on type "' + T.typename() + '"'

            return method(T) as string
        }

        func toXml(T: Object): string! {
            let [method: routine, found: bool] = Reflect.getMethod(T, 'toXml', '():string')

            !found ? return Convert.jsonToXml(toJson(T))

            return method(T) as string
        }

        func toYaml(T: Object): string! {
            let [method: routine, found: bool] = Reflect.getMethod(T, 'toYaml', '():string')

            !found ? return Convert.jsonToYaml(toJson(T))

            return method(T) as string
        }
    }
}

import Std.Serialize.serialize 
import Std.io.println as println
import Std.net.Http as Http

func sendApiRequest(): string! {
    let str = serialize(Uuid.gen4())
    var resp: Http.Response = Http.get('https://uuidtools.com/api/decode/' + str)
    
    return resp.body
}

func Main() {
    let [body: string, err: Exception] = sendApiRequest()
    if err {
        println(err.trace())
        Std.exit(Std.mkerrcode(err))
    }

    println(body)
}