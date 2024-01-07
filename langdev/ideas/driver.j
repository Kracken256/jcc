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

    uuid_t(in:string) {
        this.a = in[0..8].hex2bin()
        this.b = in[9..13].hex2bin()
        this.c = in[14..18].hex2bin()
        this.d = in[19..23].hex2bin()
        this.e = in[24..36].hex2bin()
        this.f = in[37..41].hex2bin()
    }

    func toString(): string {
        return this.a.bin2hex() + '-' + this.b.bin2hex() + '-' + this.c.bin2hex() + '-' + this.d.bin2hex() + '-' + this.e.bin2hex() + '-' + this.f.bin2hex()
    }

    func toBytes(): byte[16] {
        var result: byte[16]
        result[0..4] = this.a.bin2bytes()
        result[4..6] = this.b.bin2bytes()
        result[6..8] = this.c.bin2bytes()
        result[8..10] = this.d.bin2bytes()
        result[10..16] = this.e.bin2bytes()
        return result
    }
}

func derive_rng_seed_from_bootkey(in: const byte[32], out: ref byte[12]): void {
    // Compute UUIDv5 from bootkey
    const nameSpace: uuid_t = '6ba7b810-9dad-11d1-80b4-00c04fd430c8'
    uuid_t result = @:crypto.uuid5(nameSpace, in)

    // Copy first 16 bytes of UUIDv5 to output
    out[0..12] = result.toBytes()[0..12]
}

namespace kernel {
    namespace driver {
        interface LogHandler {
            func debug(message: string): void
            func info(message: string): void
            func warn(message: string): void
            func error(message: string): void
            func panic(message: string): void

            func debug_progress(message: string): void
            func info_progress(message: string): void
            func warn_progress(message: string): void
            func error_progress(message: string): void
            func panic_progress(message: string): void

            func debug_done(message: string): void
            func info_done(message: string): void
            func warn_done(message: string): void
            func error_done(message: string): void
            func panic_done(message: string): void
        }

        type PanicHandler = func (string): null // does not return

        class DriverGeneric {
            var device_name: string = 'Generic Device'
            var device_id: uuid_t = '00000000-0000-0000-0000-000000000000'
            var device_version: string = '0.0.0' 
            var device_vendor: string = 'Generic Vendor'
            var device_description: string = 'Generic Device Description'
            var device_type: string = 'Generic Device Type'
            var device_class: uuid_t = '00000000-0000-0000-0000-000000000000'
            var device_subclass: uuid_t = '00000000-0000-0000-0000-000000000000'
            var driver_name: string = 'Generic Driver'
            var driver_id: uuid_t = '00000000-0000-0000-0000-000000000000'
            var driver_version: string = '0.0.0'
            var driver_vendor: string = 'Generic Vendor'
            var driver_description: string = 'Generic Driver Description'
            var driver_type: uuid_t = '00000000-0000-0000-0000-000000000000'
            var driver_class: uuid_t = '00000000-0000-0000-0000-000000000000'
            let logHandler: LogHandler = null
            let panicHandler: PanicHandler = null

            @:require(logHandler == null && panicHandler == null)
            func inject(logger: LogHandler, panic: PanicHandler): void {
                this.logHandler = logger
                this.panicHandler = panic
            }

            @:require(logHandler != null && panicHandler != null)
            virtual func init(): bool {
                logHandler.info_progress('Initializing ' + this.driver_name + ' driver for ' + this.device_name)
                logHandler.info_done('Initialized ' + this.driver_name + ' driver for ' + this.device_name)
                return true
            }

            @:require(logHandler != null && panicHandler != null)
            virtual func deinit(): bool {
                logHandler.info_progress('Deinitializing ' + this.driver_name + ' driver for ' + this.device_name)
                logHandler.info_done('Deinitialized ' + this.driver_name + ' driver for ' + this.device_name)
                return true
            }

            @:require(logHandler != null && panicHandler != null)
            func has_capability(capability: uuid_t): bool {
                return false
            }

            @:require(logHandler != null && panicHandler != null)
            func get_capability(capability: uuid_t): DynamicInvocable {
                return null
            }
        }

        namespace integrated {
            namespace tpm {
                class TPMBCE69124: DriverGeneric {
                    var device_name: string = 'TPM BCE 69124'
                    var device_id: uuid_t = 'bce69124-1e3e-4e4b-8e85-0c0d7f1f8a0a'
                    var device_version: string = '0.0.0'
                    var device_vendor: string = 'Broadcom'
                    var device_description: string = 'TPM 2.0'
                    var device_type: string = 'TPM'
                    var device_class: uuid_t = '00000000-0000-0000-0000-000000000000'
                    var device_subclass: uuid_t = '00000000-0000-0000-0000-000000000000'
                    var driver_name: string = 'TPM 2.0 Driver'
                    var driver_id: uuid_t = '00000000-0000-0000-0000-000000000000'
                    var driver_version: string = '0.0.0'
                    var driver_vendor: string = 'Broadcom'
                    var driver_description: string = 'TPM 2.0 Driver'
                    var driver_type: uuid_t = '00000000-0000-0000-0000-000000000000'
                    var driver_class: uuid_t = '00000000-0000-0000-0000-000000000000'

                    override func init(): bool {
                        logHandler.info_progress('Initializing ' + this.driver_name + ' driver for ' + this.device_name)
                        logHandler.info_done('Initialized ' + this.driver_name + ' driver for ' + this.device_name)
                        return true
                    }

                    override func deinit(): bool {
                        logHandler.info_progress('Deinitializing ' + this.driver_name + ' driver for ' + this.device_name)
                        logHandler.info_done('Deinitialized ' + this.driver_name + ' driver for ' + this.device_name)
                        return true
                    }

                    override func has_capability(capability: uuid_t): bool {
                        switch (capability) {
                            case '78c20a5c-9a8c-4e0c-8b9e-5c6e8e9c9c9b':
                                return true
                            default:
                                return false
                        }
                    }

                    override func get_capability(capability: uuid_t): DynamicInvocable {
                        switch (capability) {
                            case '78c20a5c-9a8c-4e0c-8b9e-5c6e8e9c9c9b':
                                return this.get_capability_tmp2_fetch_key()
                            default:
                                return null
                        }
                    }

                    private func get_capability_tmp2_fetch_key(): DynamicInvocable {
                        return new DynamicInvocable((id:uuid_t) => {
                            return @:crypto.uuidgen(v='5', n='random', s='random')
                        })
                    }

                }
            }
        }
    }
}

func main(): dword {
    return 0
}
