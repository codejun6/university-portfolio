//
//  ListViewController.swift
//  ZigbangTestMinsuJun
//
//  Created by MS MacBook on 2017. 3. 22..
//  Copyright © 2017년 codejun6. All rights reserved.
//

import UIKit
import Kingfisher

class ListViewController: UITableViewController {
    
    @IBOutlet weak var orderButton: UIBarButtonItem!
    
    var apartments: [Apartment] = DataManager.sharedManager.getSortedApartments(forData: "name", ascending: true)

    override func viewDidLoad() {
        super.viewDidLoad()
        
        if let title = self.navigationItem.title {
            self.title = "\(title)(\(self.apartments.count)개)"
        }
        
        self.orderButton.setTitleTextAttributes([NSFontAttributeName: UIFont.systemFont(ofSize: 14)], for: UIControlState.normal)
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return self.apartments.count
    }

    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "listViewCell", for: indexPath) as! ListViewCell

        let apartment = self.apartments[indexPath.row]
        cell.priceAndFloorAreaLabel.text = "\(apartment.formattedPrice) / \(apartment.formattedFloorArea)"
        cell.nameLabel.text = apartment.name
        cell.areaLabel.text = apartment.area?.areaName
        
        cell.apartmentImageIndicator.startAnimating()
        let processor = ResizingImageProcessor(targetSize: cell.apartmentImageView.frame.size, contentMode: .none)
        cell.apartmentImageView.kf.setImage(with: URL(string: apartment.imageUrl), placeholder: nil, options: [.processor(processor)], progressBlock: nil, completionHandler: { (_, _, _, _) in
            
            cell.apartmentImageIndicator.stopAnimating()
            cell.setNeedsLayout()
        })
        
        return cell
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        guard let destination = self.storyboard?.instantiateViewController(withIdentifier: "DetailViewController") as? DetailViewController else { return }
        
        destination.detailApartment = self.apartments[indexPath.row]
        self.navigationController?.pushViewController(destination, animated: true)
    }
    
    @IBAction func tapOrderButton(_ sender: UIBarButtonItem) {
        let orderTexts = ["이름순↑", "이름순↓", "가격순↑", "가격순↓", "평점순↑", "평점순↓", "크기순↑", "크기순↓", "건설날짜순↑", "건설날짜순↓"]
        let orderParams = ["name", "price", "score", "floorArea", "buildDate"]

        guard let orderIndex = orderTexts.index(where: { $0 == sender.title }) else { return }
        
        let nextIndex = (orderIndex + 1) % orderTexts.count
        self.apartments = DataManager.sharedManager.getSortedApartments(forData: orderParams[nextIndex/2], ascending: nextIndex % 2 == 0)
        
        sender.title = orderTexts[nextIndex]
        self.tableView.reloadData()
    }
}
