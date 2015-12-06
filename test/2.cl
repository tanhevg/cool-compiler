class Z inherits IO {

    a():Int{
        5*8
    };
    c:String;
};

class P inherits Z {
};

class Q inherits P {
    b():String{"foo"};
    b:Int;
};


class A {
    set_var(num : Int, s: String, b:Bool) : SELF_TYPE {

	{
		let a:Int <- 5*8,  b:String, x:Bool, q:Q<-new Q in
			{ --w
				not x;
				a*5;
				b <- q.b();
				b.substr(1,2);
			};
	    new A;
	}

    };
};



