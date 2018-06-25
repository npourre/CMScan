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
    //TODO la taille des pads...
    _position[0] = _position[0]/vecCaloHit.size()*Analyse::_size_pad;
    _position[1] = _position[1]/vecCaloHit.size()*Analyse::_size_pad;
    _position[2] = _position[2] = Kpos[vecCaloHit.at(0)->getCellID()[2]]*10;
    _layerID = vecCaloHit.at(0)->getCellID()[2];
}

/**
 * \fn static void buildCluster(std::vector<CaloHit*>& caloHitUse, std::vector<CaloHit*>& vectCaloHit, CaloHit* hit, std::vector<CaloHit*>& caloHitSelected)
 * \brief Fonction recursive qui ajoute au cluster final le calohit si il n'est pas dans le vector caloHitUse et si la distance est inferieur a 1 pad
 *
 * @param caloHitUse Liste des calohits déjà selectionner lors du clustering
 * @param vectCaloHit Liste des calohits initiaux
 * @param hit Calohit testé sur cette itération
 * @param caloHitSelected calohits qui composeront le cluster final
 */
void Cluster::buildCluster(std::vector<CaloHit*>& caloHitUse, std::vector<CaloHit*>& vectCaloHit, CaloHit* hit, std::vector<CaloHit*>& caloHitSelected) {

    for (auto &aCaloHit : vectCaloHit) {

        if ( std::find(caloHitUse.begin(),caloHitUse.end(),(aCaloHit)) != caloHitUse.end() )
            continue;
        if (std::abs(hit->getCellID()[0]-aCaloHit->getCellID()[0])<=1 &&
            std::abs(hit->getCellID()[1]-aCaloHit->getCellID()[1])<=1){
            caloHitUse.push_back(aCaloHit);
            caloHitSelected.push_back(aCaloHit);
            buildCluster(caloHitUse, vectCaloHit, aCaloHit, caloHitSelected);
        }
    }
}

/**
 * \fn static void clustering(std::map<int, std::vector<CaloHit*>>& hits, std::map<int, std::vector<Cluster*>> &outCluster)
 * \brief Fonction qui regroupe les caloHits voisins en cluster
 *
 * \example 1001 donnent 2 clusters le premier  1000 et le deuxième 0001
 *          0100                                0100                0000
 *          1010                                1010                0000
 *          0100                                0100                0000
 *
 * @param hits Liste des hits à regrouper en cluster.
 * @param outCluster Liste des clusters incrémentés par la fonction.
 * @return NULL.
 */
//TODO faire un test avec des list et remove après chaque creation.
void Cluster::clustering(std::map<int, std::vector<CaloHit*>>& hits, std::map<int, std::vector<Cluster*>>& outCluster) {

    for (auto &vectCaloHit : hits){

        ///Liste des caloHits qui composeront le cluster
        std::vector<CaloHit*> caloHitSelected;
        ///Liste des calohits déjà selectionné lors du clustering
        std::vector<CaloHit*> caloHitUse;

        for (auto &aCaloHit : vectCaloHit.second){

            if ( std::find(caloHitUse.begin(),caloHitUse.end(),(aCaloHit)) != caloHitUse.end() )
                continue;

            caloHitUse.push_back(aCaloHit);
            caloHitSelected.push_back(aCaloHit);
            buildCluster(caloHitUse, vectCaloHit.second, aCaloHit, caloHitSelected);

            ///On determine la taille du cluster en i j
            int minIValue = 100001;
            int maxIValue = -10000;
            int minJValue = 100001;
            int maxJValue = -10000;

            for(auto it : caloHitSelected){
                if(it->getCellID()[0] < minIValue)
                    minIValue = it->getCellID()[0];
                if(it->getCellID()[1] < minJValue)
                    minJValue = it->getCellID()[1];
                if(it->getCellID()[0] > maxIValue)
                    maxIValue = it->getCellID()[0];
                if(it->getCellID()[1] > maxJValue)
                    maxJValue = it->getCellID()[1];
            }

            ///On rejete les clusters dont la taille dépasse 3 ou componsé de trop de calohit
            if(caloHitSelected.size() <= 4 &&
               abs(maxIValue-minIValue+1) <= 3 &&
               abs(maxJValue-minJValue+1) <= 3){
                auto aCluster = new Cluster(caloHitSelected, abs(maxIValue - minIValue + 1), abs(maxJValue - minJValue + 1));
                outCluster[aCluster->getLayerID()].push_back(aCluster);
            }
            caloHitSelected.clear();
        }
        caloHitUse.clear();
    }
}