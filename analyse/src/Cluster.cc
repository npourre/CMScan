//
// Created by vallois on 19/06/18.
//

#include "Cluster.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "analyse.h"

Cluster::Cluster(const std::vector<CaloHit*> &vecCaloHit, int sizeClusterX, int sizeClusterY) :
        _position{},
        _sizeClusterX(sizeClusterX),
        _sizeClusterY(sizeClusterY),
        _layerID(-1)

{
    std::map<int,double> Kpos;
    for (auto& it : Analyse::_geometryMap){
        Kpos.insert({it.first,it.second});
    }

    for (auto it : vecCaloHit){
        _position[0] += it->getCellID()[0];
        _position[1] += it->getCellID()[1];
    }

    _position[0] = _position[0]/vecCaloHit.size()*10.408;
    _position[1] = _position[1]/vecCaloHit.size()*10.408;
    _position[2] = _position[2] = Kpos[vecCaloHit.at(0)->getCellID()[2]]*10;
    _layerID = vecCaloHit.at(0)->getCellID()[2];

}
//TODO Refaire le clustering
void Cluster::clustering(std::vector<CaloHit*>& hits, std::map<int, std::vector<Cluster*>>& outCluster) {
    std::vector<CaloHit*> checkTheUse;
    std::vector<CaloHit*> clusterHitList;

    for (auto& aCaloHit : hits) {

        if ( std::find(checkTheUse.begin(),checkTheUse.end(),(aCaloHit)) != checkTheUse.end() )
            continue;

        checkTheUse.push_back(aCaloHit);
        clusterHitList.push_back(aCaloHit);

        for (auto& aSecCaloHit : hits) {

            if (std::find(checkTheUse.begin(), checkTheUse.end(), (aSecCaloHit)) != checkTheUse.end())
                continue;

            if (checkDistance(checkTheUse, aSecCaloHit)){
                clusterHitList.push_back(aSecCaloHit);
                checkTheUse.push_back(aSecCaloHit);
            }
        }

        int minIValue = 100001;
        int maxIValue = -10000;
        int minJValue = 100001;
        int maxJValue = -10000;

        for(auto it : clusterHitList){
            if(it->getCellID()[0] < minIValue)
                minIValue = it->getCellID()[0];
            if(it->getCellID()[1] < minJValue)
                minJValue = it->getCellID()[1];
            if(it->getCellID()[0] > maxIValue)
                maxIValue = it->getCellID()[0];
            if(it->getCellID()[1] > maxJValue)
                maxJValue = it->getCellID()[1];
        }

        if(clusterHitList.size() <= 4 &&
           abs(maxIValue-minIValue+1) <= 3 &&
           abs(maxJValue-minJValue+1) <= 3){
            Cluster* aCluster = new Cluster(clusterHitList, abs(maxIValue-minIValue+1), abs(maxJValue-minJValue+1));
            outCluster[aCluster->getLayerID()].push_back(aCluster);
        }

        clusterHitList.clear();
    }

}

bool Cluster::checkDistance(const std::vector<CaloHit*> &checkTheUse, CaloHit* aCaloHit) {

    bool check = false;

    for (auto it: checkTheUse){

        if(fabs(it->getCellID()[0] - aCaloHit->getCellID()[0]) <=1 &&
           fabs(it->getCellID()[1] - aCaloHit->getCellID()[1]) <=1 &&
           fabs(it->getCellID()[2] - aCaloHit->getCellID()[2]) ==0)
            check=true;
    }

    return check;
}
