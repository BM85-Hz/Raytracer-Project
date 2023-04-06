#include "common.h"

// Evaluate curve c at t and return the result.  If sub_a or sub_b are not null,
// perform subdivision on curve c at t as well.  If sub_a is not null, assign it
// the first piece (so that sub_a and c agree at their endpoint A.)  If sub_b is
// not null, assign it the second piece (so that sub_b and c agree at their
// endpoint D.)
vec2 Evaluate(const Curve& c, double t, Curve* sub_a, Curve* sub_b)
{
    //calc blended function first
    vec2 result_t(c.A*(1-t)*(1-t)*(1-t) + c.B*3*(1-t)*(1-t)*t + c.C*3*(1-t)*t*t + c.D*t*t*t);

    if(sub_a != nullptr){
        sub_a->A = c.A;
        sub_a->B = c.B*t + (1-t)*c.A;
        sub_a->C = t*(t*c.C + (1-t)*c.B) + (1-t)*(t*c.B+(1-t)*c.A);
        sub_a->D = result_t;
    }
    if(sub_b != nullptr){
        sub_b->A = result_t;
        sub_b->B = t*(c.D*t + (1-t)*c.C) + (1-t)*(t*c.C + (1-t)*c.B);
        sub_b->C = c.D*t + (1-t)*c.C;
        sub_b->D = c.D;
    }
    return result_t;
}

