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

int analyse::_size_pad=10;

int main(int argc, char** argv) {
    analyse::processGeometry();
    TApplication theApp("Analysis", &argc, argv);
    auto *tCanvas=new TCanvas("tCanvas","tCanvas",1000,800);
    std::string fileName = "../../CMTScan/cmake-build-debug/data.root";
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

    std::mt19937 ran(static_cast<unsigned long>(time(nullptr)));
    int size_pad=analyse::_size_pad;
    std::vector<CaloHit> vectCaloHit;
    Int_t actual_event = 1;
    Long64_t nbEntries = tree->GetEntries();
    std::vector <mid_point> resultat;

    for (Long64_t i = 0; i < nbEntries; ++i) {
        tree->GetEntry(i);
        std::cout<<"event " << i <<std::endl;
        //Efficiency
        if ((ran()/4294967295.)>0.95)
            continue;

        if(_eventNb == actual_event){
            //Add RawHit
            int x = (static_cast<int>(_X)+500)/size_pad;
            int y = (static_cast<int>(_Y)+500)/size_pad;
            int z = _chamberNb;
            auto time = static_cast<int>(_Time/(200e-9));

            std::vector <int> x_coord_hit_pads;
            x_coord_hit_pads.push_back(x);
            std::vector <int> y_coord_hit_pads;
            y_coord_hit_pads.push_back(y);

            double mult_test=ran()/4294967295.;
            int nb_mult=1;
            if (mult_test>0.476 && mult_test<=0.809) //7 chances over 21
            {
                //Multiplicity 2
                nb_mult=2;
                lightApad(x_coord_hit_pads,y_coord_hit_pads,nb_mult,size_pad);
            }
            if (mult_test>0.809) //4 chance over 21
            {
                //Multiplicity 3
                nb_mult=3;
                lightApad(x_coord_hit_pads,y_coord_hit_pads,nb_mult,size_pad);
            }

            for (int j = 0; j < nb_mult; ++j) {
                int tempPos[3] = {x_coord_hit_pads[j], y_coord_hit_pads[j], z};
                CaloHit aCaloHit(tempPos, 1, time);
                vectCaloHit.push_back(aCaloHit);
            }
        }
        else{
            actual_event=_eventNb;
            std::map<int, std::vector<Cluster>> mapOfClusters;
            Cluster::clustering(vectCaloHit, mapOfClusters);
            std::map<int, std::vector<Cluster>> upperCalo;
            std::map<int, std::vector<Cluster>> lowerCalo;

            for(auto& clusterMap : mapOfClusters) {
                for (auto &clusterVect : clusterMap.second) {

                    if (clusterVect.getLayerID() < 4)
                        upperCalo.insert({clusterMap.first, clusterMap.second});

                    else
                        lowerCalo.insert({clusterMap.first, clusterMap.second});
                }
            }
            if (upperCalo.size() >= 3 && lowerCalo.size() >=3){
                Trace upperTrace = Trace::tracering(upperCalo);
                double khi2=upperTrace.linearRegression();
                if (khi2==-1)
                    continue;
                Trace lowerTrace = Trace::tracering(lowerCalo);
                khi2=lowerTrace.linearRegression();
                if (khi2==-1)
                    continue;
                auto ** vectortemp=new double* [2];
                vectortemp[0]=new double [6]; /// x,y,z pour le detecteur du haut
                vectortemp[1]=new double [6]; /// x,y,z pour le detecteur du bas
                vectortemp[0][0]=upperTrace.getPointLine()[0];
                vectortemp[0][1]=upperTrace.getPointLine()[1];
                vectortemp[0][2]=upperTrace.getPointLine()[2];
                vectortemp[0][3]=upperTrace.getVectDirLine()[0];
                vectortemp[0][4]=upperTrace.getVectDirLine()[1];
                vectortemp[0][5]=upperTrace.getVectDirLine()[2];
                vectortemp[1][0]=lowerTrace.getPointLine()[0];
                vectortemp[1][1]=lowerTrace.getPointLine()[1];
                vectortemp[1][2]=lowerTrace.getPointLine()[2];
                vectortemp[1][3]=lowerTrace.getVectDirLine()[0];
                vectortemp[1][4]=lowerTrace.getVectDirLine()[1];
                vectortemp[1][5]=lowerTrace.getVectDirLine()[2];
                resultat.push_back(findpoint(vectortemp));
                delete []vectortemp[0];
                delete []vectortemp[1];
                delete []vectortemp;
            }
            vectCaloHit.clear();

        }
        if(i>600)
            break;
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