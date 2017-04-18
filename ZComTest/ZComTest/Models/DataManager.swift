//
//  DataManager.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 24..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import Alamofire
import SwiftyJSON
import RealmSwift

class DataManager {
    
    static let sharedManager = DataManager()
    
    private let realm: Realm
    
    private init() {
        self.realm = try! Realm()
        
        self.refreshLocalDBFromServer()
    }
    
    func refreshLocalDBFromServer() {
        guard let serverUrl = Bundle.main.infoDictionary?["Server JSON URL"] as? String else { return }
        
        Alamofire.request(serverUrl).responseJSON {
            guard let responseData = $0.result.value else { return }
            
            for (_, data) in JSON(responseData)["filtered"] {
                guard let id = data["id"].int,
                    self.realm.object(ofType: Apartment.self, forPrimaryKey: id) == nil
                    else { continue }

                let apartment = Apartment()
                apartment.id = data["id"].intValue
                apartment.name = data["name"].stringValue
                apartment.bunji = data["bunji"].stringValue
                apartment.latitude = data["lat"].doubleValue
                apartment.longitude = data["lng"].doubleValue
                apartment.households = data["households"].intValue
                apartment.buildDate = data["buildDate"].stringValue
                apartment.score = data["score"].doubleValue
                apartment.price = data["price"].intValue
                apartment.floorArea = data["floorArea"].doubleValue
                
                // 로컬 DB에 해당 지역 정보 데이터가 없는 경우에만 새 객체를 생성합니다.
                let areaName = "\(data["sido"].stringValue) \(data["gugun"].stringValue) \(data["dong"].stringValue)"
                if let area = self.realm.object(ofType: Area.self, forPrimaryKey: areaName) {
                    apartment.area = area
                } else {
                    let newArea = Area()
                    newArea.areaName = areaName
                    apartment.area = newArea
                }
                
                if !data["brand"].stringValue.isEmpty {
                    // 로컬 DB에 해당 브랜드 데이터가 없는 경우에만 새 객체를 생성합니다.
                    let name = data["brand"].stringValue
                    if let brand = self.realm.object(ofType: Brand.self, forPrimaryKey: name) {
                        apartment.brand = brand
                    } else {
                        let newBrand = Brand()
                        newBrand.name = name
                        apartment.brand = newBrand
                    }
                }
                
                // 마커의 옵션에 따라 마커 데이터를 설정합니다.
                let markerOptions = [["small", "large"], ["base", "selected"]]
                for kind in markerOptions[0] {
                    for state in markerOptions[1] {
                        var type = "apartment"
                        if kind != "large" {
                            type = "\(kind)_apartment"
                        }
                        if state != "base" {
                            type += "_\(state)"
                        }
                        
                        let imageUrl = data["marker"][kind]["xxxh"][state].stringValue
                        if let marker = self.realm.object(ofType: Marker.self, forPrimaryKey: imageUrl) {
                            apartment.markers.append(marker)
                        } else {
                            let newMarker = Marker()
                            newMarker.imageUrl = imageUrl
                            newMarker.type = type
                            newMarker.size = "xxxh"
                            newMarker.brand = apartment.brand
                            apartment.markers.append(newMarker)
                        }
                    }
                }
                
                try! self.realm.write {
                    self.realm.add(apartment, update: true)
                    
                    if let area = self.realm.object(ofType: Area.self, forPrimaryKey: areaName) {
                        area.apartments.append(apartment)
                    }
                    
                    if let brandName = apartment.brand?.name {
                        if let brand = self.realm.object(ofType: Brand.self, forPrimaryKey: brandName) {
                            brand.apartments.append(apartment)
                        }
                    }
                }
            }
        }
    }
    
    // MARK: - 데이터 관리 함수
    
    func getApartment(_ id: Int) -> Apartment? {
        return realm.objects(Apartment.self).first(where: { $0.id == id })
    }
    
    func getApartments() -> [Apartment] {
        return Array(realm.objects(Apartment.self))
    }
    
    func getSortedApartments(forData data: String, ascending isAscending: Bool) -> [Apartment] {
        let apartments = realm.objects(Apartment.self)
        var comparison: (Apartment, Apartment) -> Bool
        switch data {
        case "name" :
            comparison = isAscending ? { $0.0.name < $0.1.name } : { $0.0.name > $0.1.name }
        case "buildDate" :
            comparison = isAscending ? { $0.0.buildDate < $0.1.buildDate } : { $0.0.buildDate > $0.1.buildDate }
        case "score" :
            comparison = isAscending ? { $0.0.score < $0.1.score } : { $0.0.score > $0.1.score }
        case "price" :
            comparison = isAscending ? { $0.0.price < $0.1.price } : { $0.0.price > $0.1.price }
        case "floorArea" :
            comparison = isAscending ? { $0.0.floorArea < $0.1.floorArea } : { $0.0.floorArea > $0.1.floorArea }
        default :
            return Array(apartments)
        }
        return apartments.sorted(by: comparison)
    }
    
    func getMarkerImageUrl(withType type: String, of apartment: Apartment) -> String? {
        return apartment.markers.first(where: { $0.type == type })?.imageUrl
    }
}
