class Q inherits Q {};
class Z inherits IO {};
(*classB{
};*)

class P inherits R {};
class O inherits P {};
class R inherits O {};

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



