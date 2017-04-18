//
//  MainMapViewController.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 22..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import UIKit
import GoogleMaps
import Kingfisher

class MainMapViewController: UIViewController {
    
    @IBOutlet weak var mapView: GMSMapView!

    override func viewDidLoad() {
        super.viewDidLoad()
        
        let location = CLLocationCoordinate2DMake(37.51165364251, 127.012378452142)
        let camera = GMSCameraPosition.camera(withTarget: location, zoom: 14)
        let mapView = GMSMapView.map(withFrame: self.view.bounds, camera: camera)
        mapView.setMinZoom(6, maxZoom: 18)
        mapView.settings.tiltGestures = false
        mapView.settings.rotateGestures = false
        mapView.delegate = self
        self.mapView.addSubview(mapView)
    }
}


// MARK: - GMSMapViewDelegate extension

extension MainMapViewController: GMSMapViewDelegate {
    
    enum MarkerImageSize: String {
        case large = "{92, 77}"
        case small = "{73, 30}"
    }
    
    func refreshAllMarkers(_ mapView: GMSMapView) {
        let selectedMarkerId = mapView.selectedMarker?.userData as? Int
        
        mapView.clear()
        
        // 줌 값에 따라 마커 이미지를 변경합니다.
        var markerRect: CGRect
        var markerKind: String
        if mapView.camera.zoom < 16 {
            markerRect = CGRect(origin: CGPoint.zero, size: CGSizeFromString(MarkerImageSize.small.rawValue))
            markerKind = "small_apartment"
        } else {
            markerRect = CGRect(origin: CGPoint.zero, size: CGSizeFromString(MarkerImageSize.large.rawValue))
            markerKind = "apartment"
        }
        
        for apartment in DataManager.sharedManager.getApartments() {
            let location = CLLocationCoordinate2DMake(apartment.latitude, apartment.longitude)
            let marker = GMSMarker(position: location)
            
            marker.userData = apartment.id
            
            var markerType = markerKind
            if apartment.id == selectedMarkerId {
                markerType = "\(markerType)_selected"
                marker.zIndex = 1
            }
            
            let markerView = UIImageView(frame: markerRect)
            markerView.contentMode = .scaleAspectFit
            if let url = DataManager.sharedManager.getMarkerImageUrl(withType: markerType, of: apartment) {
                markerView.kf.setImage(with: URL(string: url))
            }
            marker.iconView = markerView

            marker.map = mapView
        }
    }
    
    func mapView(_ mapView: GMSMapView, idleAt position: GMSCameraPosition) {
        self.refreshAllMarkers(mapView)
    }
    
    func mapView(_ mapView: GMSMapView, didTapAt coordinate: CLLocationCoordinate2D) {
        self.refreshAllMarkers(mapView)
    }
    
    func mapView(_ mapView: GMSMapView, didTap marker: GMSMarker) -> Bool {
        guard let destination = self.storyboard?.instantiateViewController(withIdentifier: "DetailViewController") as? DetailViewController, let id = marker.userData as? Int else { return false }
        
        destination.detailApartment = DataManager.sharedManager.getApartment(id)
        self.navigationController?.pushViewController(destination, animated: true)
        return false
    }
}

