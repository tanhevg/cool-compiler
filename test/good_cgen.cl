class C {
	a : Int <- 3 + 2 * 5;
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
        5 + 2 * 3
    };
};

class Main {
--	main():C {
--	  (new C).init(1,true)
--	};
    main(): Int {
        (new D).d()
    };
};
