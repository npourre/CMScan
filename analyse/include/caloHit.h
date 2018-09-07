//
// Created by vallois on 18/06/18.
//

#ifndef ANALYSE_CALOHIT_H
#define ANALYSE_CALOHIT_H

class CaloHit {
public:
    CaloHit(int cellID[], int threshold, int time);
    ~CaloHit() = default;

    inline const int* getCellID() const { return _cellID;}
    inline int getThreshold() const { return _threshold;}
    inline int getTime() const { return _time;}

    bool operator==(CaloHit const& right);

private:
    int _cellID[3];
    int _threshold;
    int _time;
};
#endif //ANALYSE_CALOHIT_H
