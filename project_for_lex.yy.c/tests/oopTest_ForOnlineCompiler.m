#import <Foundation/Foundation.h>
@implementation MyClass1 : NSObject
- (void) print {
    NSLog (@"call method print from class 'MyClass1");
}
@end

@implementation MyClass2 : MyClass1
- (void) print {
    NSLog (@"call method print from class 'MyClass2");
}
@end

@implementation MyClass3 : MyClass1
- (void) print {
    NSLog (@"call method print from class 'MyClass3");
    [super print];
}
@end
 @implementation MyClass4 : MyClass2
 @end
int main (int argc, const char * argv[])
{
   NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    MyClass1 *obj1 = [MyClass1 new];
    MyClass2 *obj2 = [MyClass2 new];
    MyClass3 *obj3 = [MyClass3 new];
    
    MyClass1 *objects[] = { obj1, obj2, obj3, [MyClass4 new] };
    int i;
    for (i=0; i<4; i = i + 1) {
        [objects[i] print];
    }
   [pool drain];
   return 0;
}