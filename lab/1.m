#import <Foundation/Foundation.h>

@interface HelloWorld : NSObject {
}
- (void) hello;
@end

@implementation HelloWorld
- (void) hello {
  printf("Hello World\n");
}
@end

int main (int argv, char *argc[]) {
  id o = [HelloWorld new];
  [o hello];
}
