//
//  ViewController.h
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 5. 11..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController <NSTableViewDataSource, NSTableViewDelegate>

@property (weak) IBOutlet NSTextField *titleLabel;
@property (unsafe_unretained) IBOutlet NSTextView *chatArea;
@property (weak) IBOutlet NSTextField *idField;
@property (weak) IBOutlet NSTextField *inputField;
@property (weak) IBOutlet NSButton *loginBtn;
@property (weak) IBOutlet NSButton *sendBtn;
@property (weak) IBOutlet NSButton *makeBtn;
@property (weak) IBOutlet NSButton *enterBtn;
@property (weak) IBOutlet NSButton *exitBtn;
@property (weak) IBOutlet NSButton *settingBtn;
- (IBAction)clickLogin:(id)sender;
- (IBAction)clickSend:(id)sender;
- (IBAction)clickMakeRoom:(id)sender;
- (IBAction)clickEnterRoom:(id)sender;
- (IBAction)clickExitRoom:(id)sender;
- (IBAction)clickSetting:(id)sender;
- (IBAction)clickClose:(id)sender;

@end

