class Z inherits IO {

    a():Int{};
    c:String;
};

class P inherits Z {
};

class Q inherits P {
    b():String{};
    b:Int;
};


class A {
    set_var(num : Int, s: Str, b:Bool) : SELF_TYPE {
	{
		let a:Int <- 5*8,  b:Str in
			{ --w
				not x;
				a*5;
				b.length();
			};
	}
    };
};



