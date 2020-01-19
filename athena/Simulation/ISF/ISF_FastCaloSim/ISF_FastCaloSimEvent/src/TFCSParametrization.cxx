/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSParametrization.h"

//=============================================
//======= TFCSParametrization =========
//=============================================

TFCSParametrization::TFCSParametrization(const char* name, const char* title):TFCSParametrizationBase(name,title),m_Ekin_nominal(init_Ekin_nominal),m_Ekin_min(init_Ekin_min),m_Ekin_max(init_Ekin_max),m_eta_nominal(init_eta_nominal),m_eta_min(init_eta_min),m_eta_max(init_eta_max)
{
}

void TFCSParametrization::clear()
{
  m_pdgid.clear();
  m_Ekin_nominal=init_Ekin_nominal;
  m_Ekin_min=init_Ekin_min;
  m_Ekin_max=init_Ekin_max;
  m_eta_nominal=init_eta_nominal;
  m_eta_min=init_eta_min;
  m_eta_max=init_eta_max;
}

void TFCSParametrization::set_pdgid(int id)
{
  m_pdgid.clear();
  m_pdgid.insert(id);
}

void TFCSParametrization::set_pdgid(const std::set< int > &ids)
{
  m_pdgid=ids;
}

void TFCSParametrization::add_pdgid(int id)
{
  m_pdgid.insert(id);
}

void TFCSParametrization::clear_pdgid()
{
  m_pdgid.clear();
}

void TFCSParametrization::set_Ekin_nominal(double nominal)
{
  m_Ekin_nominal=nominal;
}

void TFCSParametrization::set_Ekin_min(double min)
{
  m_Ekin_min=min;
}

void TFCSParametrization::set_Ekin_max(double max)
{
  m_Ekin_max=max;
}


void TFCSParametrization::set_eta_nominal(double nominal)
{
  m_eta_nominal=nominal;
}

void TFCSParametrization::set_eta_min(double min)
{
  m_eta_min=min;
}

void TFCSParametrization::set_eta_max(double max)
{
  m_eta_max=max;
}

void TFCSParametrization::set_Ekin(const TFCSParametrizationBase& ref)
{
  set_Ekin_nominal(ref.Ekin_nominal());
  set_Ekin_min(ref.Ekin_min());
  set_Ekin_max(ref.Ekin_max());
}

void TFCSParametrization::set_eta(const TFCSParametrizationBase& ref)
{
  set_eta_nominal(ref.eta_nominal());
  set_eta_min(ref.eta_min());
  set_eta_max(ref.eta_max());
}

void TFCSParametrization::set_Ekin_eta(const TFCSParametrizationBase& ref)
{
  set_Ekin(ref);
  set_eta(ref);
}

void TFCSParametrization::set_pdgid_Ekin_eta(const TFCSParametrizationBase& ref)
{
  set_Ekin_eta(ref);
  set_pdgid(ref.pdgid());
}

