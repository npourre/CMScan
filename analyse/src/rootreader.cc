//
// Created by vallois on 02/07/18.
//
#include <iostream>
#include <utility>
#include "rootreader.h"


RootReader::RootReader() : file_name_("../data/rawdata_0000.root"), tree_name_("Tracker") {
}

RootReader::~RootReader() {
    delete tree_;
    delete file_;
}

void RootReader::Initialise(const std::string &file_name) {

    if (!file_name.empty()){
        file_name_ = "../data/";
        file_name_.append(file_name);
    }

    file_ = new TFile(file_name_.c_str(),"READ");
    if(file_->IsZombie()) std::cout<<"Error opening file"<<std::endl;
    else std::cout<<file_name_<<" is open"<<std::endl;

    tree_ = new TTree();
    file_->GetObject(tree_name_.c_str(), tree_);
    if(!tree_)std::cout<<"Error : access TTree impossible"<<std::endl;
    else std::cout<<"Access TTree OK"<<std::endl;

    tree_->SetBranchAddress("eventNb", &eventNb_, &b_eventNb_);
    tree_->SetBranchAddress("chamberNb",&chamberNb_, &b_chamberNb_);
    tree_->SetBranchAddress("X", &X_, &b_X_);
    tree_->SetBranchAddress("Y", &Y_, &b_Y_);
    tree_->SetBranchAddress("Z", &Z_, &b_Z_);
    tree_->SetBranchAddress("Time", &time_, &b_time_);
    tree_->SetBranchAddress("Energy", &energy_, &b_energy_);
}

RawHit RootReader::GetEntry(Long64_t entry) {
    tree_->GetEntry(entry);
    RawHit a_raw_hit = {eventNb_, chamberNb_, X_, Y_, Z_, time_, energy_};
    return a_raw_hit;
}

Long64_t RootReader::GetTreeSize() {
    return tree_->GetEntries();
}
