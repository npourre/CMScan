//
// Created by vallois on 18/06/18.
//

#include "caloHit.h"

CaloHit::CaloHit(int cellID[], int threshold, int time) :
        _threshold(threshold),
        _time(time),
        _cellID{}
{
    for (int i = 0; i < 3; ++i) {
        _cellID[i]=cellID[i];
    }
}

bool CaloHit::operator==(CaloHit const &right) {
    return this->getCellID()[0] == right.getCellID()[0] &&
           this->getCellID()[1]==right.getCellID()[1] &&
           this->getCellID()[2]==right.getCellID()[2] &&
           this->getTime()==right.getTime() &&
           this->getThreshold()==right.getThreshold();
}
