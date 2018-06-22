#include <iostream>
#include <iostream>
#include "TPolyLine3D.h"
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TH2D.h"
#include "TApplication.h"
#include "TCanvas.h"
#include <cmath>
#include <chrono>
#include <thread>
#include "TGraph2D.h"
#include <random>
#include "caloHit.h"
#include "Cluster.h"
#include "analyse.h"
#include "trace.h"

//TODO ajouter les absorb
//TODO prendre en compte la taille des pads
int Analyse::_size_pad=10;

int main(int argc, char** argv) {

    Analyse::processGeometry();
    TApplication theApp("Analysis", &argc, argv);

    auto *tCanvas=new TCanvas("tCanvas","tCanvas",1000,800);

    /*ouverture du fichier data.root creation de l'arbre et des branches*/
    std::string fileName = "/data/data.root";
    std::string treeName = "Tracker";

    auto* file = new TFile(fileName.c_str(),"READ");
    if(file->IsZombie()) std::cout<<"Error opening file"<<std::endl;
    else std::cout<<fileName<<" is open"<<std::endl;

    auto* tree = new TTree();
    file->GetObject(treeName.c_str(), tree);
    if(!tree)std::cout<<"Error : access TTree impossible"<<std::endl;
    else std::cout<<"Access TTree OK"<<std::endl;

    TBranch* b_eventNb;
    TBranch* b_chamberNb;
    TBranch* b_X;
    TBranch* b_Y;
    TBranch* b_Z;
    TBranch* b_Time;

    Int_t _eventNb;
    Int_t _chamberNb;
    Double_t _X;
    Double_t _Y;
    Double_t _Z;
    Double_t _Time;

    tree->SetBranchAddress("eventNb",& _eventNb, & b_eventNb);
    tree->SetBranchAddress("chamberNb",& _chamberNb, & b_chamberNb);
    tree->SetBranchAddress("X",& _X, & b_X);
    tree->SetBranchAddress("Y",& _Y, & b_Y);
    tree->SetBranchAddress("Z",& _Z, & b_Z);
    tree->SetBranchAddress("Time",& _Time, & b_Time);


    /*random init*/
    long seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937 rand_engine(seed);
    std::uniform_real_distribution<double> real_distribution(0,1);
    std::uniform_int_distribution<int> multipli_distribution(1,4);

    int size_pad = Analyse::_size_pad;
    std::map<int, std::vector<CaloHit*>> mapCaloHit;
    Int_t actual_event = 1;
    Long64_t nbEntries = tree->GetEntries();
    std::vector <mid_point> resultat;

    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    auto temps1 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    auto temps2 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    auto temps3 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
    auto temps4 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();

    /*boucle sur les valeurs de l'arbre*/
    for (Long64_t i = 0; i < nbEntries; ++i) {

        tree->GetEntry(i);

        ///Efficiency
        if (real_distribution(rand_engine)>0.95)
            continue;
        start = std::chrono::high_resolution_clock::now();
        ///Multiplicity
        if(_eventNb == actual_event){
            std::vector<std::pair<int, int>> vectPair;
            std::pair<int, int> coord({(static_cast<int>(_X)+500)/size_pad, (static_cast<int>(_Y)+500)/size_pad});
            vectPair.push_back(coord);

            int multiplicity = multipli_distribution(rand_engine);
            generateMulti(vectPair,multiplicity);

            int k = _chamberNb;
            auto time = static_cast<int>(_Time/(200e-9));


            ///Add calohit
            for (auto &j : vectPair) {
                int tempPos[3] = {j.first, j.second, k};
                auto * aCaloHit = new CaloHit(tempPos, 1, time);
                mapCaloHit[k].push_back(aCaloHit);
            }
        }
        else{
            stop = std::chrono::high_resolution_clock::now();
            temps1 += std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
            ///Calcul de la trace qu'il est tout nul et tout lent
            actual_event=_eventNb;
            start = std::chrono::high_resolution_clock::now();
            std::map<int, std::vector<Cluster*>> mapOfClusters;
            Cluster::clustering(mapCaloHit, mapOfClusters);
            std::map<int, std::vector<Cluster*>> upperCalo;
            std::map<int, std::vector<Cluster*>> lowerCalo;

            for(auto& clusterMap : mapOfClusters) {
                for (auto &clusterVect : clusterMap.second) {

                    if (clusterVect->getLayerID() < 4)
                        upperCalo.insert({clusterMap.first, clusterMap.second});
                    else
                        lowerCalo.insert({clusterMap.first, clusterMap.second});
                }
            }
            stop = std::chrono::high_resolution_clock::now();
            temps2 += std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
            start = std::chrono::high_resolution_clock::now();

            if (upperCalo.size() >= 3 && lowerCalo.size() >=3){
                std::vector<Trace*> upperTrace;
                Trace::tracering(upperCalo, upperTrace);
                double khi2;
                if (!upperTrace.empty()){
                    khi2 = upperTrace[0]->linearRegression();
                    if (khi2==-1)
                        continue;
                }
                else
                    continue;

                std::vector<Trace*> lowerTrace;
                Trace::tracering(lowerCalo, lowerTrace);
                if(!lowerTrace.empty()) {
                    khi2 = lowerTrace[0]->linearRegression();
                    if (khi2 == -1)
                        continue;
                }
                else
                    continue;

                stop = std::chrono::high_resolution_clock::now();
                temps3 += std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
                start = std::chrono::high_resolution_clock::now();

                auto ** vectortemp=new double* [2];
                vectortemp[0]=new double [6]; /// x,y,z pour le detecteur du haut
                vectortemp[1]=new double [6]; /// x,y,z pour le detecteur du bas
                vectortemp[0][0]=upperTrace[0]->getPointLine()[0];
                vectortemp[0][1]=upperTrace[0]->getPointLine()[1];
                vectortemp[0][2]=upperTrace[0]->getPointLine()[2];
                vectortemp[0][3]=upperTrace[0]->getVectDirLine()[0];
                vectortemp[0][4]=upperTrace[0]->getVectDirLine()[1];
                vectortemp[0][5]=upperTrace[0]->getVectDirLine()[2];
                vectortemp[1][0]=lowerTrace[0]->getPointLine()[0];
                vectortemp[1][1]=lowerTrace[0]->getPointLine()[1];
                vectortemp[1][2]=lowerTrace[0]->getPointLine()[2];
                vectortemp[1][3]=lowerTrace[0]->getVectDirLine()[0];
                vectortemp[1][4]=lowerTrace[0]->getVectDirLine()[1];
                vectortemp[1][5]=lowerTrace[0]->getVectDirLine()[2];
                resultat.push_back(findpoint(vectortemp));
                delete []vectortemp[0];
                delete []vectortemp[1];
                delete []vectortemp;

                stop = std::chrono::high_resolution_clock::now();
                temps4 += std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();
                delete upperTrace[0];
                delete lowerTrace[0];
                for (auto& it : mapOfClusters){
                    for (auto& vect : it.second)
                        delete vect;
                }
            }
            for (auto &vectCaloHit : mapCaloHit)
                for (auto &aCaloHit : vectCaloHit.second)
                    delete aCaloHit;
            mapCaloHit.clear();
        }
        if(i>1000){
            std::cout << "temps 1 : "<< temps1*1e-9 << std::endl;
            std::cout << "temps 2 : "<< temps2*1e-9 << std::endl;
            std::cout << "temps 3 : "<< temps3*1e-9 << std::endl;
            std::cout << "temps 4 : "<< temps4*1e-9 << std::endl;

            break;
        }

    }
    auto* histo3d = new TH3D("h3", "h3 title", 50, 0, 1000, 50, 0, 1000, 50, -500, 500);
    auto* histo2dXZ = new TH2D("histo2dXZ", "histo2dXZ", 50, 0, 1000, 50, -500, 500);
    auto* histo2dYZ = new TH2D("histo2dYZ", "histo2dYZ", 50, 0, 1000, 50, -500, 500);
    for (auto& it : resultat) {
        if(it.position[0]>0 &&
           it.position[0]<1000 &&
           it.position[1]>0 &&
           it.position[1]<1000 &&
           it.position[2]>109 &&
           it.position[2]<455 &&
           it.angle>0*M_PI/180){
            histo3d->Fill(it.position[0],it.position[1],it.position[2]);
            histo2dXZ->Fill(it.position[0],it.position[2]);
            histo2dYZ->Fill(it.position[1],it.position[2]);
        }
    }
    tCanvas->Divide(1,3);
    tCanvas->cd(1);
    histo3d->Draw("BOX2Z");
    tCanvas->cd(2);
    histo2dXZ->Draw("colz");
    tCanvas->cd(3);
    histo2dYZ->Draw("colz");

    theApp.Run(kTRUE);
    delete tCanvas;
    delete tree;
    delete file;
    return 0;
}