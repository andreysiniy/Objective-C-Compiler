#import <Foundation/Foundation.h>

int main (int argc, const char * argv[])
{
   NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int a = 10 + 20;
    int b = 10 + 'a';
    int c = 'a' + 20;
    int d = 'a' + 'b';
    char e = 10 + 20;
    char f = 10 + 'a';
    char g = 'a' + 10;
    char h = 'a' + 'b';
    int a1 = 10 - 20;
    int b1 = 10 - 'a';
    int c1 = 'a' - 20;
    int d1 = 'a' - 'b';
    int a2 = 10 * 20;
    int b2 = 10 * 'a';
    int c2 = 'a' * 20;
    int d2 = 'a' * 'b';
    int a3 = 10 / 20;
    int b3 = 10 / 'a';
    int c3 = 'a' / 20;
    int d3 = 'a' / 'b';
    int a4 = 10 == 20;
    int b4 = 10 == 'a';
    int c4 = 'a' == 20;
    int d4 = 'a' == 'b';
    int a5 = 10 != 20;
    int b5 = 10 != 'a';
    int c5 = 'a' != 20;
    int d5 = 'a' != 'b';
    int a6 = 10 < 20;
    int b6 = 10 < 'a';
    int c6 = 'a' < 20;
    int d6 = 'a' < 'b';
    int a7 = 10 > 20;
    int b7 = 10 > 'a';
    int c7 = 'a' > 20;
    int d7 = 'a' > 'b';
    int a8 = 10 <= 20;
    int b8 = 10 <= 'a';
    int c8 = 'a' <= 20;
    int d8 = 'a' <= 'b';
    int a9 = 10 >= 20;
    int b9 = 10 >= 'a';
    int c9 = 'a' >= 20;
    int d9 = 'a' >= 'b';


    NSLog (@"Plus");

    NSLog (@"%d", a);
    NSLog (@"%d", b);
    NSLog (@"%d", c);
    NSLog (@"%d", d);
    NSLog (@"%c", e);
    NSLog (@"%c", f);
    NSLog (@"%c", g);
    NSLog (@"%c", h);

    
    NSLog (@"Minus");

    NSLog (@"%d", a1);
    NSLog (@"%d", b1);
    NSLog (@"%d", c1);
    NSLog (@"%d", d1);

    NSLog (@"Mul");

    NSLog (@"%d", a2);
    NSLog (@"%d", b2);
    NSLog (@"%d", c2);
    NSLog (@"%d", d2);

    NSLog (@"Div");

    NSLog (@"%d", a3);
    NSLog (@"%d", b3);
    NSLog (@"%d", c3);
    NSLog (@"%d", d3);

    NSLog (@"Equal");

    NSLog (@"%d", a4);
    NSLog (@"%d", b4);
    NSLog (@"%d", c4);
    NSLog (@"%d", d4);

    NSLog (@"NotEqual");

    NSLog (@"%d", a5);
    NSLog (@"%d", b5);
    NSLog (@"%d", c5);
    NSLog (@"%d", d5);

    NSLog (@"Less");

    NSLog (@"%d", a6);
    NSLog (@"%d", b6);
    NSLog (@"%d", c6);
    NSLog (@"%d", d6);

    NSLog (@"Greater");

    NSLog (@"%d", a7);
    NSLog (@"%d", b7);
    NSLog (@"%d", c7);
    NSLog (@"%d", d7);

    NSLog (@"LessEqual");

    NSLog (@"%d", a8);
    NSLog (@"%d", b8);
    NSLog (@"%d", c8);
    NSLog (@"%d", d8);

    NSLog (@"GreaterEqual");

    NSLog (@"%d", a9);
    NSLog (@"%d", b9);
    NSLog (@"%d", c9);
    NSLog (@"%d", d9);


   [pool drain];
   return 0;
}