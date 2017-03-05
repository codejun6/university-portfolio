//
//  SettingController.m
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 3..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import "SettingController.h"

@implementation SettingController

@synthesize ipAddr, port;

// 객체를 초기화하고 반환하는 팩토리 메소드
+ (SettingController *)instanceWithIpAddr:(NSString *)ipAddr andPort:(int)port
{
    SettingController *settingController = [[SettingController alloc] initWithWindowNibName: @"SettingWindow"];
    [settingController.window update];
    settingController.ipAddr = settingController.ipAddrField.stringValue = ipAddr;
    settingController.port = port;
    settingController.portField.stringValue = [NSString stringWithFormat: @"%d", port];
    
    return settingController;
}

// 버튼을 눌렀을 경우, 창을 닫는 메소드
- (IBAction)clickButton:(NSButton *)sender
{
    // 누른 버튼의 태그값에 따라 처리를 한다.
    // 확인 버튼을 눌렀다면, 텍스트 필드에 설정한 값으로 IP주소와 포트번호를 설정한다.
    if(sender.tag) {
        self.ipAddr = self.ipAddrField.stringValue;
        self.port = (int)self.portField.integerValue;
    }
    
    // 누른 버튼의 태그값을 반환하며 Modal모드를 중지하고, 창을 닫는다.
    // 확인 버튼: 1, 취소 버튼: 0
    [NSApp stopModalWithCode: sender.tag];
    [self.window close];
}

@end
