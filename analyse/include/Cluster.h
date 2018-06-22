//
// Created by vallois on 19/06/18.
//

#ifndef ANALYSE_CLUSTER_H
#define ANALYSE_CLUSTER_H
#include "caloHit.h"
#include <vector>
#include <map>


class Cluster {
public:
    Cluster(const std::vector<CaloHit*> &vecCaloHit, int sizeClusterX, int sizeClusterY);
    virtual ~Cluster() = default;

    inline const double* getPosition() const { return _position;}
    inline int getLayerID() const {return _layerID;}
    inline int getSizeClusterX() const { return _sizeClusterX;}
    inline int getSizeClusterY() const { return _sizeClusterY;}

    static void clustering(std::map<int, std::vector<CaloHit*>>& hits, std::map<int, std::vector<Cluster*>> &outCluster);
    static void buildCluster(std::vector<CaloHit*>& caloHitUse, std::vector<CaloHit*>& vectCaloHit, CaloHit* hit, std::vector<CaloHit*>& caloHitSelected);

private:
    double _position[3];
    int _sizeClusterX;
    int _sizeClusterY;
    int _layerID;
};


#endif //ANALYSE_CLUSTER_H
