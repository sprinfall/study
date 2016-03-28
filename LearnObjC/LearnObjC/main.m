//
//  main.m
//  LearnObjC
//
//  Created by csi on 16/3/28.
//  Copyright (c) 2016年 Adam Gu. All rights reserved.
//

#import <Foundation/Foundation.h>


// NSArray

void testNSArray() {

  // Create

  {
    NSArray *someArray = [NSArray arrayWithObject:@"string0"];
    NSUInteger count = [someArray count];
    assert(count == 1);
  }

  {
    NSArray *someArray = [NSArray arrayWithObjects:@"string0", @"string1", @"string2", nil];
    NSUInteger count = [someArray count];
    assert(count == 3);
  }

  {
    NSArray *someArray = [[NSArray alloc] init];
    assert([someArray count] == 0);
  }

  {
    NSArray *someArray = [[NSArray alloc] initWithObjects:@"string0", @"string1", @"string2", nil];
    NSUInteger count = [someArray count];
    assert(count == 3);
  }

  {
    NSArray *someArray = @[ @"string0", @"string1", @"string2" ];
    assert([someArray count] == 3);
  }

  {
    NSArray *someArray = @[];
    assert([someArray count] == 0);
  }


  {
    NSMutableString *str0 = [NSMutableString stringWithString:@"string0"];
    NSMutableString *str1 = [NSMutableString stringWithString:@"string1"];
    NSMutableString *str2 = [NSMutableString stringWithString:@"string2"];

    NSArray *someArray = @[ str0, str1, str2 ];
    NSArray *someOtherArray = [[NSArray alloc] initWithArray:someArray];
    NSArray *someOtherCopiedArray = [[NSArray alloc] initWithArray:someArray copyItems:YES];

    [str0 setString:@""];

    assert([someArray[0] length] == 0);
    assert([someOtherArray[0] length] == 0);
    assert([someOtherCopiedArray[0] isEqualToString:@"string0"]);
  }

}


int main(int argc, const char * argv[]) {
  @autoreleasepool {
    NSLog(@"Hello, World!");

    testNSArray();
  }

  return 0;
}
