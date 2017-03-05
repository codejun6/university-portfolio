//
//  EnteringRoomController.h
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 12..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "RoomItem.h"

@interface EnteringRoomController : NSWindowController <NSTableViewDataSource>
{
@private
    IBOutlet NSTableView *roomListTable;
    NSMutableArray *roomList;
}

@property int roomNo;

+ (EnteringRoomController *)instance;
- (IBAction)clickButton:(NSButton *)sender;
- (void)addItemWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max;
- (void)addItemWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max andCurrUsers:(int)curr;
- (RoomItem *)getSelectedRoomItem;

@end
