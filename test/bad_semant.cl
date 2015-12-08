class C {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : C {
           {
		a <- x;
		b <- y;
		self;
           }
	};
};
(*
class D inherits C {
    init():Object {}; -- Method 'init' in class D overrides another method with same name and different signature from class C
};
*)


Class Main {
	main():C {
	 {
	  (new C).init(1,1);        -- Method 'C.init' parameter #1 type mismatch. Actual: 'Int' should be a subtype of 'Bool'
	  (new C).init(1,true,3);   -- Method 'C.init' formal parameter count (2) does not match actual parameter count (3)
	  (new C).iinit(1,true);    -- No such method: 'C.iinit'
	  let a:Int <- a*5 in {};   -- 'a' is not declared
	  let c:C <- new C in {
	      c<-5;
	  };
	  (new C);
	 }
	};
};
