#import <Foundation/Foundation.h>
@implementation MyClass1 : NSObject
{
    int a;
}
@end

@implementation MyClass2 : MyClass1

- (void) func {
    a = 10;
}
@end

@implementation MyClass3 : NSObject
{
    MyClass1 *obj;
}

- (void) func {
    obj = [MyClass1 new];
    //obj->a = 15; //Ошибка
}
@end

int main ()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    MyClass1 *obj = [MyClass1 new];
    obj->a = 20;
    //[InOutFuncs printInt: obj->a];
    NSLog(@"%d", obj->a);
    [pool drain];
    return 0;
}
