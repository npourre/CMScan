//
// Created by vallois on 19/06/18.
//

#ifndef ANALYSE_ANALYSE_H
#define ANALYSE_ANALYSE_H

#include <map>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "Cluster.h"
#include "trace.h"
#include "TH3D.h"

struct mid_point //Structure solution
{
    double angle;   //angle en radians
    double* position;   //les x, y, z du point trouvé
};

std::pair<int, int> addPair(std::pair<int, int> p1, std::pair<int, int> p2);
void generateMulti(std::vector<std::pair<int, int>>& vectPair, int multi);
void display(std::map<int, std::vector<CaloHit*>>& mapCaloHit ,
             std::map<int, std::vector<Cluster*>>& mapOfClusters ,
             std::vector<Trace*>& lowerTrace,
             std::vector<Trace*>& upperTrace);

mid_point findpoint (Trace* upperTrace, Trace* lowerTrace);
void addAbsorb(std::vector<Trace*> &upperTrace, TH3D *accumulator);


class Analyse {
public :
    static void processGeometry();
    static std::map<int, double> _geometryMap;
    static int _size_pad;

};


#endif //ANALYSE_ANALYSE_H
