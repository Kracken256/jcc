func SayHi(name: string = "God") {
    print("Hello \(name)!")

    // use `var` to declare a thread-safe variable
    var counter = 0
    var callbacks: bit[3]

    go func() {
        // create a new routine
        print("Hello \(name), \(counter++)!")
    }().then(callbacks[0]=true)

    go func() {
        // create a new routine 
        print("Hello \(name), \(counter++)!")
    }().then(callbacks[1]=true)

    go func() {
        // create a new routine
        print("Hello \(name), \(counter++)!")
    }().then(callbacks[2]=true)

    // wait for all routines to finish
    wait(callbacks.every(x => x))
}

export func Main() {
    SayHi("User")
}