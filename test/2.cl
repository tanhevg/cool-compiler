class Z inherits IO {

    a():Int{
        5*8
    };
    c:String <- new String;
};

class P inherits Z {
    b:Int <- 42;
};

class Q inherits P {
    b():String{};
    d:Int <- b+5;
};


class A {
    set_var(num : Int, s: String, b:Bool) : SELF_TYPE {

	{
		let a:Int <- 5*8,  b:String, x:Bool, q:Q<-new Q in
			{ --w
			    x <- isvoid q;
				not x;
				a*5;
				b <- q.b();
				x <- isvoid(b);
				b.substr(1,2);
			};
	    new A;
	}

    };
};



