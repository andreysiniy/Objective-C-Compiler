@interface Complex : NSObject
{
double _re; //инвариант для действительной части
double _im; //инвариант для мнимой части
NSString *_format; //строка формата для метода description
}
- (id)initWithRe: (double)re andIm: (double)im; //специализированный конструктор
+ (Complex *)complexWithRe: (double)re andIm: (double)im; //метод класса для одноэтапного создания объекта
- (Complex *)add: (Complex *)other; //метод для сложения
- (Complex *)sub: (Complex *)other; //метод для вычетания
- (NSString *)format; //метод доступа к _format
- (void)setFormat: (NSString *)format; //метод установки _format
- (double)re; //остальные методы доступа к действительной и мнимой частям
- (void)setRe: (double)re;
- (double)im;
- (void)setIm: (double)im;
@end