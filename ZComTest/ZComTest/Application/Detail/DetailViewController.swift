//
//  DetailViewController.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 22..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import UIKit
import Kingfisher

class DetailViewController: UIViewController {
    
    @IBOutlet weak var detailImageView: UIImageView!
    @IBOutlet weak var nameLabel: UILabel!
    @IBOutlet weak var priceLabel: UILabel!
    @IBOutlet weak var scoreLabel: UILabel!
    @IBOutlet weak var addressLabel: UILabel!
    @IBOutlet weak var floorAreaLabel: UILabel!
    @IBOutlet weak var householdsLabel: UILabel!
    @IBOutlet weak var buildDateLabel: UILabel!
    
    var detailApartment: Apartment? = nil

    override func viewDidLoad() {
        super.viewDidLoad()
        
        guard let apartment = self.detailApartment else { return }

        self.title = apartment.area?.areaName
        
        let processor = ResizingImageProcessor(targetSize: self.detailImageView.frame.size, contentMode: .aspectFill)
        self.detailImageView.kf.indicatorType = .activity
        self.detailImageView.kf.setImage(with: URL(string: apartment.imageUrl), placeholder: nil, options: [.processor(processor), .transition(.fade(0.2))], progressBlock: nil, completionHandler: { (_, _, _, _) in
            
            self.view.layoutIfNeeded()
        })
        
        self.nameLabel.text = apartment.fullName
        self.priceLabel.attributedText = NSAttributedString.indentedString(apartment.formattedPrice)
        self.scoreLabel.attributedText = NSAttributedString.indentedString(apartment.formattedScore)
        self.addressLabel.attributedText = NSAttributedString.indentedString(apartment.address)
        self.floorAreaLabel.attributedText = NSAttributedString.indentedString(apartment.formattedFloorArea)
        self.householdsLabel.attributedText = NSAttributedString.indentedString(apartment.formattedHouseholds)
        self.buildDateLabel.attributedText = NSAttributedString.indentedString(apartment.formattedBuildDate)
    }
}
