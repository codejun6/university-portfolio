//
//  AppDelegate.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 22..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import UIKit
import GoogleMaps

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

    var window: UIWindow?

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplicationLaunchOptionsKey: Any]?) -> Bool {
        if let googleMapsApiKey = Bundle.main.infoDictionary?["GoogleMaps API Key"] as? String {
            GMSServices.provideAPIKey(googleMapsApiKey)
        }
        return true
    }

}

