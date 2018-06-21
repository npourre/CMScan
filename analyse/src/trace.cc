//
// Created by vallois on 19/06/18.
//

#include "trace.h"
#include <iostream>
#include "trace.h"
#include "analyse.h"
#include <cmath>
#include "TPolyLine3D.h"
#include <vector>
#include "TH1D.h"
#include "TH3F.h"
#include "TCanvas.h"
#include <chrono>
#include <thread>
#include "TGraph2D.h"

#include "TH2D.h"

Trace::Trace(std::vector<Cluster*> &traceHitList) :
        _clusterVec{},
        _pointLine{},
        _vectDirLine{} {
    for (auto &it : traceHitList){
        _clusterVec.push_back(it);
    }
}

void Trace::tracering(std::map<int, std::vector<Cluster*> > &mapOfClusters, std::vector<Trace*> &traceVec) {
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    for (auto& clusterMap : mapOfClusters) {

        for (auto& clusterVec : clusterMap.second){

            x.push_back(clusterVec->getPosition()[0]);
            y.push_back(clusterVec->getPosition()[1]);
            z.push_back(clusterVec->getPosition()[2]);
        }
    }

    std::pair<double,double> linePlanXZ = processHough(z,x);
    std::pair<double,double> linePlanYZ = processHough(z,y);

    std::vector<Cluster*> traceHitList;

    for (auto& clusterMap : mapOfClusters) {

        for (auto& clusterVec : clusterMap.second){

            if(distance(linePlanXZ,
                        linePlanYZ,
                        clusterVec->getPosition()[0],
                        clusterVec->getPosition()[1],
                        clusterVec->getPosition()[2]) < 100) {
                traceHitList.push_back(clusterVec);
            }
        }
    }

    if(traceHitList.size()>=3){
        Trace* aTrace = new Trace(traceHitList);
        traceVec.push_back(aTrace);
    }
}

std::pair<double,double> Trace::processHough(std::vector<double>& x, std::vector<double>& y){

    //Plot the rho(theta) curve for a corresponding couple (xi,yi)
    auto nbCluster = static_cast<int>(x.size());
    double rhoMax=0;
    int nbBin = 50;

    double dtheta = 2*M_PI/nbBin;
    auto ** theta = new double* [nbCluster];
    auto ** rho = new double* [nbCluster];
    for (int k = 0; k < nbCluster; ++k) {
        theta[k] = new double [nbBin];
        rho[k] = new double [nbBin];
    }

    for (int i = 0; i < nbCluster; ++i) {
        for (int j = 0; j < nbBin; ++j) {
            theta[i][j] = j*dtheta-M_PI;
            rho[i][j] = x[i]*cos(theta[i][j])+y[i]*sin(theta[i][j]);
            if (rho[i][j]>rhoMax)
                rhoMax=rho[i][j];
        }
    }

    //Fill the accumulator
    auto* accumulator = new TH2D("accumulator","accumulator",nbBin,-M_PI,M_PI,nbBin,0,rhoMax);
    for (int i = 0; i < nbCluster; ++i) {
        for (int j = 0; j < nbBin; ++j) {
            if(rho[i][j]>0) {
                accumulator->Fill(theta[i][j],rho[i][j]);
            }
        }
    }

    //Search the maximum in the accumulator
    double Xbin = (M_PI-(-M_PI))/nbBin;
    double Ybin = (rhoMax-0)/nbBin;
    int maxBinX = 0;
    int maxBinY = 0;
    int valueMax = 0;
    for (int i = 1; i < nbBin+2; ++i) {
        for (int j = 1; j < nbBin+2; ++j) {
            double valueBin = accumulator->GetBinContent(i,j);
            if (valueBin>valueMax) {
                maxBinX = i;
                maxBinY = j;
                valueMax= static_cast<int>(valueBin);
            }
        }
    }
    double valueRho=(maxBinY-0.5)*Ybin;
    double valueTheta=(maxBinX-0.5)*Xbin-M_PI;

    // y = ax+b
    double a = -cos(valueTheta)/sin(valueTheta);
    double b = valueRho/sin(valueTheta);

    for (int l = 0; l < nbCluster; ++l) {
        delete []rho[l];
        delete []theta[l];
    }
    delete []rho;
    delete []theta;
    delete accumulator;
    return std::make_pair(a, b);
}

//Si l'espace est muni d'un repère orthonormé, si la droite (d ) passe par le point B
// et a pour vecteur directeur u → {\displaystyle {\vec {u}}} {\vec {u}},
// la distance entre le point A et la droite (d) est donnée par la formule d= norm(BA vect u)/norm(u)
double Trace::distance(std::pair<double,double> linePlanXZ, std::pair<double,double> linePlanYZ, double x, double y, double z){
    double vectDirLine[3] = {linePlanXZ.first,linePlanYZ.first,1};
    double pointLine[3] = {linePlanXZ.second,linePlanYZ.second,0};

    double prodVect[3] = {((y-pointLine[1])-(z*vectDirLine[1])),
                          ((z*vectDirLine[0])-(x-pointLine[0])),
                          ((x-pointLine[0])*vectDirLine[1]-(y-pointLine[1])*vectDirLine[0])};

    double normProdVect = sqrt(prodVect[0]*prodVect[0]+prodVect[1]*prodVect[1]+prodVect[2]*prodVect[2]);
    double normVectDirLine = sqrt(vectDirLine[0]*vectDirLine[0]+vectDirLine[1]*vectDirLine[1]+vectDirLine[2]*vectDirLine[2]);

    return normProdVect/normVectDirLine;
}

double Trace::linearRegression() {

    double Xm=0;
    double Ym=0;
    double Zm=0;
    double Sxx=0;
    double Sxy=0;
    double Syy=0;
    double Sxz=0;
    double Szz=0;
    double Syz=0;
    double theta=0;
    double K11=0;
    double K22=0;
    double K10=0;
    double K01=0;
    double K00=0;
    double c0=0;
    double c1=0;
    double c2=0;
    double p=0;
    double q=0;
    double R=0;
    double deltam2=0;
    double RHO=0;
    double phi=0;
    double a=0;
    double b=0;
    double u=0;
    double v=0;
    double w=0;
/*
    //Visualisation des traces et des clusters
    auto *tCanvas=new TCanvas("tCanvas","tCanvas",1000,800);
    auto *l1 = new TPolyLine3D();
    auto *graph = new TGraph2D();
    int compteur=0;
    for (auto& it : _clusterVec){
        graph->SetPoint(compteur, it->getPosition()[0], it->getPosition()[1], it->getPosition()[2]);
        compteur++;
    }
    graph->SetPoint(compteur,1000,1000,1000);
    graph->SetPoint(compteur+1,000,000,000);
    // Fin Visu
*/

    unsigned int nd=0;
    for (auto& it : _clusterVec) {
        Xm += it->getPosition()[0];
        Ym += it->getPosition()[1];
        Zm += it->getPosition()[2];

        Sxx += it->getPosition()[0] * it->getPosition()[0];
        Sxy += it->getPosition()[0] * it->getPosition()[1];
        Syy += it->getPosition()[1] * it->getPosition()[1];
        Sxz += it->getPosition()[0] * it->getPosition()[2];
        Szz += it->getPosition()[2] * it->getPosition()[2];
        Syz += it->getPosition()[1] * it->getPosition()[2];
        nd++;
    }

    Xm=Xm/nd;
    Ym=Ym/nd;
    Zm=Zm/nd;
    Sxx=-(Xm*Xm)+Sxx/nd;
    Sxy=-(Xm*Ym)+Sxy/nd;
    Syy=-(Ym*Ym)+Syy/nd;
    Sxz=-(Xm*Zm)+Sxz/nd;
    Szz=-(Zm*Zm)+Szz/nd;
    Syz=-(Ym*Zm)+Syz/nd;

    theta=0.5*atan((2*Sxy)/(Sxx-Syy));
    K11=(Syy+Szz)*cos(theta)*cos(theta)+(Sxx+Szz)*sin(theta)*sin(theta)-2*Sxy*cos(theta)*sin(theta);
    K22=(Syy+Szz)*sin(theta)*sin(theta)+(Sxx+Szz)*cos(theta)*cos(theta)+2*Sxy*cos(theta)*sin(theta);
    K10=Sxz*cos(theta)+Syz*sin(theta);
    K01=-Sxz*sin(theta)+Syz*cos(theta);
    K00=Sxx+Syy;

    c2=-K00-K11-K22;
    c1=(K00*K11)+(K00*K22)+(K11*K22)-(K01*K01)-(K10*K10);
    c0=(K01*K01*K11)+(K10*K10*K22)-(K00*K11*K22);

    p=c1-((1./3.)*c2*c2);
    q=((2./27.)*c2*c2*c2)-((1./3.)*c1*c2)+c0;
    R=(1./4.)*q*q+(1./27.)*p*p*p;

    if (R>=0){
        deltam2=(-1./3.)*c2+cbrt(-0.5*q+sqrt(R))+cbrt(-0.5*q-sqrt(R));
    }
    else {
        RHO=sqrt(-(1./27.)*p*p*p);
        phi=acos(-q/(2*RHO));
        deltam2=fmin(fmin(-(1./3)*c2+2*cbrt(RHO)*cos((1./3.)*phi),-(1./3.)*c2+2*cbrt(RHO)*cos((1./3.)*(phi+2*M_PI))),-(1./3.)*c2+2*cbrt(RHO)*cos((1./3.)*(phi+4*M_PI)));
    }

    a=((-K10)/(K11-deltam2))*cos(theta)+((K01)/(K22-deltam2))*sin(theta);
    b=((-K10)/(K11-deltam2))*sin(theta)+((-K01)/(K22-deltam2))*cos(theta);
    u=(1/(1+a*a+b*b))*((1+b*b)*Xm-a*b*Ym+a*Zm);
    v=(1/(1+a*a+b*b))*(-a*b*Xm+(1+a*a)*Ym+b*Zm);
    w=(1/(1+a*a+b*b))*(a*Xm+b*Ym+(a*a+b*b)*Zm);

    _vectDirLine[0] = u-Xm;
    _pointLine[0] = u;
    _vectDirLine[1] = v-Ym;
    _pointLine[1] = v;
    _vectDirLine[2] = w-Zm;
    _pointLine[2] = w;
/*
    //Visualisation des traces et des clusters
    Double_t x,y,z;
    double norm = std::sqrt(_vectDirLine[0]*_vectDirLine[0]+_vectDirLine[1]*_vectDirLine[1]+_vectDirLine[2]*_vectDirLine[2]);
    double z0;
    if(_clusterVec.empty())
        z0=0;
    else
        z0=_clusterVec[0]->getPosition()[2];
    double t0=(z0-_pointLine[2])/(_vectDirLine[2]/norm);
    for (int i = 0; i < 1000; ++i) {

        x = _pointLine[0] + (i*10.+t0-3000)*_vectDirLine[0]/norm;
        y = _pointLine[1] + (i*10.+t0-3000)*_vectDirLine[1]/norm;
        z = _pointLine[2] + (i*10.+t0-3000)*_vectDirLine[2]/norm;

        l1->SetPoint(i,x,y,z);
    }
    graph->SetMarkerStyle(20);
    graph->Draw("pcol");
    graph->GetXaxis()->SetRange(0,1000);
    graph->GetYaxis()->SetRange(0,1000);
    graph->GetZaxis()->SetRange(0,600);
    l1->SetLineColor(kRed);
    l1->Draw("SAME");
    tCanvas->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    delete graph;
    delete l1;
    delete tCanvas;
    // Fin Visu
*/
    double khi2 = 0;
    for (auto& it : _clusterVec) {
        double t = (it->getPosition()[2] - _pointLine[2]) / _vectDirLine[2];
        double deltaX = (t * _vectDirLine[0] + _pointLine[0]) -
                        it->getPosition()[0];
        double deltaY = (t * _vectDirLine[1] + _pointLine[1]) -
                        it->getPosition()[1];
        khi2 += deltaX * deltaX / (it->getSizeClusterX()*10.408 * it->getSizeClusterX()*10.408 / 12.) +
                deltaY * deltaY / (it->getSizeClusterY()*10.408 * it->getSizeClusterY()*10.408 / 12.);
    }
    khi2 /= _clusterVec.size();

    if (std::isnan(khi2) || khi2 >5)
        khi2 = -1;

    return khi2;
}


