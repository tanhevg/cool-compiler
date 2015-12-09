class C inherits B {
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

class A {};
class B inherits A{};

Class Main {
	main():C {{
        (new C).init(1,1);          -- Method 'C.init' parameter #1 type mismatch. Actual: 'Int' should be a subtype of 'Bool'
        (new C).init(1,true,3);     -- Method 'C.init' formal parameter count (2) does not match actual parameter count (3)
        (new C).iinit(1,true);      -- No such method: 'C.iinit'
        let a:Int <- a*5 in {};     -- 'a' is not declared
        let c:C <- new C, x:Bool in {
            c<-5;                   -- Expression type is 'Int' should be a subtype of 'C'
            c <- isvoid(c);         -- Expression type is 'Bool' should be a subtype of 'C'
            c <- if x then
                new A
            else
                new B
            fi;                     -- Expression type is 'A' should be a subtype of 'C'
            case x of
                a:A => a <- new A;
                b:B => b <- new B;
                a:Int => 5;
                z:A => new B;       -- Branch with type 'A' declared multiple times in the same case statement
            esac;
            let a:A <-
                case x of
                    a:A => a <- new A;
                    b:B => b <- new B;
                    a:Int => 5;
                    z:C => new B;
                esac in {};         -- Let initializer type 'Object' should be a subtype of declared type 'A'
        };
        (new A);
	}};                             -- Method 'main' body type should be a subtype of the declared return type 'C'; found 'A'
};
