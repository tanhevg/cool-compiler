
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

	static_str():String {
	    "C"
	};

};

class D inherits C {
    c: String;
	static_str():String {
	    "D"
	};
	d1() : Object {

	};

    d(d1:Int, d2:Int):Int {
        a * d1 + d2
    };
};

class E inherits D {
	static_str():String {
	    "E"
	};
    e() : Object {
        new IO.out_string("e\n")
    };
};

class Main inherits IO {

    e: E <- new E;

    abs(i:Int):Int {
        if (i < 0) then
            ~i
        else
            i
        fi
    };

    test_case() : Object {
        e
    };

    test1() : Int { {
        out_string("Enter a number: ");
        let i : Int <- in_int(), even : Bool in
        {
            even <- (i / 2) * 2 = i;
            if even then
                out_string("Even\n")
            else
                out_string("Odd\n")
            fi;
            out_int(i).out_string("\n");
            while not(abs(i) <= 1) loop
            {
                i <- i / 2;
                if not(even) then i <- ~i else {} fi;
                out_int(i).out_string("\n");
            } pool;
        };
        0;
    } };

    test2() : Int { {
        let tc:Object <- test_case() in
            case tc of
                e : E => out_string(e@D.static_str()).out_string("\n");
                d : D => out_string("D\n");
                c : C => out_string("C\n");
                o : Object => out_string("Object\n");
                io : IO => out_string("IO\n");
--                i : Int => out_int(i).out_string("\n");
                s : String => out_string(s).out_string("\n");
                main : Main => out_string("Main\n");
            esac;
        0;
    } };

    main(): Int {
        test2()
    };
};
