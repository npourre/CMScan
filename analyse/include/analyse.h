//
// Created by vallois on 19/06/18.
//

#ifndef ANALYSE_ANALYSE_H
#define ANALYSE_ANALYSE_H

#include <map>
#include <vector>

struct mid_point //Structure solution
{
    double angle;   //angle en radians
    double* position;   //les x, y, z du point trouvé
};

mid_point findpoint (double** dataAB);
void lightApad(std::vector<int> & x_coord_hit_pads, std::vector<int> & y_coord_hit_pads, int nb_mult,int nb_pad);

class analyse {
public :
    static void processGeometry();
    static std::map<int, double> _geometryMap;
    static int _size_pad;
};


#endif //ANALYSE_ANALYSE_H
