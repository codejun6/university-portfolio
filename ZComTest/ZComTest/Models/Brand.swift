//
//  Brand.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 24..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import RealmSwift

class Brand: Object {
    
    dynamic var name = ""                   // 브랜드명
    let apartments = List<Apartment>()      // 해당 브랜드에 속한 아파트 목록
    
    // MARK: - 저장되지 않는 부분
    
    override static func primaryKey() -> String? {
        return "name"
    }
}
