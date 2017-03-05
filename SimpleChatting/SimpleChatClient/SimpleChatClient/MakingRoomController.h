//
//  MakingRoomController.h
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 12..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MakingRoomController : NSWindowController

@property NSString *title;
@property int maxNum;

@property (weak) IBOutlet NSTextField *titleField;
@property (weak) IBOutlet NSTextField *maxNumField;

+ (MakingRoomController *)instance;
- (IBAction)clickButton:(id)sender;

@end
