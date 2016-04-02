//
//  TestNSArray.m
//  LearnObjC
//
//  Created by Adam Gu on 4/2/16.
//  Copyright © 2016 Adam Gu. All rights reserved.
//

#import <Foundation/Foundation.h>


// NSArray

void testNSArray_Create() {
  
  // alloc ] init
  {
    NSArray *array = [[NSArray alloc] init];
    assert(array.count == 0);
  }
  
  // new
  {
    NSArray *array = [NSArray new];
    assert(array.count == 0);
  }
  
  // array
  {
    NSArray *array = [NSArray array];
    assert(array.count == 0);
  }
  
  // arrayWithObject
  {
    NSArray *array = [NSArray arrayWithObject:@"string0"];
    assert(array.count == 1);
  }
  
  // arrayWithObjects
  {
    NSArray *array = [NSArray arrayWithObjects:@"string0", @"string1", @"string2", nil];
    assert(array.count == 3);
  }
  
  // initWithObjects (end with nil)
  {
    NSArray *array = [[NSArray alloc] initWithObjects:@"string0", @"string1", @"string2", nil];
    assert(array.count == 3);
  }
  
  // Literal syntax
  {
    NSArray *array = @[ @"string0", @"string1", @"string2" ];
    assert([array count] == 3);
  }
  
  // Literal syntax - empty list
  {
    NSArray *array = @[];
    assert([array count] == 0);
  }
  
  // initWithArray:copyItems:
  {
    NSMutableString *str0 = [NSMutableString stringWithString:@"string0"];
    NSMutableString *str1 = [NSMutableString stringWithString:@"string1"];
    NSMutableString *str2 = [NSMutableString stringWithString:@"string2"];
    
    NSArray *array = @[ str0, str1, str2 ];
    NSArray *anotherArray = [[NSArray alloc] initWithArray:array];
    NSArray *anotherCopiedArray = [[NSArray alloc] initWithArray:array copyItems:YES];
    
    [str0 setString:@""];
    
    assert([array[0] length] == 0);
    assert([anotherArray[0] length] == 0);
    assert([anotherCopiedArray[0] isEqualToString:@"string0"]);
  }
  
}  // testNSArray_Create


void testNSArray_Query() {
  
  {
    NSArray *array = @[ @"string0", @"string1", @"string2" ];
    
    assert([[array objectAtIndex:0] isEqualToString:@"string0"]);
    assert([array[0] isEqualToString:@"string0"]);
    
    BOOL contains = [array containsObject:@"string0"];
    assert(contains == YES);
    
    contains = [array containsObject:array[0]];
    assert(contains == YES);
    
    NSUInteger index = [array indexOfObject:@"string0"];
    assert(index == 0);
    
    index = [array indexOfObject:array[0]];
    assert(index == 0);
  }
  
}  // testNSArray_Query


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
  
}  // testNSArray_Sort

void testNSArray_Enumeration() {
  NSArray *array = @[ @1, @2, @3, @4, @5 ];
  
  // Use objectAtIndex (via subscripting)
  {
    for (NSUInteger i = 0; i < array.count; i++) {
      // id obj = array[i];
      // id obj = [array objectAtIndex:i];
    }
  }
  
  // Use enumerator (old school)
  {
    NSMutableArray *mutableArray = [NSMutableArray array];
    NSEnumerator *enumerator = [array objectEnumerator];
    id obj = nil;
    while ((obj = [enumerator nextObject]) != nil) {
      // ...
    }
  }
  
  // Classic enumeration (for each)
  {
    NSMutableArray *mutableArray = [NSMutableArray array];
    for (id obj in array) {
      // ...
    }
  }
  
}  // testNSArray_Enumeration


// Return YES if the object is a number > 0.
static
BOOL filterObject(id obj) {
  if ([obj isKindOfClass:NSNumber.class]) {
    NSNumber *number = (NSNumber *)obj;
    if ([number integerValue] > 0) {
      return YES;
    }
  }
  return NO;
}

void testNSArray_Filtering() {
  
  NSArray *array = @[ @1, @2, @3, @4, @5, @-6, @-7, @-8, @-9 ];
  
  // Use predicates (block-based)
  {
    NSArray *filteredArray = [array filteredArrayUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(id obj, NSDictionary *bindings) {
      return filterObject(obj);
    }]];
    NSLog(@"Filtered array: %@", filteredArray);  // 1, 2, 3, 4, 5
  }
  
  // Use indexesOfObjectsWithOptions:passingTest:
  {
    NSIndexSet *indexes = [array indexesOfObjectsWithOptions:NSEnumerationConcurrent
                                                 passingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
                                                   return filterObject(obj);
                                                 }];
    NSArray *filteredArray = [array objectsAtIndexes:indexes];
    NSLog(@"Filtered array: %@", filteredArray);  // 1, 2, 3, 4, 5
  }
  
  // Block-based enumeration
  {
    NSMutableArray *filteredArray = [NSMutableArray array];
    [array enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
      if (filterObject(obj)) {
        [filteredArray addObject:obj];
      }
    }];
  }
  
}  // testNSArray_Filtering


// NSArray has come with built-in binary search since iOS 4 / Snow Leopard:
//
// typedef NS_OPTIONS(NSUInteger, NSBinarySearchingOptions) {
//   NSBinarySearchingFirstEqual     = (1UL << 8),
//   NSBinarySearchingLastEqual      = (1UL << 9),
//   NSBinarySearchingInsertionIndex = (1UL << 10),
// };
//
// - (NSUInteger)indexOfObject:(id)obj
//               inSortedRange:(NSRange)r
//                     options:(NSBinarySearchingOptions)opts
//             usingComparator:(NSComparator)cmp;
//
void testNSArray_BinarySearch() {
  
  // First Equal
  {
    NSArray *sortedArray = @[ @1, @2, @3, @3, @4, @5 ];
    NSRange searchRange = NSMakeRange(0, [sortedArray count]);
    
    NSUInteger index = [sortedArray indexOfObject:@3
                                    inSortedRange:searchRange
                                          options:NSBinarySearchingFirstEqual
                                  usingComparator:^NSComparisonResult(id obj1, id obj2) {
                                    return [obj1 compare:obj2];
                                  }];
    assert(index == 2);
  }
  
  // Last Equal
  {
    NSArray *sortedArray = @[ @1, @2, @3, @3, @4, @5 ];
    NSRange searchRange = NSMakeRange(0, [sortedArray count]);
    
    NSUInteger index = [sortedArray indexOfObject:@3
                                    inSortedRange:searchRange
                                          options:NSBinarySearchingLastEqual
                                  usingComparator:^NSComparisonResult(id obj1, id obj2) {
                                    return [obj1 compare:obj2];
                                  }];
    assert(index == 3);
  }
  
  // Insertion Index
  {
    NSMutableArray *sortedArray = [NSMutableArray arrayWithArray:@[ @1, @2, @3, @3, @4, @5 ]];
    NSRange searchRange = NSMakeRange(0, [sortedArray count]);
    
    NSUInteger index = [sortedArray indexOfObject:@0
                                    inSortedRange:searchRange
                                          options:NSBinarySearchingFirstEqual|NSBinarySearchingInsertionIndex
                                  usingComparator:^NSComparisonResult(id obj1, id obj2) {
                                    return [obj1 compare:obj2];
                                  }];
    assert(index == 0);
    
    [sortedArray insertObject:@0 atIndex:index];
    //NSLog(@"Sorted array after insertion: %@", sortedArray);
  }
}
