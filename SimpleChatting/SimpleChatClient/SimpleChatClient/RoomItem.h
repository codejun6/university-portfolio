//
//  RoomItem.h
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 14..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RoomItem : NSObject

+ (RoomItem *)initWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max;
+ (RoomItem *)initWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max andCurrUsers:(int)curr;

- (NSInteger)roomNo;
- (void)setRoomNo:(NSInteger)roomNoValue;
- (NSString *)title;
- (void)setTitle:(NSString *)titleValue;
- (NSString *)users;
- (void)setUsers:(NSString *)usersValue;
- (int)maxUsers;
- (void)setMaxUsers:(int)maxUsersValue;
- (int)currUsers;
- (void)setCurrUsers:(int)currUsersValue;

@end
