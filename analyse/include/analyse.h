//
// Created by vallois on 19/06/18.
//

#ifndef ANALYSE_ANALYSE_H
#define ANALYSE_ANALYSE_H

#include <map>
#include <vector>
#include "TFile.h"
#include "TTree.h"

struct mid_point //Structure solution
{
    double angle;   //angle en radians
    double* position;   //les x, y, z du point trouvé
};


std::pair<int, int> addPair(std::pair<int, int> p1, std::pair<int, int> p2);
void generateMulti(std::vector<std::pair<int, int>>& vectPair, int multi);
mid_point findpoint (double** dataAB);
void lightApad(std::vector<int> & x_coord_hit_pads, std::vector<int> & y_coord_hit_pads, int nb_mult,int nb_pad);

class Analyse {
public :
    static void processGeometry();
    static std::map<int, double> _geometryMap;
    static int _size_pad;

};


#endif //ANALYSE_ANALYSE_H
