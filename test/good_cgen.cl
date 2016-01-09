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

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};
