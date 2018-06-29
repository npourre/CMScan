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
int Analyse::_size_pad=1;

int main(int argc, char** argv) {

    Analyse::processGeometry();
    TApplication theApp("Analysis", &argc, argv);

    auto *tCanvas=new TCanvas("tCanvas","tCanvas",1000,800);

    /*ouverture du fichier data.root creation de l'arbre et des branches*/
    std::string fileName = "data10e5Pb.root"; //Juste parce que je met le fichier data dans le meme dossier
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

    /*boucle sur les valeurs de l'arbre*/
    for (Long64_t i = 0; i < nbEntries; ++i) {
        tree->GetEntry(i);
        if (i%10000==0)
        std::cout<<"event:"<<actual_event<<std::endl;
        ///Efficiency
        if (real_distribution(rand_engine)>0.95)
            continue;

        ///Multiplicity
        if(_eventNb != actual_event){
            ///Calcul de la trace qu'il est tout nul et tout lent
            actual_event=_eventNb;
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

            if (upperCalo.size() >= 3 && lowerCalo.size() >=3){
                std::vector<Trace*> upperTrace;
                Trace::tracering(upperCalo, upperTrace);
                double khi2;
                bool nextEpisode = true;
                if (!upperTrace.empty()){
                    khi2 = upperTrace[0]->linearRegression();
                    if (khi2==-1) {
                        nextEpisode = false;
                    }
                }
                else{
                    nextEpisode = false;
                }

                std::vector<Trace*> lowerTrace;
                Trace::tracering(lowerCalo, lowerTrace);
                if(!lowerTrace.empty() && nextEpisode) {
                    khi2 = lowerTrace[0]->linearRegression();
                    if (khi2 == -1) {
                        nextEpisode = false;;
                    }
                }
                else{
                    nextEpisode = false;
                }

                if (nextEpisode){
                    resultat.push_back(findpoint(upperTrace[0],lowerTrace[0]));
                    //display(mapCaloHit,mapOfClusters,lowerTrace,upperTrace);
                }
                for (auto &it : lowerTrace){
                    delete it;
                }
                for (auto &it : upperTrace){
                    delete it;
                }

            }
            for (auto& it : mapOfClusters){
                for (auto& vect : it.second)
                    delete vect;
                it.second.clear();
            }
            for (auto &vectCaloHit : mapCaloHit) {
                for (auto &aCaloHit : vectCaloHit.second)
                    delete aCaloHit;
                vectCaloHit.second.clear();
            }
            mapCaloHit.clear();

        }
        std::vector<std::pair<int, int>> vectPair;
        std::pair<int, int> coord({(static_cast<int>(_X)+500)/size_pad, (static_cast<int>(_Y)+500)/size_pad});
        vectPair.push_back(coord);

        double multiTest = real_distribution(rand_engine);
        int multiplicity=1;
        if(multiTest>0.809)
            multiplicity=3;
        else
        if(multiTest>0.476)
            multiplicity=2;

        generateMulti(vectPair, multiplicity);

        int k = _chamberNb;
        auto time = static_cast<int>(_Time/(200e-9));

        ///Add calohit
        for (auto &j : vectPair) {
            int tempPos[3] = {j.first, j.second, k};
            auto * aCaloHit = new CaloHit(tempPos, 1, time);
            mapCaloHit[k].push_back(aCaloHit);
        }

        //if(i>10000){
        //    break;
        //}

    }
    auto* histo3d = new TH3D("h3", "h3 title", 50, 0, 1000, 50, 0, 1000, 50, -1200, 1200);
    auto* histo2dXZ = new TH2D("histo2dXZ", "histo2dXZ", 50, 0, 1000, 50, -1200, 1200);
    auto* histo2dYZ = new TH2D("histo2dYZ", "histo2dYZ", 50, 0, 1000, 50, -1200, 1200);
    std::cout<<"On a quelque chose?="<<resultat.size()<<std::endl;
    for (auto& it : resultat) {
        if(it.position[0] > 0 &&
           it.position[0] < 1000 &&
           it.position[1] > 0 &&
           it.position[1] < 1000 &&
           it.position[2] > -1200 &&
           it.position[2] < 1200 &&
           fabs(it.angle)>10*M_PI/180){
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
