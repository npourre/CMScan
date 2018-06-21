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

void lightApad(std::vector<int> & x_coord_hit_pads, std::vector<int> & y_coord_hit_pads, int nb_mult,int nb_pad)
{
    std::mt19937 ran(static_cast<unsigned long>(time(nullptr)));
    int marker;
    double random_xy=ran()/4294967295.; //modif on x or y
    double random_dir=ran()/4294967295.;

    //****** The first pad*********
    if (random_xy<0.5) //we light on x
    {
        if (random_dir<0.5) //+x
        {
            if (x_coord_hit_pads.back()+1<nb_pad)
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                marker=1; //marker corresponding to +x
            }
            else
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                marker=2;//marker corresponding to -x
            }
        }
        else //-x
        {
            if (x_coord_hit_pads.back()-1>0)
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                marker=2;
            }
            else
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                marker=1;
            }
        }
    }
    else //we light on y
    {
        if (random_dir<0.5)//+y
        {
            if (y_coord_hit_pads.back()+1<nb_pad)
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                marker=3;
            }
            else
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                marker=4;
            }
        }
        else //-y
        {
            if(y_coord_hit_pads.back()-1>0)
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                marker=4;
            }
            else
            {
                x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                marker=3;
            }
        }
    }
    //****** Eventual second pad*********
    if (nb_mult==3) //if there are two to light on
    {
        //Different tests are made, for example if we did x+1 to place the first pad
        //it is forbidden to do x-1 to place the second (or it will be the coordinate of the initial pad)
        if(marker==1) // if we did x+1 to place the first pad
        {
            double random=ran()/4294967295.;
            if (random<=(1./3.)) //it remains 3 choices
            {
                if (x_coord_hit_pads.back()+1<nb_pad)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
                else
                { //because x-1 is forbidden we do the modif on y but we have to verify the border limit
                    if (y_coord_hit_pads.back()+1<nb_pad)
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                    }
                    else
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                    }
                }
            }
            if (random>(1./3.) && random<=(2./3.))
            {
                if (y_coord_hit_pads.back()+1<nb_pad)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                }
            }
            if (random>(2./3.) && random<=1.)
            {
                if (y_coord_hit_pads.back()-1>0)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                }
            }
        }
        if(marker==2) // if we did x-1 to place the first pad
        {
            double random=ran()/4294967295.;
            if (random<=(1./3.)) //it remains 3 choices
            {
                if (x_coord_hit_pads.back()-1>0)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
                else
                { //because x+1 is forbidden we do the modif on y but we have to verify the border limit
                    if(y_coord_hit_pads.back()-1>0)
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                    }
                    else
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                    }
                }
            }
            if (random>(1./3.) && random<=(2./3.))
            {
                if (y_coord_hit_pads.back()+1<nb_pad)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                }
            }
            if (random>(2./3.) && random<=1.)
            {
                if (y_coord_hit_pads.back()-1>0)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                }
            }
        }
        if(marker==3) // if we did y+1 to place the first pad
        {
            double random=ran()/4294967295.;
            if (random<=(1./3.)) //it remains 3 choices
            {
                if (x_coord_hit_pads.back()+1<nb_pad)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
            }
            if (random>(1./3.) && random<=(2./3.))
            {
                if (x_coord_hit_pads.back()-1>0)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
            }
            if (random>(2./3.) && random<=1.)
            {
                if (y_coord_hit_pads.back()+1<nb_pad)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()+1);
                }
                else
                {//because y-1 is forbidden we do the modif on x but we have to verify the border limit
                    if (x_coord_hit_pads.back()+1<nb_pad)
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                    }
                    else
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                    }
                }
            }
        }
        if(marker==4) // if we did y-1 to place the first pad
        {
            double random=ran()/4294967295.;
            if (random<=(1./3.)) //it remains 3 choices
            {
                if (x_coord_hit_pads.back()+1<nb_pad)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
            }
            if (random>(1./3.) && random<=(2./3.))
            {
                if (x_coord_hit_pads.back()-1>0)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
                else
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                }
            }
            if (random>(2./3.) && random<=1.)
            {
                if (y_coord_hit_pads.back()-1>0)
                {
                    x_coord_hit_pads.push_back(x_coord_hit_pads.back());
                    y_coord_hit_pads.push_back(y_coord_hit_pads.back()-1);
                }
                else
                {//because y+1 is forbidden we do the modif on x but we have to verify the border limit
                    if (x_coord_hit_pads.back()+1<nb_pad)
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back()+1);
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                    }
                    else
                    {
                        x_coord_hit_pads.push_back(x_coord_hit_pads.back()-1);
                        y_coord_hit_pads.push_back(y_coord_hit_pads.back());
                    }
                }
            }
        }
    }
}

std::map<int, double> Analyse::_geometryMap={};
//TODO changer fopen
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
    Sol=Coeff.Invert()*vB;    //Résolution du SystemeLineaire
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