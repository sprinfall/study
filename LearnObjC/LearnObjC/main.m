//
//  main.m
//  LearnObjC
//

#import <Foundation/Foundation.h>
#import "TestNSArray.h"
#import "TestNSDictionary.h"

static
void testNSArray() {
  testNSArray_Create();
  testNSArray_Query();
  testNSArray_Sort();
  testNSArray_Enumeration();
  testNSArray_Filtering();
  testNSArray_BinarySearch();
}

static
void testNSDictionary() {
  testNSDictionary_Create();
}

int main(int argc, const char * argv[]) {
  @autoreleasepool {
    NSLog(@"Hello, World!");

    testNSArray();
    testNSDictionary();
    
  }

  return 0;
}
