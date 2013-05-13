#include "Quaternion.h"
#include <tdd/tdd.h>

using tdd::String;

namespace roe {

Test(test that elements are public) {
	Quaternion a;
	a.w = .5; a.x = .25;
	a.y = 3.0; a.z = 42.0;
	assertClose(a.w,.5);
	assertClose(a.x,.25);
	assertClose(a.y,3.0);
	assertClose(a.z,42.0);
}

Test(test that constructor works) {
	Quaternion a(.5,.25,3.0,42.0);
	assertClose(a.w,.5);
	assertClose(a.x,.25);
	assertClose(a.y,3.0);
	assertClose(a.z,42.0);
}

Test(string conversion should work) {
	auto a = Quaternion(1.2,3.4,5.6,7.8);
	auto s = String(a);
	assertTrue(s.contains("1.2"));
	assertTrue(s.contains("3.4"));
	assertTrue(s.contains("5.6"));
	assertTrue(s.contains("7.8"));
	assertEqual(s, a.toString());
}

Test(comparison should work) {
	Quaternion a(1,2,3,4);
	Quaternion b(2,3,4,5);
	Quaternion c(1,2,3,4);
	assertEqual(a,a);
	assertEqual(a,c);
	assertEqual(b,b);
	assertUnequal(a,b);
	assertUnequal(b,c);
}

Test(assignment and copy construction should work) {
	Quaternion a(1,2,3,4);
	Quaternion b(a);
	Quaternion c;
	c = a;
	assertEqual(a,b);
	assertEqual(a,c);
}

Test(single minus should be implemented) {
	Quaternion a(1,2,-3,4);
	assertEqual(-a, Quaternion(-1,-2,3,-4));
}

Test(adding should work component-wise) {
	Quaternion a(1,2,3,4);
	Quaternion b(2,3,4,5);
	assertEqual(b+a, Quaternion(3,5,7,9));
	a += b;
	a += b;
	assertEqual(a, Quaternion(5,8,11,14));
}

Test(subtracting should work component-wise) {
	Quaternion a(1,2,3,4);
	Quaternion b(2,3,4,5);
	assertEqual(b-a, Quaternion(1,1,1,1));
	a -= b;
	a -= b;
	assertEqual(a, Quaternion(-3,-4,-5,-6));
}

Test(scalar multiplication and division should work component-wise) {
	Quaternion a(2,4,6,10);
	assertEqual(3.0*a, Quaternion(6,12,18,30));
	assertEqual(a*0.5, Quaternion(1,2,3,5));
	assertEqual(a/2.0, Quaternion(1,2,3,5));
	a /= 2.0;
	assertEqual(a, Quaternion(1,2,3,5));
	a *= 5.0;
	assertEqual(a, Quaternion(5,10,15,25));
}

Test(length works) {
	Quaternion a(2,2,-4,1);
	assertEqual(a.length2(),25);
	assertEqual(a.length(),5);
}

Test(conjugate works) {
	Quaternion a(2,2,-4,1);
	assertEqual(a.conjugated(),Quaternion(2,-2,4,-1));
	a.conjugate();
	assertEqual(a,Quaternion(2,-2,4,-1));
}

Test(normalization works) {
	Quaternion a(4,4,4,4);
	Quaternion b(-2,2,-4,1);
	assertEqual(a.normalized(), Quaternion(.5,.5,.5,.5));
	b.normalize();
	double f = 5.0;
	assertEqual(b,Quaternion(-2/f,2/f,-4/f,1/f));
}

Test(invert should work) {
	Quaternion a(2,-2,3,-4);
	Quaternion q(2,2,-3,4);
	q /= 33.0;
	assertEqual(a.inverted(), q);
	a.invert();
	assertEqual(a, q);
}

Test(squaring should work) {
	Quaternion a(2,-2,3,-4);
	assertEqual(a.squared(), Quaternion(-25,-8,12,-16));
	a.square();
	assertEqual(a, Quaternion(-25,-8,12,-16));
}

Test(quaternion muliplication should behave correctly) {
	Quaternion a(1,2,3,4);
	Quaternion b(2,3,4,5);
	assertEqual(a*b, Quaternion(-36,6,12,12));
	assertUnequal(b*a, Quaternion(-36,6,12,12));
	a *= b;
	assertEqual(a, Quaternion(-36,6,12,12));
}

} // namespace roe










