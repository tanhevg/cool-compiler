
class C {
	a : Int <- 5 * 8;
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
    d():Int {
        a + 2
    };
};

class Main inherits IO {
--	main():C {
--	  (new C).init(1,true)
--	};
    main(): Int {
        {
            out_string("ROARRR!!!!\n");
            out_int((new D).d());
            out_string("\n");
            0;
        }
    };
};
