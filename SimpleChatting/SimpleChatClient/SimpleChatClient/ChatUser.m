//
//  ChatUser.m
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 5. 19..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import "ChatUser.h"

@implementation ChatUser
{
    NSString *userId;
    BOOL login;
}

+ (ChatUser *)initWithUserId:(NSString *)userIdValue {
    ChatUser *chatUser = [[ChatUser alloc] init];
    chatUser->userId = userIdValue;
    chatUser->login = NO;
    return chatUser;
}

+ (ChatUser *)initWithUserId:(NSString *)userIdValue andLogin:(BOOL)loginValue {
    ChatUser *chatUser = [[ChatUser alloc] init];
    chatUser->userId = userIdValue;
    chatUser->login = loginValue;
    return chatUser;
}

- (NSString *)userId {
    return self->userId;
}

- (void)setUserId:(NSString *)userIdValue {
    self->userId = userIdValue;
}

- (BOOL)login {
    return self->login;
}

- (void)setLogin:(BOOL)loginValue {
    self->login = loginValue;
}

@end
