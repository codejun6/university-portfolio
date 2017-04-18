//
//  ListViewCell.swift
//  ZigbangTestMinsuJun
//
//  Created by Minsu Jun on 2017. 4. 17..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import UIKit

class ListViewCell: UITableViewCell {
    
    @IBOutlet weak var apartmentImageView: UIImageView!
    @IBOutlet weak var apartmentImageIndicator: UIActivityIndicatorView!
    
    @IBOutlet weak var priceAndFloorAreaLabel: UILabel!
    @IBOutlet weak var nameLabel: UILabel!
    @IBOutlet weak var areaLabel: UILabel!
}
