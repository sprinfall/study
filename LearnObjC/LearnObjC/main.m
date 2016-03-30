//
//  main.m
//  LearnObjC
//
//  Created by csi on 16/3/28.
//  Copyright (c) 2016年 Adam Gu. All rights reserved.
//

#import <Foundation/Foundation.h>


// NSArray

void testNSArray_Create() {

  // arrayWithObject
  {
    NSArray *someArray = [NSArray arrayWithObject:@"string0"];
    NSUInteger count = [someArray count];
    assert(count == 1);
  }

  // arrayWithObjects
  {
    NSArray *someArray = [NSArray arrayWithObjects:@"string0", @"string1", @"string2", nil];
    NSUInteger count = [someArray count];
    assert(count == 3);
  }

  // alloc ] init
  {
    NSArray *someArray = [[NSArray alloc] init];
    assert([someArray count] == 0);
  }

  // new
  {
    NSArray *someArray = [NSArray new];
    assert([someArray count] == 0);
  }

  // initWithObjects (end with nil)
  {
    NSArray *someArray = [[NSArray alloc] initWithObjects:@"string0", @"string1", @"string2", nil];
    NSUInteger count = [someArray count];
    assert(count == 3);
  }

  // Literal syntax
  {
    NSArray *someArray = @[ @"string0", @"string1", @"string2" ];
    assert([someArray count] == 3);
  }

  // Literal syntax - empty list
  {
    NSArray *someArray = @[];
    assert([someArray count] == 0);
  }

  // initWithArray:copyItems:
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


void testNSArray_Query() {

  {
    NSArray *someArray = @[ @"string0", @"string1", @"string2" ];

    assert([[someArray objectAtIndex:0] isEqualToString:@"string0"]);
    assert([someArray[0] isEqualToString:@"string0"]);

    BOOL contains = [someArray containsObject:@"string0"];
    assert(contains == YES);

    contains = [someArray containsObject:someArray[0]];
    assert(contains == YES);

    NSUInteger index = [someArray indexOfObject:@"string0"];
    assert(index == 0);

    index = [someArray indexOfObject:someArray[0]];
    assert(index == 0);
  }

}


void testNSArray_Sort() {

  // Use sortedArrayUsingSelector
  {
    NSArray *numbers = @[ @9, @3, @2 ];  // Array of NSNumber

    // NSNumber implements compare:
    NSArray *sortedNumbers = [numbers sortedArrayUsingSelector:@selector(compare:)];
    NSLog(@"Sorted numbers: %@", sortedNumbers);
  }

  // Use sortedArrayUsingComparator (block-based)
  {
    NSArray *numbers = @[ @9, @3, @2 ];

    NSArray *sortedNumbers = [numbers sortedArrayUsingComparator:^(id obj1, id obj2) {
      if ([obj1 integerValue] > [obj2 integerValue]) {
        return NSOrderedDescending;
      }
      if ([obj1 integerValue] < [obj2 integerValue]) {
        return NSOrderedAscending;
      }
      return NSOrderedSame;
    }];

    NSLog(@"Sorted numbers: %@", sortedNumbers);
  }

  // Use sortedArrayWithOptions (block-based)
  {
    NSArray *numbers = @[ @9, @3, @2 ];

    // NSSortOptions: NSSortConcurrent | NSSortStable
    NSArray *sortedNumbers = [numbers sortedArrayWithOptions:NSSortStable usingComparator:^(id obj1, id obj2) {
      if ([obj1 integerValue] > [obj2 integerValue]) {
        return NSOrderedDescending;
      }
      if ([obj1 integerValue] < [obj2 integerValue]) {
        return NSOrderedAscending;
      }
      return NSOrderedSame;
    }];

    NSLog(@"Sorted numbers: %@", sortedNumbers);
  }

  // Use sortedArrayUsingSelector to sort strings case insensitively.
  {
      NSArray *strings = @[@"gammaString", @"alphaString", @"betaString"];
      NSArray *sortedStrings = [strings sortedArrayUsingSelector:@selector(caseInsensitiveCompare:)];
      NSLog(@"Case insensitively sorted strings: %@", sortedStrings);
  }

}


int main(int argc, const char * argv[]) {
  @autoreleasepool {
    NSLog(@"Hello, World!");

    testNSArray_Create();
    testNSArray_Query();
    testNSArray_Sort();
  }

  return 0;
}
