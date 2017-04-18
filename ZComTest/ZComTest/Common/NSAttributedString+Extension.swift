//
//  NSAttributedString+Extension.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 4. 14..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import UIKit

extension NSAttributedString {
    
    class func indentedString(_ string: String) -> NSAttributedString {
        let style = NSMutableParagraphStyle()
        style.firstLineHeadIndent = 10
        
        let indentedString = NSMutableAttributedString(string: string)
        indentedString.addAttribute(NSParagraphStyleAttributeName, value: style, range: NSRange(location: 0,length: indentedString.length))
        return indentedString
    }
}
