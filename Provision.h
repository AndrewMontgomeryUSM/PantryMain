#include <string>
#include <iostream>

#ifndef PROVISION_H
#define PROVISION_H

class Provision
{
    private:
        std::string name;
        int quantOnHand;
        double cost;
    
    public:
        Provision():
        name("N/A"), quantOnHand(0), cost(0.0)
        {}
        void setName(std::string);
        void setQuant(int);
        void setCost(double);
        std::string getName()const;
        int getQuant()const;
        double getCost()const;
        double getTotalCost();
        ~Provision()
        {}
};

#endif 