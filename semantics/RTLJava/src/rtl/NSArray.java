package rtl;

import java.lang.reflect.Array;
import java.util.Arrays;

public class NSArray extends NSObject {

    private NSObject[] array;
    private NSArray() {
        array = new NSObject[0];
    }

    private NSArray(NSArray array) {
        this.array = array.array;
    }

    private NSArray(NSObject[] array) {
        this.array = array;
    }
}
