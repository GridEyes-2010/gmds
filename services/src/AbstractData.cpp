/*----------------------------------------------------------------------------*/
//
// Created by ledouxf on 1/9/19.
//
/*----------------------------------------------------------------------------*/
#include "gmds/services/AbstractData.h"
/*----------------------------------------------------------------------------*/
using namespace gmds;
/*----------------------------------------------------------------------------*/
AbstractData::AbstractData(const std::string AName):m_name(AName){}
/*----------------------------------------------------------------------------*/
std::string AbstractData::name() const { return m_name;}
/*----------------------------------------------------------------------------*/