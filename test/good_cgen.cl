
class C {
	a : Int <- 5 * 2 - (2+3);
	b : Bool;

	init(x : Int, y : Bool) : C {
           {

           let s:String <- "QWERTY" in {
		a <- x + 5;
		b <- y;
		};
		self;
           }
	};

};

class D inherits C {
    c: String;
    d(d1:Int, d2:Int):Int {
        a * d1 + d2
    };
};

class E inherits D {

};

class Main inherits IO {

    abs(i:Int):Int {
        if (i < 0) then
            ~i
        else
            i
        fi
    };

    test_case() : Object {
        new E
    };

    main(): Int {
        {
            out_string("Enter a number: ");
            -- let i : Int <- 8 in
            let i : Int <- in_int(), even : Bool in
            {
                even <- (i / 2) * 2 = i;
                if even then
                    out_string("Even\n")
                else
                    out_string("Odd\n")
                fi;
                -- todo chain calls
                out_int(i);
                out_string("\n");
                while not(abs(i) <= 1) loop
                {
                    i <- i / 2;
                    if not(even) then i <- ~i else {} fi;
                    out_int(i);
                    out_string("\n");
                } pool;
            };
            let tc:Object <- test_case() in
                case tc of
                    e : E => out_string("E");
                    d : D => out_string("D");
                    c : C => out_string("C");
                    o : Object => out_string("Object");
                    io : IO => out_string("IO");
                    main : Main => out_string("IO");
                esac;

            -- out_int((new D).d(8, 2));
            -- out_string("\n");
            0;
        }
    };
};
