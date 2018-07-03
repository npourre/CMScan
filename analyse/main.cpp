#include <iostream>
#include "TPolyLine3D.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TH2D.h"
#include "TApplication.h"
#include "TCanvas.h"
#include <thread>
#include "TGraph2D.h"
#include <random>
#include "caloHit.h"
#include "Cluster.h"
#include "analyse.h"
#include "rootreader.h"

//TODO doxygen...
//TODO cree une classe write c'est pas du luxe...
int Analyse::_size_pad=10;

int main(int argc, char* argv[]) {

    TApplication theApp("Analysis", &argc, argv);

    RootReader reader;
    if (argc == 1)
        reader.Initialise();
    else{
        if (argc == 2)
            reader.Initialise(argv[1]);
    }

    // Import geometry.json
    Analyse::processGeometry();

    // random init
    long seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937 rand_engine(seed);
    std::uniform_real_distribution<double> real_distribution(0,1);
    std::uniform_int_distribution<int> multipli_distribution(1,4);

    // Accumulateur pour les absorptions
    auto *accumulator = new TH3D("absorber accumulator","absorber accumulator",100,0,1000,100,0,1000,240,-1200,1200);

    int size_pad = Analyse::_size_pad;
    std::map<int, std::vector<CaloHit*>> mapCaloHit;
    Int_t actual_event = 1;

    Long64_t nb_entries = reader.GetTreeSize();

    std::vector <mid_point> resultat;

    // boucle sur les valeurs de l'arbre
    for (Long64_t i = 0; i < nb_entries; ++i) {
        RawHit a_raw_hit = reader.GetEntry(i);

        ///Efficiency
        if (real_distribution(rand_engine)>0.95)
            continue;

        if(a_raw_hit.eventNb != actual_event){
            ///Calcul de la trace qu'il est tout nul et tout lent
            if(a_raw_hit.eventNb%10000==0)
                std::cout<<"event : "<<a_raw_hit.eventNb<<std::endl;
            actual_event=a_raw_hit.eventNb;
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

//                    display(mapCaloHit,mapOfClusters,lowerTrace,upperTrace);

                }
                for (auto &it : lowerTrace){
                    delete it;
                }
                for (auto &it : upperTrace){
                    delete it;
                }

            }
            if(upperCalo.size() >= 3 && lowerCalo.empty()){
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

                if (nextEpisode){
                    addAbsorb(upperTrace, accumulator);
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
        ///Multiplicity
        std::vector<std::pair<int, int>> vectPair;
        std::pair<int, int> coord({(static_cast<int>(a_raw_hit.X)+500)/size_pad, (static_cast<int>(a_raw_hit.Y)+500)/size_pad});
        vectPair.push_back(coord);

        double multiTest = real_distribution(rand_engine);
        int multiplicity=1;
        if(multiTest>0.809){
            multiplicity=3;
        }
        else{
            if(multiTest>0.476){
                multiplicity=2;
            }
        }
//        generateMulti(vectPair, multiplicity);

        int k = a_raw_hit.chamberNb;
        auto time = static_cast<int>(a_raw_hit.time/(200e-9));

        ///Add calohit
        for (auto &j : vectPair) {
            int tempPos[3] = {j.first, j.second, k};
            auto * aCaloHit = new CaloHit(tempPos, 1, time);
            mapCaloHit[k].push_back(aCaloHit);
        }
    }
/*
    auto *outputFile = new TFile("/data/data_0006.root","RECREATE");
    if(!(outputFile->IsOpen()) || outputFile->IsZombie())
        std::cout<<"Error : opening file"<<std::endl;
    else
        std::cout<<"outputFile file is open"<<std::endl;

    double x;
    double y;
    double z;
    double angle;

    auto outputTree = new TTree("Hits","Hits");
    outputTree->Branch("x",&x,"x/D");
    outputTree->Branch("y",&y,"y/D");
    outputTree->Branch("z",&z,"z/D");
    outputTree->Branch("angle",&angle, "angle/D");
    for (auto &midpoint : resultat){
        x=midpoint.position[0];
        y=midpoint.position[1];
        z=midpoint.position[2];
        angle=midpoint.angle;
        outputTree->Fill();
    }
    outputTree->Write();
*/
    auto *tCanvas=new TCanvas("tCanvas","tCanvas",1000,800);
    tCanvas->Divide(2,2);
    tCanvas->cd(1);
    accumulator->Draw("BOX2Z");
    tCanvas->cd(2);
    accumulator->Project3D("yx")->Draw("colz");
    tCanvas->cd(3);
    accumulator->Project3D("zy")->Draw("colz");
    tCanvas->cd(4);
    accumulator->Project3D("zx")->Draw("colz");
    theApp.Run(kTRUE);
/*
    delete outputTree;
    delete outputFile;
*/
    delete tCanvas;

    return 0;
}