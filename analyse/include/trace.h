//
// Created by vallois on 19/06/18.
//

#ifndef ANALYSE_TRACE_H
#define ANALYSE_TRACE_H


#include "Cluster.h"
#include <vector>
#include <map>

class Trace {
public:
    explicit Trace(std::vector<Cluster*> &traceHitList);
    double linearRegression();

    static void tracering(std::map<int, std::vector<Cluster*>> &mapOfClusters, std::vector<Trace*> &traceVec);
    static std::pair<double,double> processHough(std::vector<double>& x, std::vector<double>& y);
    static double distance(std::pair<double,double> linePlanXZ, std::pair<double,double> linePlanYZ, double x, double y, double z);

    inline const double* getPointLine() const { return _pointLine;}
    inline const double* getVectDirLine() const { return _vectDirLine;}

private:
    std::vector<Cluster*> _clusterVec;
    double _pointLine[3];
    double _vectDirLine[3];
};



#endif //ANALYSE_TRACE_H
