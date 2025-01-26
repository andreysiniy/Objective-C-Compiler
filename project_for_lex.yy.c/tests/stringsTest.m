@class InOutFuncs, NSString;

int main()
{
    NSString *nsstr = @"nsstr";
    NSString *nsstr2 = [nsstr stringByAppendingString: @"2"];
    NSString *nsstrToUpper = [nsstr uppercaseString];
    int notequal = [nsstr isEqual: nsstr2];
    int equal = [nsstr isEqual: nsstr];
    [InOutFuncs printNSString: nsstr2];
    [InOutFuncs printNSString: nsstrToUpper];
    [InOutFuncs printCharArray: "nsstr is equal to nsstr2"];
    [InOutFuncs printInt: notequal];
    [InOutFuncs printCharArray: "nsstr is equal to nsstr"];
    [InOutFuncs printInt: equal];
}
