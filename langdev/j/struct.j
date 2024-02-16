# All members must have default values
# All members must be public
# Static methods are members that don't have a `this` parameter at the start
# parameters are immutable by default. So `this` is immutable by default
# Use `&` to make a parameter mutable

struct Uuid {
    @format("%08x-");
    @endian("big");
    a: dword = 0;

    @format("%04x-");
    @endian("big");
    b: word = 0;

    @format("%04x-");
    @endian("big");
    c: word = 0;

    @format("%04x-");
    @endian("big");
    d: word = 0;

    @format("%08x");
    @endian("big");
    e: dword = 0;

    @format("%04x");
    @endian("big");
    f: word = 0;

    toString: (this): string;

    eq: (this, other: &Uuid): bool;
    lt: (this, other: &Uuid): bool;

    parse: (str: string): Uuid;
};

struct PersonAge {
    age: byte: 7 = 0;
    isAlive: bool = true;
};

struct Person {
    name: string = "";
    id: Uuid = default;
    personAge: PersonAge = default;

    new: (name: string, age: int, id: Uuid): Person;
    toString: (this): string;
    normalize: (&this): void;
};

struct PersonTable {
    people: Person[] = default;

    add: (&this, person: Person);
    remove: (&this, id: Uuid);
};
