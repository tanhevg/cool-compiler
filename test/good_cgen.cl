class C {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : C {
           {

           let s:String <- "QWERTY" in {
		a <- x;
		b <- y;
		};
		self;
           }
	};
};

class D inherits C {
    c: Strin;
};

class Main {
	main():C {
	  (new C).init(1,true)
	};
};
