//
//  ChatUser.h
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 5. 19..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ChatUser : NSObject

+ (ChatUser *)initWithUserId:(NSString *)userIdValue;
+ (ChatUser *)initWithUserId:(NSString *)userIdValue andLogin:(BOOL)loginValue;

- (NSString *)userId;
- (void)setUserId:(NSString *)userIdValue;
- (BOOL)login;
- (void)setLogin:(BOOL)loginValue;

@end
