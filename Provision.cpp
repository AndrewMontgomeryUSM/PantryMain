#include "Provision.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
using namespace std;

void Provision::setName(std::string n)
{ name = n; }

void Provision::setQuant(int q)
{ quantOnHand = q; }

void Provision::setCost(double c)
{ cost = c; }

std::string Provision::getName()const
{ return name;}

int Provision::getQuant()const
{ return quantOnHand;}

double Provision::getCost()const
{ return cost; }

double Provision::getTotalCost()
{ return quantOnHand * cost;}