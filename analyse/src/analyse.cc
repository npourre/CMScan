//
// Created by vallois on 19/06/18.
//

#include "analyse.h"
#include <vector>
#include <random>
#include <iostream>
#include <iostream>
#include "TROOT.h"
#include "TMatrixT.h"
#include "TVectorT.h"
#include <cmath>
#include "filereadstream.h"
#include "document.h"
#include "TPolyLine3D.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include <thread>
#include "TH2D.h"
#include "TDecompSVD.h"


void addAbsorb(std::vector<Trace*> &upperTrace, TH3D *accumulator){
    for (auto &trace : upperTrace) {

        double x, y, z;
        const double *pointLine = trace->getPointLine();
        const double *vectDirLine = trace->getVectDirLine();
        double norm = std::sqrt(
                vectDirLine[0] * vectDirLine[0] + vectDirLine[1] * vectDirLine[1] + vectDirLine[2] * vectDirLine[2]);
        double z0 = 1200;
        double t0 = (z0 - pointLine[2]) / (vectDirLine[2] / norm);
        int i = 0;
        do {
            x = pointLine[0] + (i * 10 + t0) * vectDirLine[0] / norm;
            y = pointLine[1] + (i * 10 + t0) * vectDirLine[1] / norm;
            z = pointLine[2] + (i * 10 + t0) * vectDirLine[2] / norm;
            accumulator->Fill(x, y, z);
            i++;
        } while (z > -1200);
    }
}

std::pair<int, int> addPair(std::pair<int, int> p1, std::pair<int, int> p2){
    std::pair<int, int> p({p1.first+p2.first, p1.second+p2.second});
    return p;
}

void generateMulti(std::vector<std::pair<int, int>>& vectPair, int multi){

    std::vector<std::pair<int, int>> randomchoice({{-1,-1}, {0, -1}, {0,  1}, {-1, 0}, { 1, 0}, {-1, 1}, {0, 1}, {1, 1}});
    for (int i = 0; i < multi-1; ++i) {
        std::shuffle( randomchoice.begin(), randomchoice.end(), std::mt19937(std::random_device()()));

        std::pair<int, int> newPair = addPair(vectPair[0], randomchoice[randomchoice.size()-1]);
        randomchoice.pop_back();

        if(newPair.first < 0 || newPair.first > 1000 || newPair.second < 0 || newPair.second > 1000)
            continue;
        vectPair.push_back(newPair);
    }
}

std::map<int, double> Analyse::_geometryMap={};
void Analyse::processGeometry() {
    _geometryMap.clear();
    FILE* fp = fopen("../geometry/geometry.json", "r");
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);
    fclose(fp);

    assert(d.HasMember("chambers"));
    assert(d["chambers"].IsArray());
    const rapidjson::Value& chambers = d["chambers"];
    for (rapidjson::SizeType i = 0; i < chambers.Size(); i++)
    {
        if(chambers[i].HasMember("slot"))
        {
            auto it = _geometryMap.insert( {chambers[i]["slot"].GetInt(), chambers[i]["position_z"].GetDouble()}) ;
        }
        else
            std::cout <<"ERROR : json file has no slot member"<<std::endl;
    }
}

void display(std::map<int, std::vector<CaloHit*>>& mapCaloHit ,
             std::map<int, std::vector<Cluster*>>& mapOfClusters ,
             std::vector<Trace*>& lowerTrace,
             std::vector<Trace*>& upperTrace){

    auto *c1 = new TCanvas("tCanvas","tCanvas",1200,800);
    auto *trace_top = new TPolyLine3D();
    auto *trace_bot = new TPolyLine3D();
    auto *graph_cluster = new TGraph2D();
    auto *graph_calohit = new TGraph2D();
    auto *graph_rootsux = new TGraph2D();

    graph_rootsux->SetPoint(0,0,0,-1804);
    graph_rootsux->SetPoint(1,1000,1000, 1804);

    int size_pad = Analyse::_size_pad;
    int i = 0;
    for (auto &vectCalohit : mapCaloHit){
        for (auto &calohit : vectCalohit.second){
            graph_calohit->SetPoint(i, calohit->getCellID()[0]*size_pad, calohit->getCellID()[1]*size_pad, Analyse::_geometryMap[calohit->getCellID()[2]]*10);
            i++;
        }
    }

    i=0;
    for (auto &vectCluster : mapOfClusters){
        for (auto &cluster : vectCluster.second ){
            graph_cluster->SetPoint(i,cluster->getPosition()[0], cluster->getPosition()[1], cluster->getPosition()[2]);
            i++;
        }
    }

    double norm = std::sqrt(lowerTrace[0]->getVectDirLine()[0]*lowerTrace[0]->getVectDirLine()[0]+
                            lowerTrace[0]->getVectDirLine()[1]*lowerTrace[0]->getVectDirLine()[1]+
                            lowerTrace[0]->getVectDirLine()[2]*lowerTrace[0]->getVectDirLine()[2]);

    Double_t x,y,z;
    for (int j = 0; j < 1000; ++j) {
        x = lowerTrace[0]->getPointLine()[0] + (j*10.-3000)*lowerTrace[0]->getVectDirLine()[0]/norm;
        y = lowerTrace[0]->getPointLine()[1] + (j*10.-3000)*lowerTrace[0]->getVectDirLine()[1]/norm;
        z = lowerTrace[0]->getPointLine()[2] + (j*10.-3000)*lowerTrace[0]->getVectDirLine()[2]/norm;
        trace_bot->SetPoint(j,x,y,z);
    }

    norm = std::sqrt(upperTrace[0]->getVectDirLine()[0]*upperTrace[0]->getVectDirLine()[0]+
                     upperTrace[0]->getVectDirLine()[1]*upperTrace[0]->getVectDirLine()[1]+
                     upperTrace[0]->getVectDirLine()[2]*upperTrace[0]->getVectDirLine()[2]);

    for (int k = 0; k < 1000; ++k) {
        x = upperTrace[0]->getPointLine()[0] + (k*10.-3000)*upperTrace[0]->getVectDirLine()[0]/norm;
        y = upperTrace[0]->getPointLine()[1] + (k*10.-3000)*upperTrace[0]->getVectDirLine()[1]/norm;
        z = upperTrace[0]->getPointLine()[2] + (k*10.-3000)*upperTrace[0]->getVectDirLine()[2]/norm;
        trace_top->SetPoint(k,x,y,z);
    }

    graph_rootsux->Draw("p0");
    graph_calohit->SetMarkerSize(2);
    graph_calohit->SetMarkerStyle(20);
    graph_calohit->SetMarkerColor(40);
    graph_calohit->Draw("p same");
    graph_cluster->SetMarkerSize(1);
    graph_cluster->SetMarkerColor(42);
    graph_cluster->SetMarkerStyle(21);
    graph_cluster->Draw("p same");
    trace_bot->SetLineColor(kRed);
    trace_bot->Draw("SAME");
    trace_top->SetLineColor(kBlue);
    trace_top->Draw("SAME");

    c1->Update();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    delete graph_rootsux;
    delete trace_bot;
    delete trace_top;
    delete graph_calohit;
    delete graph_cluster;
    delete c1;
}

mid_point findpoint (double** dataAB)
{
    double SMALL_NUM=0.00000001; //Pour éviter de diviser par zéro
    //On a deux vecteurs définis par un point et une direction
    TVectorT <double> A(3); //Point A correspond au detect haut
    A(0)=dataAB[0][0]; //[haut=0;bas=1][x=0;y=1;z=2]
    A(1)=dataAB[0][1];
    A(2)=dataAB[0][2];
    TVectorT <double> B(3); //Point B correspond au detect bas
    B(0)=dataAB[1][0];
    B(1)=dataAB[1][1];
    B(2)=dataAB[1][2];
    TVectorT <double> Ad(3); // Vecteur directeur de la trace du haut
    Ad(0)=dataAB[0][3]; //[haut=0;bas=1][u=3;v=4;w=5]
    Ad(1)=dataAB[0][4];
    Ad(2)=dataAB[0][5];
    TVectorT <double> Bd(3); // Vecteur directeur de la trace du bas
    Bd(0)=dataAB[1][3];
    Bd(1)=dataAB[1][4];
    Bd(2)=dataAB[1][5];

    TVectorT <double> w(3); //un vecteur qui relie A et B
    w(0)=B(0)-A(0);
    w(1)=B(1)-A(1);
    w(2)=B(2)-A(2);
    double a=Ad*Ad;
    double b=Ad*Bd;
    double c=Bd*Bd;
    double d=Ad*w;
    double e=Bd*w;
    double D=a*c-b*b;
    double sc, tc;
    if (D < SMALL_NUM) // the lines are almost parallel
    {
        sc = 0.0;
        tc = (b>c ? d/b : e/c);    // use the largest denominator
    }
    else
    {
        sc = (b*e - c*d) / D;
        tc = (a*e - b*d) / D;
    }
    TVectorT <double> dP(3); //Le vecteur le plus court
    for (int i=0;i<3;i++)
    {
        dP(i)=w(i)+(sc*Ad(i))-(tc*Bd(i));
    }

//Résolution d'un systeme linaire pour connaitre les coordonnées du segment
//le plus court qui lie les deux droites
    //Equation de type A(Coeff)*x=B(vB)
    TMatrixT <double> Coeff (8,8);
    Coeff (0,0)=-1.; //Première equation (Numéro equa, numéro inconnue)
    Coeff (0,3)=1.;
    Coeff (1,1)=-1;
    Coeff (1,4)=1;
    Coeff (2,2)=-1;
    Coeff (2,5)=1;
    Coeff (3,0)=1;
    Coeff (3,6)=-Ad(0);
    Coeff (4,1)=1;
    Coeff (4,6)=-Ad(1);
    Coeff (5,2)=1;
    Coeff (5,6)=-Ad(2);
    Coeff (6,3)=1;
    Coeff (6,7)=-Bd(0);
    Coeff (7,4)=1;
    Coeff (7,7)=-Bd(1);
    TVectorT <double> vB (8);
    vB(0)=dP(0);
    vB(1)=dP(1);
    vB(2)=dP(2);
    vB(3)=A(0);
    vB(4)=A(1);
    vB(5)=A(2);
    vB(6)=B(0);
    vB(7)=B(1);
    TVectorT <double> Sol (8);
    TDecompSVD Dec(Coeff);
    Dec.Invert(Coeff);
    Sol=Coeff*vB;    //Résolution du SystemeLineaire
    //On récupère les coordonnées du point du segment sur la droite venant du haut
    TVectorT <double> N (3);
    N(0)=Sol(0);
    N(1)=Sol(1);
    N(2)=Sol(2);
    //Et les coordonnées du point du segment sur la droite venant du bas
    TVectorT <double> Np (3);
    Np(0)=Sol(3);
    Np(1)=Sol(4);
    Np(2)=Sol(5);
    //On cherche les coordonnées du point du milieu
    TVectorT <double> Mid (3);
    Mid(0)=(N(0)+Np(0))/2.;
    Mid(1)=(N(1)+Np(1))/2.;
    Mid(2)=(N(2)+Np(2))/2.;
    /*cout<<"Les coordonnées du point trouve entre les droites:"<<endl
    <<"x="<<Mid(0)<<endl<<"y="<<Mid(1)<<endl<<"z="<<Mid(2)<<endl<<endl;*/

    //Calcul de l'angle entre les deux traces (celles du haut et celles du bas)
    double theta;
    theta=acos(b/(sqrt(a)*sqrt(c)));  //b, c et a on été calculés précedemment
    /*cout<<"L'angle entre les deux vecteurs vaut: "<<theta<<" radians donc "
    <<theta*360/(2.*M_PI)<<" degres."<<endl;*/

    mid_point resultat{};   //On crée notre objet resultat
    resultat.position=new double [3];
    for (int i=0;i<3;i++)
    {
        resultat.position[i]=Mid(i);
    }
    resultat.angle=theta;
    return resultat;
}