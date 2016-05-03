class Main inherits IO {
    five(): Int {

    };
    six() : Int {
        6
    };
    seven() : Int {
        7
    };
    sum(x:Int, y:Int, z:Int):Int {
        x + y + z
    };
    test() : SELF_TYPE {
        out_int(sum(five(), six(), seven())).
        out_string("foo\n")
    };

    main(): SELF_TYPE {
        test()
    };
};