# Very flexible enum definition
# It is just a set of constants

# Forwards declaration of enum
enum MenuOptions;

enum KernelBootMode {
    KKInit0: dword = 0x00008000;
    TaintedTMP: dword = 0x81000a00;
    EtxPanicked: qword = 0x0000000000000001;

    YadaYada: MenuOptions = MenuOptions.Burger;
};

enum MenuOptions {
    Fries: int = 1;
    Burger: int = 10;
    Soda: int = 20;
    Milkshake: int = 30;
    IceCream: int = 40;
    Coffee: int = 50;
    Tea: int = 60;
    Water: int = 70;
    Beer: int = 80;

    # Enums may contain mixed types
    Special: string = "Special";
};
