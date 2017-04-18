//
//  Marker.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 24..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import RealmSwift

class Marker: Object {
    
    dynamic var imageUrl = ""       // 이미지 URL
    dynamic var type = ""           // 마커 종류
    dynamic var size = ""           // 크기
    dynamic var brand: Brand?       // 속한 브랜드
    
    // MARK: - 저장되지 않는 부분
    
    override static func primaryKey() -> String? {
        return "imageUrl"
    }
}
