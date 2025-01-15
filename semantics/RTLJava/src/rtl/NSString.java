package rtl;

public class NSString extends  NSObject {

    private String string;

    private NSString(String str) {
        string = str;
    }

    private NSString(NSString str) {
        string = str.string;
    }

    public static NSString stringStatic() {
        return new NSString("");
    }

    public static NSString stringWithCStringStatic(char[] s) {
        return new NSString(new String(s));
    }

    public static NSString stringWithStringStatic(NSString s) {
        return new NSString(s);
    }
}
