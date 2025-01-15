package rtl;

public class NSString extends  NSObject {

    private String string;

    private NSString(String str) {
        string = str;
    }

    private NSString(NSString str) {
        string = str.string;
    }

    /**
     * Создание пустой строки. <br/>
     * + (id)string
     * */
    public static NSString stringStatic() {
        return new NSString("");
    }

    /**
     * Создание строки на основе строки c. <br/>
     * + (id)stringWithCString: (const char *)byteString
     * */

    public static NSString stringWithCStringStatic(char[] s) {
        return new NSString(new String(s));
    }

    /**
     * Создание строки на основе другой строки. <br/>
     * + (id)stringWithString: (NSString *)aString
     * */
    public static NSString stringWithStringStatic(NSString s) {
        return new NSString(s);
    }
}
