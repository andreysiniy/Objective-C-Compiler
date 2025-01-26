@class InOutFuncs;
@implementation MyClass1 : NSObject
+ (void) print {
    [InOutFuncs printCharArray: "call method print from class 'MyClass1'"];
}
@end


int main ()
{

    [MyClass1 print];

}
