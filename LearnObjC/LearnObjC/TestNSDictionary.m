//
//  TestNSDictionary.m
//  LearnObjC
//
//  Created by Adam Gu on 4/2/16.
//  Copyright © 2016 Adam Gu. All rights reserved.
//

#import <Foundation/Foundation.h>

void testNSDictionary_Create() {

  // new
  {
    NSDictionary *dict = [NSDictionary new];
    assert(dict.count == 0);
  }
  
  // alloc ] init
  {
    NSDictionary *dict = [[NSDictionary alloc] init];
    assert(dict.count == 0);
  }
  
  // dictionary
  {
    NSDictionary *dict = [NSDictionary dictionary];
    assert(dict.count == 0);
  }

  // Literal syntax
  {
    NSDictionary *dict = @{ @"key1": @1,
                            @"key2": @2,
                            @"key3": @3 };
    assert(dict.count == 3);

    assert([dict[@"key1"] isEqualToNumber:@1]);
    assert([dict[@"key2"] isEqualToNumber:@2]);
    assert([dict[@"key3"] isEqualToNumber:@3]);
  }
  
  // dictionaryWithObject
  {
    NSDictionary *dict = [NSDictionary dictionaryWithObject:@1 forKey:@"key1"];
    assert(dict.count == 1);
    assert([dict[@"key1"] isEqualToNumber:@1]);
  }
  
  // dictionaryWithObjectsAndKeys
  {
    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:@1, @"key1", @2, @"key2", @3, @"key3", nil];
    
    assert(dict.count == 3);
    
    assert([dict[@"key1"] isEqualToNumber:@1]);
    assert([dict[@"key2"] isEqualToNumber:@2]);
    assert([dict[@"key3"] isEqualToNumber:@3]);
  }

  // dictionaryWithObjects:forKeys:count:
  {
    // NOTE: NOT NSString *keys = ...
    NSString *keys[] = { @"key1", @"key2", @"key3" };
    NSNumber *values[] = { @1, @2, @3 };
    
    NSDictionary *dict = [NSDictionary dictionaryWithObjects:(id *)values forKeys:(id *)keys count:3];
    
    assert(dict.count == 3);
    
    assert([dict[@"key1"] isEqualToNumber:@1]);
    assert([dict[@"key2"] isEqualToNumber:@2]);
    assert([dict[@"key3"] isEqualToNumber:@3]);
  }
  
  // dictionaryWithObjects:forKeys:
  {
    NSArray *keys = @[ @"key1", @"key2", @"key3" ];
    NSArray *values = @[ @1, @2, @3 ];
    
    NSDictionary *dict = [NSDictionary dictionaryWithObjects:values forKeys:keys];
    
    assert(dict.count == 3);

    assert([dict[@"key1"] isEqualToNumber:@1]);
    assert([dict[@"key2"] isEqualToNumber:@2]);
    assert([dict[@"key3"] isEqualToNumber:@3]);
  }
  
  // dictionaryWithDictionary
  {
    NSDictionary *dict = @{ @"key1": @1,
                            @"key2": @2,
                            @"key3": @3 };
    
    NSDictionary *anotherDict = [NSDictionary dictionaryWithDictionary:dict];
    
    assert(anotherDict.count == 3);
    
    assert([anotherDict[@"key1"] isEqualToNumber:@1]);
    assert([anotherDict[@"key2"] isEqualToNumber:@2]);
    assert([anotherDict[@"key3"] isEqualToNumber:@3]);
  }
  
}