#import <Cocoa/Cocoa.h>

int main () {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  NSApp = [NSApplication sharedApplication];

  NSWindow *window = [
    [NSWindow alloc]
    initWithContentRect:NSMakeRect(10.0, 10.0, 400.0, 400.0)
    styleMask:NSWindowStyleMaskTitled
    backing:NSBackingStoreBuffered
    defer:NO
  ];

  [window setTitle:@"Hello, World!"];
  [window makeKeyAndOrderFront:nil];

  [NSApp run];
  [NSApp release];
  [pool release];

  return 0;
}
