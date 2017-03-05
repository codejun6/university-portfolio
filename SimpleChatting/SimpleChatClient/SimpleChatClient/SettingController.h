//
//  SettingController.h
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 3..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface SettingController : NSWindowController

@property NSString *ipAddr;
@property int port;

@property (weak) IBOutlet NSTextField *ipAddrField;
@property (weak) IBOutlet NSTextField *portField;

+ (SettingController *)instanceWithIpAddr:(NSString *)ipAddr andPort:(int)port;
- (IBAction)clickButton:(NSButton *)sender;

@end
