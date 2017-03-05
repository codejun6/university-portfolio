//
//  MakingRoomController.m
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 12..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import "MakingRoomController.h"

@implementation MakingRoomController

@synthesize title, maxNum;

// 객체를 초기화하고 반환하는 팩토리 메소드
+ (MakingRoomController *)instance
{
    MakingRoomController *makingRoomController = [[MakingRoomController alloc] initWithWindowNibName: @"MakingRoomWindow"];
    [makingRoomController.window update];
    makingRoomController.title = @"";
    makingRoomController.maxNum = 0;
    makingRoomController.titleField.stringValue = @"즐거운 채팅방";
    makingRoomController.maxNumField.stringValue = @"5";
    
    return makingRoomController;
}

// 버튼을 눌렀을 경우, 창을 닫는 메소드
- (IBAction)clickButton:(NSButton *)sender
{
    // 누른 버튼의 태그값에 따라 처리를 한다.
    // 만들기 버튼을 눌렀다면, 텍스트 필드에 설정한 값으로 방 제목과 최대 인원을 설정한다.
    if(sender.tag) {
        self.title = self.titleField.stringValue;
        self.maxNum = (int)self.maxNumField.integerValue;
    }
    
    // 누른 버튼의 태그값을 반환하며 Modal모드를 중지하고, 창을 닫는다.
    // 만들기 버튼: 1, 취소 버튼: 0
    [NSApp stopModalWithCode: sender.tag];
    [self.window close];
}

@end
