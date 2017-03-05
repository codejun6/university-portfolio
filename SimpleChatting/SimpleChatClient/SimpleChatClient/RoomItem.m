//
//  RoomItem.m
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 14..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import "RoomItem.h"

@implementation RoomItem
{
    NSInteger roomNo;
    NSString *title;
    NSString *users;
    int maxUsers;
    int currUsers;
}

+ (RoomItem *)initWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max
{
    RoomItem *roomItem = [[RoomItem alloc] init];
    roomItem->roomNo = no;
    roomItem->title = title;
    roomItem->users = [NSString stringWithFormat: @"1/%d", max];
    roomItem->maxUsers = max;
    roomItem->currUsers = 1;
    return roomItem;
}

+ (RoomItem *)initWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max andCurrUsers:(int)curr
{
    RoomItem *roomItem = [[RoomItem alloc] init];
    roomItem->roomNo = no;
    roomItem->title = title;
    roomItem->users = [NSString stringWithFormat: @"%d/%d", curr, max];
    roomItem->maxUsers = max;
    roomItem->currUsers = curr;
    return roomItem;
}

- (NSInteger)roomNo
{
    return roomNo;
}

- (void)setRoomNo:(NSInteger)roomNoValue
{
    roomNo = roomNoValue;
}

- (NSString *)title
{
    return title;
}

- (void)setTitle:(NSString *)titleValue
{
    title = titleValue;
}

- (NSString *)users
{
    return users;
}

- (void)setUsers:(NSString *)usersValue
{
    users = usersValue;
}

- (int)maxUsers
{
    return maxUsers;
}

- (void)setMaxUsers:(int)maxUsersValue
{
    maxUsers = maxUsersValue;
}

- (int)currUsers
{
    return currUsers;
}

- (void)setCurrUsers:(int)currUsersValue
{
    currUsers = currUsersValue;
}

@end
