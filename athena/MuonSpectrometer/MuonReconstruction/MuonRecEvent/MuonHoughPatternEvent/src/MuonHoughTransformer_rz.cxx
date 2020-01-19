/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughTransformer_rz.h"

MuonHoughTransformer_rz::MuonHoughTransformer_rz(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle, double threshold_histo, int number_of_sectors):MuonHoughTransformer(nbins, nbins_angle, detectorsize, detectorsize_angle, threshold_histo, number_of_sectors), m_use_residu_grad(false)
{
  m_add_weight_radius = false;
  m_weight_constant_radius=0.;
  m_add_weight_angle=false;

//   double nbins = 2*m_detectorsize/m_stepsize;
//   int detectorsize_angle = 181; // has to be made private member
//   double nbins_angle = detectorsize_angle*m_frequency_stepsize_per_angle;
}

MuonHoughTransformer_rz::~MuonHoughTransformer_rz()
{
}

void MuonHoughTransformer_rz::fillHit(MuonHoughHit* hit, double weight)
{
  //  std::cout << "MuonHoughTransformer_rz::WEIGHT " << weight << "ip_settting: " << m_ip_setting << std::endl;
  
  const double hitz = hit->getHitz();

  if (m_ip_setting == true) // niels and peter alg, butterfly to be added, and to be looked into the method
    {
      const int sectorhit = sector(hit);
      const double perp = hit->getRadius();
      const double radius = hit->getAbs();

      double dotprod = 0;
      MuonHoughHisto2D* histo = m_histos.getHisto(sectorhit);
      for (double rz0 = histo->getXmin()+m_stepsize/2.; rz0<histo->getXmax(); rz0+=m_stepsize)
	{
	  if (std::abs(rz0) > radius) continue;

	  double heigth = sqrt(radius*radius - rz0*rz0);
	  double theta = std::atan2(perp,hitz)+std::atan2(rz0,heigth); 
	  double theta_in_grad = (theta/MuonHough::Pi)*180.;

	  dotprod = perp * std::sin(theta) + hitz * std::cos(theta);
	  // 	  std::cout << " rz0 " << rz0 << " theta " << theta_in_grad << " dotprod " << dotprod  
	  //                << " hitz: " << hitz << std::endl;

	  if( theta_in_grad > 180. ) continue; // to keep the angle physical
	  if( theta_in_grad < 0. ) continue; // idem
  
	  if (dotprod >=0)
	    {
	      fillHisto(rz0,theta_in_grad,weight,sectorhit);
	    }
	}
    }
  else // (m_ip_setting == false)
    {
      int sectorhit = 0; // for ip == false always sector ==0
      const double radius = hit->getRadius();

      for (double theta=m_stepsize_per_angle/2.; theta<m_detectorsize_angle; theta+=m_stepsize_per_angle)
	{
	  double theta_in_rad = MuonHough::Pi*theta/180.;
	  double rz0 = hitz*std::sin(theta_in_rad) - radius*std::cos(theta_in_rad);
	  //	  double rz0 = hitz*sinus(theta_in_rad) - radius*cosinus(theta_in_rad);
	  double dotprod = 0;
      
	  dotprod = radius * std::sin(theta_in_rad) + hitz * std::cos(theta_in_rad);
	  if (dotprod >=0)
	    {
	      fillHisto(rz0,theta,weight,sectorhit);
	    } // dotprod
	}
    } // m_atlas_setting
}

int MuonHoughTransformer_rz::fillHisto(double rz0, double theta_in_grad, double weight, int sector)
{
  MuonHoughHisto2D* histo = m_histos.getHisto(sector);

  double binwidthx = histo->getBinWidthX();
  double binwidthy = histo->getBinWidthY();
  
  int filled_binnumber = histo->fill(rz0,theta_in_grad,weight); 
  // butterfly:

  // nearby sectors:
  if (m_number_of_sectors>=3)
    {
      double third_weight = weight/3.;
      if (sector != 0 && sector != m_number_of_sectors - 1)
	{
	  m_histos.getHisto(sector+1)->fill(rz0,theta_in_grad,third_weight);
	  m_histos.getHisto(sector-1)->fill(rz0,theta_in_grad,third_weight);
	}
      else if (sector == 0)
	{
	  m_histos.getHisto(sector+1)->fill(rz0,theta_in_grad,third_weight);
	  m_histos.getHisto(m_number_of_sectors-1)->fill(rz0,theta_in_grad,third_weight);
	}
      else // sector == m_number_of_sectors - 1
	{
	  m_histos.getHisto(sector-1)->fill(rz0,theta_in_grad,third_weight);
	  m_histos.getHisto(0)->fill(rz0,theta_in_grad,third_weight);
	}
    }

  double half_weight = 0.5*weight;
  
  if (theta_in_grad - binwidthy < 0)
    {
      histo->fill(rz0+binwidthx,theta_in_grad+binwidthy,half_weight);
      //histo->fill(rz0-binwidthx,theta_in_grad-binwidthy + 180.,half_weight); // no cyclic angle here
      if (m_use_negative_weights)
	{
	  histo->fill(rz0-binwidthx,theta_in_grad+binwidthy,-half_weight);
	}
      
    }
  else if (theta_in_grad + binwidthy > 180.)
    {
      //histo->fill(rz0+binwidthx,theta_in_grad+binwidthy - 180.,half_weight);
      histo->fill(rz0-binwidthx,theta_in_grad-binwidthy,half_weight);
      if (m_use_negative_weights)
	{
	  histo->fill(rz0+binwidthx,theta_in_grad-binwidthy,-half_weight);
	}
    }
  else 
    {
      histo->fill(rz0+binwidthx,theta_in_grad+binwidthy,half_weight);
      histo->fill(rz0-binwidthx,theta_in_grad-binwidthy,half_weight);
      if (m_use_negative_weights)
	{
	  histo->fill(rz0-binwidthx,theta_in_grad+binwidthy,-half_weight);
	  histo->fill(rz0+binwidthx,theta_in_grad-binwidthy,-half_weight);
	}
    }
  return filled_binnumber;
}

double MuonHoughTransformer_rz::calculateAngle(double hitx, double hity, double hitz, double z0)const 
{

  // z0 is cartesian coordinate where track goes through z axis

  // analog to xyz:
  double theta = 0;
  double r = std::sqrt(hitx*hitx+hity*hity);
  
  theta = std::atan2(r,hitz-z0);

  return theta;
}

MuonHoughPattern* MuonHoughTransformer_rz::hookAssociateHitsToMaximum(const MuonHoughHitContainer* event, std::pair <double,double> coordsmaximum,double maximum_residu_mm, double maximum_residu_angle, int maxsector, bool /*which_segment*/, int printlevel)const
{
  MuonHoughPattern* houghpattern = new MuonHoughPattern(MuonHough::hough_rz);

  double theta=0.,residu_distance=0.,maximum_residu=0.;
  double eradius=0., etheta=0., sin_theta=0., cos_theta=0., sin_phi=0., cos_phi=0., phi=0., ephi=0.;
  double coordsmaximumsecondinrad = m_muonhoughmathutils.angleFromGradToRadial(coordsmaximum.second);
  double rz0 = coordsmaximum.first;

  if (printlevel>=4)
    {
      std::cout << "sector: " << maxsector << std::endl;
      std::cout << "coordsmaximumfirst: " << rz0 << std::endl;
      std::cout << "coordsmaximumsecond: " << coordsmaximum.second << " coordsmaximumsecondinrad: " << coordsmaximumsecondinrad << std::endl;
      std::cout << "MuonHoughTransformer_rz::size of event: " << event->size() << std::endl;
    }

  for (unsigned int i=0; i<event->size(); i++)
    {
      double dotprod =0;
      double hitx = event->getHitx(i);
      double hity = event->getHity(i);
      int sectorhit = sector(event->getHit(i));
      int maxsecmax = maxsector + 1;
      int maxsecmin = maxsector - 1;
      if (maxsecmin < 0) maxsecmin = m_number_of_sectors - 1;
      if (maxsecmax > m_number_of_sectors -1 ) maxsecmax = 0;
      // select which hits could be in maximum:
      if (sectorhit == maxsector || sectorhit == maxsecmin || sectorhit == maxsecmax)
	{
	  double hitz = event->getHitz(i);
	  double radius = std::sqrt(hitx*hitx + hity*hity);

	  dotprod = radius * std::sin(coordsmaximumsecondinrad) + hitz * std::cos(coordsmaximumsecondinrad);

	  if (dotprod>=0)
	    {
	      double residu_distance_mm = m_muonhoughmathutils.signedDistanceToLine(hitz,radius,rz0,coordsmaximumsecondinrad);
	      
//	      theta = calculateAngle(hitx,hity,hitz,coordsmaximum.first/std::sin(coordsmaximumsecondinrad)); // used for estimating the angle
// Use this code for rz scan and theta 
//
              double radius3 = sqrt(hitx*hitx+hity*hity+hitz*hitz);
	      double heigth=0.;
	      if (std::abs(rz0) < radius3) {heigth = sqrt(radius3*radius3 - rz0*rz0);}
	      else {heigth = sqrt(rz0*rz0 - radius3*radius3);}

	      theta = std::atan2(radius,hitz)+std::atan2(rz0,heigth);
  	      
              if (printlevel>=4) { std::cout << "theta: " << theta << " heigth: " << heigth << " radius3: " << radius3 << "  std::atan2(radius,hitz): " <<  std::atan2(radius,hitz) << " +std::atan2(rz0,heigth): " << std::atan2(rz0,heigth) << " rz0: " << rz0 << std::endl; }
//              std::cout << " check rz0  estimate " << hitz*sin(theta) - radius*cos(theta) << " rz0 " << rz0 <<std::endl;   
              if (printlevel>=4 && std::abs(rz0) > radius3) {std::cout << "rz0 > radius3" << std::endl;}
	      if (m_use_residu_grad==1) {
		double residu_distance_grad = std::abs(std::sin(theta-coordsmaximumsecondinrad));
		residu_distance=residu_distance_grad;
		maximum_residu=maximum_residu_angle;
	      }
	      else {
		residu_distance=residu_distance_mm;
		maximum_residu=maximum_residu_mm;  
	      }
	      
	      if (printlevel>=4) {
		std::cout << "MuonHoughTransformer_rz::hitx: " << hitx << " hity: " << hity << " hitz: " << hitz << std::endl;
		std::cout << "MuonHoughTransformer_rz::residu_distance: " << residu_distance << std::endl;
	      }
	      bool inmax = false ;  
              if ( std::abs( theta*180./MuonHough::Pi - coordsmaximum.second) < 1.1 ) inmax = true ;

	      if(std::abs(residu_distance)<maximum_residu) // here no circular effect
		{
		  if (printlevel>=4)
		    {
		      std::cout << "MuonHoughTransformer_rz::hit added to houghpattern!" << std::endl;
		      std::cout << " Sector number hit " << sectorhit << " max " << maxsector << " detector: " << event->getHit(i)->getWhichDetector() << std::endl;
                      if (inmax) std::cout << " MuonHoughTransformer_rz:: in maximum "  << std::endl;
                      if (!inmax) std::cout << " OUTSIDE maximum theta hit "  << theta*180./MuonHough::Pi << " max Hough theta " << coordsmaximum.second << std::endl;
		    }
		  houghpattern->addHit(event->getHit(i));
		  
		  event->getHit(i)->setAssociated(true);

		  double rz0hit = residu_distance_mm + rz0;
		  eradius += rz0hit;
		  
		  if (printlevel>=4){std::cout << "MuonHoughTransformer_rz::calculateAngle: " << theta << " calculateradius: " << rz0hit << std::endl;
		    double radiush = sqrt(event->getHitx(i)*event->getHitx(i)+event->getHity(i)*event->getHity(i));

		    std::cout << " R Z hit added to hough pattern theta hit " << atan2(radiush,event->getHitz(i) ) << " theta all " <<  coordsmaximumsecondinrad << std::endl;
		  }
		  
		  sin_theta += std::sin(theta);
		  cos_theta += std::cos(theta);
		  
		  //if (m_ip_setting == true)
		  //{
		  phi = std::atan2(hity,hitx);
		  
		  sin_phi += std::sin(phi);
		  cos_phi += std::cos(phi);
		  //}
		} else  {
                  if (inmax) std::cout << "LOST hit in maximum distance" << residu_distance << std::endl;
                }

	    }// dotprod
	} // sector constraint
    } //hitno
  
  etheta = std::atan2(sin_theta,cos_theta);
  //if (m_ip_setting == true)
  //{
  // also for cosmics (but no cut on it later):
  ephi = std::atan2(sin_phi,cos_phi);
  houghpattern->setEPhi(ephi);
  //}
  //else {houghpattern->setEPhi( -MuonHough::Pi/2.);}    

  eradius=eradius/(houghpattern->size()+0.0001);
  
  if (printlevel>=4)
    {
     std::cout << "MuonHoughTransformer_rz::Etheta : " << etheta << " Size houghpattern " << houghpattern->size() << " ephi " << ephi << std::endl;
    }
  
  houghpattern->setETheta(etheta);
  houghpattern->setERTheta(eradius);
  houghpattern->setECurvature(1.);
  
  if (houghpattern->size()==0)
    {
      if (printlevel>=4){std::cout << "MuonHoughTransformer_rz::WARNING : no hits found on pattern" << std::endl;}
    }

  else if (std::abs(eradius-rz0) > 500. || std::sin(etheta-coordsmaximumsecondinrad) > 0.05)
    {
      if (printlevel>=4){
	std::cout << "MuonHoughTransformer_rz::WARNING Eradius or Etheta calc. WRONG" << std::endl;
	std::cout << "MuonHoughTransformer_rz::eradius: " << rz0 << " etheta: " << coordsmaximumsecondinrad << std::endl;
	std::cout << "MuonHoughTransformer_rz::eradius: " << eradius << " etheta: " << etheta << std::endl;
      }
      houghpattern->setETheta(coordsmaximumsecondinrad); //coordsmaximumsecondinrad
      houghpattern->setERTheta(rz0);
    }

  return houghpattern;
}

float MuonHoughTransformer_rz::weightHoughTransform (double r0) const
{ 
  if (m_add_weight_radius==true)
    {return 1./(std::abs(r0/(m_weight_constant_radius+1.)));}
  else {return 1;} // weight function, to give more importance to patterns close to origin
}

float MuonHoughTransformer_rz::weightHoughTransform (double r0,double theta) const // theta in grad
{ 
  if (m_add_weight_angle==false)
    {
      return weightHoughTransform(r0);
    }
  else 
    {
      if (m_add_weight_radius==true)
	{
	  double theta_rad = m_muonhoughmathutils.angleFromGradToRadial(theta);

	  float r_theta_weight = std::abs(std::sin(theta_rad))/(1. + std::abs((r0/6000.))); 

	  return r_theta_weight;
	}

      else {return 1;} // weight function, to give more importance to patterns close to origin
    }
}
