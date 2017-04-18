//
//  Apartment.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 24..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import RealmSwift

class Apartment: Object {
    
    dynamic var id = 0                      // 아파트 ID
    dynamic var name = ""                   // 아파트명
    dynamic var area: Area?                 // 속한 지역(시도/구군/동)
    dynamic var bunji = ""                  // 번지수
    dynamic var latitude: Double = 0.0      // 위도
    dynamic var longitude: Double = 0.0     // 경도
    dynamic var households = 0              // 세대수
    dynamic var buildDate = ""              // 건설날짜
    dynamic var score: Double = 0.0         // 평점
    dynamic var brand: Brand?               // 속한 브랜드
    dynamic var price = 0                   // 가격
    dynamic var floorArea: Double = 0.0     // 크기(평수)
    let markers = List<Marker>()            // 보유 마커 목록
    
    // MARK: - 저장되지 않는 부분
    
    override static func primaryKey() -> String? {
        return "id"
    }
    
    // 이미지 URL은 규칙에 따라 구성되어 반환합니다.
    var imageUrl: String {
        return "https://i6.zigbang.com/v1/apartments/\(id)/profile.jpg?orientation=landscape"
    }
    
    var fullName: String {
        if let brand = self.brand {
            return "\(self.name) 아파트[\(brand.name)]"
        }
        return "\(self.name) 아파트"
    }
    
    var address: String {
        if let area = self.area {
            return "\(area.areaName) \(self.bunji)"
        }
        return self.bunji
    }
    
    var formattedHouseholds: String {
        return "\(self.households)세대"
    }
    
    var formattedBuildDate: String {
        if self.buildDate != "" {
            var date = self.buildDate
            date.insert(".", at: date.index(date.startIndex, offsetBy: 4))
            return date
        }
        return self.buildDate
    }
    
    var formattedScore: String {
        return "\(String(format: "%.1f", self.score))점"
    }
    
    var formattedPrice: String {
        if self.price % 10000 == 0 {
            return "\(self.price/10000)억원"
        }
        return "\(Double(self.price)/10000)억원"
    }
    
    var formattedFloorArea: String {
        return "\(Int(self.floorArea))평"
    }
}
