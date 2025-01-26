#import <Foundation/Foundation.h>
@implementation MyClass1 : NSObject
{
    int a;
}
- (void) setA: (int) n {
    a = n;
}
@end

@interface Test : NSObject
- (void) testNumber: (int) n;
- (void) testArray: (int[]) a;
- (void) testString: (char []) c;
- (void) testNSString: (NSString*) s;
@end

@implementation Test
- (void) testNumber: (int) n {
    n = n + 10;
}

- (void) testArray: (int[]) a {
    a[0] = a[0] + 10;
}

- (void) testString: (char []) c {
    c[0] = 'a';
}

-(void) testNSString: (NSString*) s {
    s = @"test";
}

- (void) testObject: (MyClass1*) o {
    [o setA: 157];
}
@end

int main (int argc, const char * argv[])
{
   NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    Test *test = [Test new];
    NSLog (@"Test number");
    int n = 4;
    NSLog (@"%d", n);
    [test testNumber: n];
    NSLog (@"%d", n);
    
    NSLog (@"Test array");
    int arr[] = {1, 2, 3};
    NSLog (@"%d", arr[0]);
    NSLog (@"%d", arr[1]);
    NSLog (@"%d", arr[2]);
    [test testArray: arr];
    NSLog (@"%d", arr[0]);
    NSLog (@"%d", arr[1]);
    NSLog (@"%d", arr[2]);
    
   // NSLog (@"Test string");
//    char str[] = "c str";
 //   NSLog (@"Test string");
  //  [InOutFuncs printCharArray: str];
//    [test testString: str];
 //   [InOutFuncs printCharArray: str];
   
    NSLog (@"Test NSString");
    
    NSString *nsstr = @"nsstr";
    NSLog (nsstr);
    [test testNSString: nsstr];
    NSLog (nsstr);

    

   NSLog (@"Test object");
   MyClass1 *obj = [MyClass1 new];
   [obj setA: 10];
   NSLog (@"%d", obj->a);
   [test testObject: obj];
   NSLog (@"%d", obj->a);
   NSLog (@"hello world");
   [pool drain];
   return 0;
}