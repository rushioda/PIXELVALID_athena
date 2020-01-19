/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// Runge-Kutta method for tracking a particle through a magnetic field
// (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iomanip>
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkExRungeKuttaIntersector/RungeKuttaIntersector.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"

namespace Trk
{

RungeKuttaIntersector::RungeKuttaIntersector (const std::string&	type,
					      const std::string&	name, 
					      const IInterface*		parent)
    :	AthAlgTool		(type, name, parent),
	m_magFieldSvc		("MagField::AtlasFieldSvc/AtlasFieldSvc", name),
	m_productionMode	(true),
	m_caloR0		(1900.*Gaudi::Units::mm),	// r min for calo high field gradient region
	m_caloR1		(2500.*Gaudi::Units::mm),	// r max for calo high field gradient region
	m_caloR2		(3500.*Gaudi::Units::mm),	// r min for calo medium field gradient region
	m_caloR3		(3700.*Gaudi::Units::mm),	// r min for calo outer flux return region
	m_caloR4		(3800.*Gaudi::Units::mm),	// r max for calo medium field gradient region
	m_caloZ0		(2350.*Gaudi::Units::mm),	// z min for calo medium field gradient region
	m_caloZ1		(2600.*Gaudi::Units::mm),	// z min for calo high field gradient region
	m_caloZ2		(3600.*Gaudi::Units::mm),	// z max for calo high field gradient region
	m_caloZ3		(6000.*Gaudi::Units::mm),	// z min for calo outer flux return region
	m_inDetR0		(400.*Gaudi::Units::mm),	// end of central barrel near constant field region
	m_inDetR1		(350.*Gaudi::Units::mm),	// inner radius of middle/outer transition region
	m_inDetR2		(800.*Gaudi::Units::mm),	// outer radius of low field gradient field region
	m_inDetZ0		(350.*Gaudi::Units::mm),	// end of central barrel near constant field region
	m_inDetZ1		(420.*Gaudi::Units::mm),	// start of well behaved transition region
	m_inDetZ2		(700.*Gaudi::Units::mm),	// start of endcap region
	m_momentumThreshold	(1./20.*Gaudi::Units::MeV),	// protection against loopers
	m_momentumWarnThreshold	(1./450.*Gaudi::Units::MeV),	// warning threshold for intersection failure
	m_muonR0		(4300.*Gaudi::Units::mm),	// inner radius of barrel toroid region
	m_muonZ0		(6600.*Gaudi::Units::mm),	// start of endcap toroid region
	m_shortStepMax		(3.0*Gaudi::Units::mm),
	m_shortStepMin		(10.*Gaudi::Units::nanometer),
	m_solenoidR		(1300.*Gaudi::Units::mm),	// r max after coil (will take small steps near coil)
	m_solenoidZ		(3500.*Gaudi::Units::mm),	// z end of InDet region
	m_stepMax0		(8.0*Gaudi::Units::mm),
	m_stepMax1		(40.0*Gaudi::Units::mm),
	m_stepMax2		(80.0*Gaudi::Units::mm),
	m_stepMax3		(160.0*Gaudi::Units::mm),
	m_stepMax4		(320.0*Gaudi::Units::mm),
	m_stepsUntilTrapped	(2000),
	m_third			(1./3.),
	m_toroidR0		(1850.0*Gaudi::Units::mm),	// endcap toroid central field - inner radius
	m_toroidR1		(3500.0*Gaudi::Units::mm),	// endcap toroid central field - outer radius
	m_toroidR2		(6000.0*Gaudi::Units::mm),	// after inner barrel or outer endcap coil
	m_toroidR3		(6500.0*Gaudi::Units::mm),	// toroid region -  radius above which long steps OK
	m_toroidZ0		(7000.0*Gaudi::Units::mm),	// endcap - near iron structure
	m_toroidZ1		(8000.0*Gaudi::Units::mm),	// endcap - high field gradient
	m_toroidZ2		(8700.0*Gaudi::Units::mm),	// barrel - before coil end loop
	m_toroidZ3		(9100.0*Gaudi::Units::mm),	// barrel - nearing coil end loop
	m_toroidZ4		(9500.0*Gaudi::Units::mm),	// endcap toroid full field and barrel coil regions
	m_toroidZ5		(9800.0*Gaudi::Units::mm),	// endcap toroid central field - inner z
	m_toroidZ6		(11400.0*Gaudi::Units::mm),	// endcap toroid central field - outer z
	m_toroidZ7		(12900.0*Gaudi::Units::mm),	// toroid exit fringe fields
	m_toroidZ8		(14000.0*Gaudi::Units::mm),	// essentially out of any toroid fields
	m_cOverP{},
	m_direction{},
	m_distance{},
	m_fieldValue{},
	m_intersectionNumber	(0),
	m_maxSteps{},
	m_pathLength{},
	m_position{},
	m_previousDistance{},
	m_qOverP{},
	m_rCurrent{},
	m_rStart{},
	m_sinTheta{},
	m_stepLength{},
	m_trapped		(false),
	m_zStart{},
	m_countExtrapolations	(0),
	m_countShortStep	(0),
	m_countStep		(0),
	m_countStepReduction	(0)
{
    declareInterface<Trk::IIntersector>(this);
    declareProperty("MagFieldSvc",		m_magFieldSvc ); 
    declareProperty("ProductionMode",		m_productionMode);
}

RungeKuttaIntersector::~RungeKuttaIntersector (void)
{}
 
StatusCode
RungeKuttaIntersector::initialize(){
    // print name and package version
    ATH_MSG_DEBUG( "RungeKuttaIntersector::initialize()" << " - package version " << PACKAGE_VERSION );

    // initialize base class
    if (StatusCode::SUCCESS != AlgTool::initialize()) return StatusCode::FAILURE;

    // retrieve MagneticFieldTool and StraightLineIntersector
    if (! m_magFieldSvc.empty()) ATH_CHECK(m_magFieldSvc.retrieve());
	
	  ATH_MSG_INFO( "Retrieved service " << m_magFieldSvc );
	
    // productionMode gives steplengths tuned for use in production (adequate precision),
    // otherwise take very small steps for maximum precision but with a heavy execution penalty
    if (! m_productionMode){
      m_shortStepMax		= 500.*Gaudi::Units::nanometer;
      m_stepMax0		= 1.8*Gaudi::Units::mm;
      m_stepMax1		= 1.8*Gaudi::Units::mm;
      m_stepMax2		= 1.8*Gaudi::Units::mm;
      m_stepMax3		= 1.8*Gaudi::Units::mm;
      m_stepMax4		= 1.8*Gaudi::Units::mm;
      m_stepsUntilTrapped	= 20000;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode
RungeKuttaIntersector::finalize()
{
    msg(MSG::INFO)  << "finalized after " << m_countExtrapolations << " extrapolations,";
    if (m_countExtrapolations)
    {
	double norm = 1./static_cast<double>(m_countExtrapolations);
	msg(MSG::INFO)   << std::setiosflags(std::ios::fixed)
			 << " taking an average" << std::setw(7) << std::setprecision(1)
			 << norm*static_cast<double>(m_countStep)
			 << " full steps," << std::setw(5) << std::setprecision(2)
			 << norm*static_cast<double>(m_countStepReduction)
 			 << " step reductions and" << std::setw(5) << std::setprecision(2)
			 << norm*static_cast<double>(m_countShortStep)
			 << " short final steps";
    }
    msg(MSG::INFO)   << endmsg;

    return StatusCode::SUCCESS;
}

/**IIntersector interface method for general Surface type */
const Trk::TrackSurfaceIntersection*
RungeKuttaIntersector::intersectSurface(const Surface&		surface,
					const TrackSurfaceIntersection*	trackIntersection,
					const double      	qOverP)
{
    // trap low momentum
    if (std::abs(qOverP) > m_momentumThreshold)
    {
	ATH_MSG_DEBUG(" trapped as below momentum threshold" );
	m_intersectionNumber = 0;
	return 0;
    }
    const PlaneSurface* plane			= dynamic_cast<const PlaneSurface*>(&surface);
    if (plane)		return intersectPlaneSurface(*plane,trackIntersection,qOverP);
    
    const StraightLineSurface* straightLine	= dynamic_cast<const StraightLineSurface*>(&surface);
    if (straightLine)	return approachStraightLineSurface(*straightLine,trackIntersection,qOverP);

    const CylinderSurface* cylinder		= dynamic_cast<const CylinderSurface*>(&surface);
    if (cylinder)	return intersectCylinderSurface(*cylinder,trackIntersection,qOverP);
	
    const DiscSurface* disc			= dynamic_cast<const DiscSurface*>(&surface);
    if (disc)		return intersectDiscSurface(*disc,trackIntersection,qOverP);
    
    const PerigeeSurface* perigee		= dynamic_cast<const PerigeeSurface*>(&surface);
    if (perigee)	return approachPerigeeSurface(*perigee,trackIntersection,qOverP);
    
    ATH_MSG_WARNING( " unrecognized Surface" );
    m_intersectionNumber = 0;
    return 0;
}
                                    
/**IIntersector interface method for specific Surface type : PerigeeSurface */
const Trk::TrackSurfaceIntersection*
RungeKuttaIntersector::approachPerigeeSurface(const PerigeeSurface&		surface,
					      const TrackSurfaceIntersection*	trackIntersection,
					      const double			qOverP)
{
    // set member data
    setCache(trackIntersection, qOverP);

    // straight line distance along track to closest approach to line
    const Amg::Vector3D& lineDirection	= (surface.transform().rotation()).col(2);
    distanceToLine (surface.center(),lineDirection);
    m_maxSteps				= m_countStep + m_stepsUntilTrapped;
    m_previousDistance			= 1.1*m_distance;

    // integration loop (step)
    while (m_distance > m_shortStepMax && notTrapped())
    {
	assignStepLength();
	step();
	distanceToLine (surface.center(),lineDirection);
    }

    // if intersection OK: make short final step to surface - assuming constant field
    if (! m_trapped)
    {
	if (m_distance > m_shortStepMin)	shortStep();
	return newIntersection(surface);
    }

    // trapped: ensure cache reset
    m_intersectionNumber = 0;
    if (msgLvl(MSG::DEBUG))			debugFailure (surface);
    return 0;
}
	
/**IIntersector interface method for specific Surface type : StraightLineSurface */
const Trk::TrackSurfaceIntersection*
RungeKuttaIntersector::approachStraightLineSurface(const StraightLineSurface&		surface,
						   const TrackSurfaceIntersection*	trackIntersection,
						   const double      			qOverP)
{
    // set member data
    setCache(trackIntersection, qOverP);

    // straight line distance along track to closest approach to line
    const Amg::Vector3D& lineDirection	= (surface.transform().rotation()).col(2);
    distanceToLine (surface.center(),lineDirection);
    m_maxSteps				= m_countStep + m_stepsUntilTrapped;
    m_previousDistance			= 1.1*m_distance;

    // integration loop (step)
    while (m_distance > m_shortStepMax && notTrapped())
    {
	assignStepLength();
	step();
	distanceToLine (surface.center(),lineDirection);
    }

    // if intersection OK: make short final step assuming constant field
    if (! m_trapped)
    {
	if (m_distance > m_shortStepMin)	shortStep();
	return newIntersection(surface);
    }

    // trapped: ensure cache reset
    m_intersectionNumber = 0;
    if (msgLvl(MSG::DEBUG))			debugFailure (surface);
    return 0;
}
            
/**IIntersector interface method for specific Surface type : CylinderSurface */
const Trk::TrackSurfaceIntersection*
RungeKuttaIntersector::intersectCylinderSurface (const CylinderSurface&			surface,
						 const TrackSurfaceIntersection*	trackIntersection,
						 const double				qOverP)
{
    // set member data
    setCache(trackIntersection, qOverP);

    // calculate straight line distance along track to intersect with cylinder radius
    double cylinderRadius	= (surface.globalReferencePoint() - surface.center()).perp();
    Amg::Vector3D offset	= surface.center() - m_position;
    double rCurrent		= offset.perp();
    distanceToCylinder (cylinderRadius,rCurrent,offset);
    m_maxSteps			= m_countStep + m_stepsUntilTrapped;
    m_previousDistance		= 1.1*m_distance;

    // integration loop (step)
    while (m_distance > m_shortStepMax && ! m_trapped && notTrapped())
    {
	assignStepLength();
	double rPrevious= rCurrent;
	step();
	offset		= surface.center() - m_position;
	rCurrent	= offset.perp();
	double deltaR1	= rCurrent - rPrevious;
	double deltaR2	= cylinderRadius - rCurrent;	
	double scale   	= 1.;
	if (deltaR1 != 0.) scale = deltaR2 / deltaR1;
	if (scale < 1.)
	{
	    m_stepLength *= scale;
	    if (scale < -1.) m_trapped = true;
	}
	m_distance	= std::abs(m_stepLength);
    }

    // if intersection OK: make short final step assuming constant field
    if (! m_trapped
	&& std::abs(cylinderRadius - rCurrent) < m_shortStepMax)
    {
	distanceToCylinder (cylinderRadius,rCurrent,offset);	
	// protect against divergence (looping)
	if (m_distance < m_shortStepMax)
	{
	    if (m_distance > m_shortStepMin) shortStep();
	    return newIntersection(surface);
	}
    }

    // trapped: ensure cache reset
    m_trapped			= true;
    m_intersectionNumber	= 0;
    if (msgLvl(MSG::DEBUG))	debugFailure (surface);
    return 0;
}

/**IIntersector interface method for specific Surface type : DiscSurface */
const Trk::TrackSurfaceIntersection*
RungeKuttaIntersector::intersectDiscSurface (const DiscSurface&			surface,
					     const TrackSurfaceIntersection*	trackIntersection,
					     const double			qOverP)
{
    setCache(trackIntersection, qOverP);

    // straight line distance along track to intersect with disc
    distanceToDisc (surface.center().z());
    m_maxSteps		= m_countStep + m_stepsUntilTrapped;
    m_previousDistance 	= 1.1*m_distance;
    
    // integration loop
    while (std::abs(m_stepLength) > m_shortStepMax && notTrapped())
    {
	assignStepLength();
	step();
	distanceToDisc (surface.center().z());
    }

    // if intersection OK: make short final step assuming constant field
    if (! m_trapped)
    {
	if (std::abs(m_stepLength) > m_shortStepMin)	shortStep();
	return newIntersection(surface);
    }

    // trapped: ensure cache reset
    m_intersectionNumber = 0;
    if (msgLvl(MSG::DEBUG))				debugFailure (surface);
    return 0;
}

/**IIntersector interface method for specific Surface type : PlaneSurface */
const Trk::TrackSurfaceIntersection*
RungeKuttaIntersector::intersectPlaneSurface(const PlaneSurface&		surface,
					     const TrackSurfaceIntersection*	trackIntersection,
					     const double			qOverP)
{
    // set member data
    setCache(trackIntersection, qOverP);

    // straight line distance along track to intersect with plane
    distanceToPlane (surface.center(),surface.normal());
    m_maxSteps		= m_countStep + m_stepsUntilTrapped;
    m_previousDistance	= 1.1*m_distance;
    
    // integration loop (step)
    while (m_distance > m_shortStepMax && notTrapped())
    {
	assignStepLength();
	step();
	distanceToPlane (surface.center(),surface.normal());
    }

    // if intersection OK: make short final step assuming constant field
    if (! m_trapped)
    {
	if (m_distance > m_shortStepMin)	shortStep();
	return newIntersection(surface);
    }

    // trapped: ensure cache reset
    m_intersectionNumber = 0;
    if (msgLvl(MSG::DEBUG))			debugFailure (surface);
    return 0;
}


//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>

void
RungeKuttaIntersector::assignStepLength (void)
{
    // step length assigned according to abs value of current rz position
    // use default (large) value for most regions of InDet and Calo
    // TODO: consider bounded steps acc region => solves in/outwards problem
    double zCurrent	= std::abs(m_position.z());
    double stepMax	= m_stepMax3;
    // m_stepFlag		= 0;

    // first treat InDet and Solenoid regions
    // TODO: so far validated out to R = 1.0m
    if (m_rCurrent < m_solenoidR && zCurrent < m_solenoidZ)
    {
	if (m_rCurrent < m_inDetR2)
	{
	    // inner barrel, forward tracks and endcap regions generally happy with long steps
	    // double number of steps for middle barrel and much of the transition region
	    if (zCurrent < m_inDetZ0 && m_rCurrent < m_inDetR0)
	    {
		// inner barrel
		// stepMax = m_stepMax3;
	    }
	    else if (zCurrent > m_inDetZ2 || m_sinTheta < 0.35)
	    {
		// endcap and far forward
		// stepMax = m_stepMax3;
	    }
	
	    // double number of steps for most of the remainder
	    else if (zCurrent > m_inDetZ1 && m_sinTheta < 0.60)
	    {
		// barrel:endcap transition
		stepMax = m_stepMax2;
	    }
	    else if (m_rCurrent > m_inDetR0)
	    {
		// middle barrel
		stepMax = m_stepMax2;
	    }

	    // the remaining transition region requires much shorter steps
	    else
	    {
		if (m_rCurrent > m_inDetR1)
		{
		    // middle/outer transition region
		    stepMax = m_stepMax1;
		}
		else
		{
		    // inner transition (anomalous behaviour causing low pt track rotation)
		    stepMax = m_stepMax0;
		}
	    }
	}
	// take care when moving to region with shorter steps
	// FIXME : bug somewhere (Precise not precise....)  check final/short step...
	// else if (m_rCurrent < m_solenoidR)

	// field less uniform (approaching coil structure)
	else if (m_rCurrent < 1000.)	// 1000
	{
	    if (zCurrent > 700.)
	    {
		// stepMax = m_stepMax3;
		// exception at higher radii (influence of coil structure)
		if (m_rCurrent > 850. )	stepMax = m_stepMax2;
	    }
	    // // double number of steps for most of the remainder
	    else if (zCurrent > 420. && m_sinTheta < 0.60)
	    {
		// barrel:endcap transition
		stepMax = m_stepMax2;
	    }
	    else
	    {
		// decrease step length with increasing radius
		if (m_rCurrent > 900. )
		{
		    // coil structure
		    stepMax = m_stepMax0;
		}
		else
		{
		    // middle
		    stepMax = m_stepMax1;
		}
	    }
	}
	else
	{
	    // field less uniform in vicinity of solenoid coils
	    stepMax = m_stepMax1;
	    if (zCurrent < 3000.) stepMax = m_stepMax0;
	}
    }
    
    // secondly treat MuonSpectrometer regions
    // start with central barrel
    else if (m_rCurrent > m_muonR0 && zCurrent < m_toroidZ4)	// Z3
    {
	// ++m_count0;
	// optimize - only refresh phi occasionally
	double period   =  M_PI/4.;
	double phi	=  m_position.phi() + 0.5*period;
	if (phi < 0.) phi += 2.*M_PI;   
	int n           =  static_cast<int>(phi/period);
	phi             -= static_cast<double>(n)*period;
	if (phi > 0.5*period) phi = period - phi;

	// sol 1: 12.1 full steps, 0.34 reduc, 0 short
	// sol 2: 12.0 full steps, 0.34 reduc, 0 short
	// sol 3: 11.9 full steps, 0.34 reduc, 0 short  (tidy-up)
	double radius	= m_rCurrent;
	if (m_stepLength < 0.) radius -= m_stepMax2;
	if (zCurrent > m_toroidZ3 && phi < 0.04)
	{
	    // fringe and coil regions 
	    stepMax 	= m_stepMax1;		////   2 = 			3/ 0.
	    // m_stepFlag	= 22;
	    // ++m_count5;
	}
	else if (radius < 5300.)
	{
	    // fringe and coil regions 
	    stepMax 	= m_stepMax1;		////   2 = 			3/ 0.482
	    // m_stepFlag	= 21;
	    // ++m_count1;
	}
	// note: long sector (phi>0.175) has max 1/3 outer steps as finishes at lower r
	// try: phi-extended long sector with stepMax3 for all  r > 5500
	else if (phi > 0.04
		 || (radius > 5500.
		     && (radius < 9000.
			 || (phi > 0.028
			     && radius < 9350.))))	// (extended) long sector
	{
	    // central 'aircore' between coils
	    stepMax 	= m_stepMax3;		////   3 = 0;			3/ 0.470
	    // m_stepFlag	= 27;
	    // ++m_count2;
	}
	else if (radius > 5500. && radius < 9350.)
	{
	    // outer coil fringe region 
	    stepMax 	= m_stepMax2;		////   2 = 0			3/ 0.003
	    // m_stepFlag	= 28;
	    // ++m_count3;
	}
	else
	{
	    // outer coil region 
	    stepMax	= m_stepMax1;		////   2 = 0			3/ 0.045
	    // m_stepFlag	= 29;
	    // ++m_count4;
	}
    }
    else if (m_rCurrent > m_muonR0 || zCurrent > m_muonZ0)
    {
	if (zCurrent > m_toroidZ4)
	{
	    if (zCurrent > m_toroidZ8)
	    {
		// essentially out of any toroid fields
		stepMax = m_stepMax4;
	    }
	    else if (zCurrent > m_toroidZ7)
	    {
		// toroid exit fringe fields
		stepMax = m_stepMax3;
	    }
	    else if (m_rCurrent > m_toroidR0 && m_rCurrent < m_toroidR1
		     && zCurrent > m_toroidZ5 && zCurrent < m_toroidZ6)
	    {
		// endcap toroid central field
		stepMax = m_stepMax3;
	    }
	    else
	    {
		// endcap toroid high field gradient and barrel coil regions
		stepMax = m_stepMax1;
	    }
	}
	else if (m_rCurrent > m_toroidR3)
	{
	    // main region of barrel toroid :
	    // generally take long steps but with care near coil end loop
	    if (zCurrent < m_toroidZ2)
	    {
		stepMax = m_stepMax4;
		// if (m_rCurrent > 9000.) stepMax = m_stepMax3;
		// if (m_rCurrent > 9400.) stepMax = m_stepMax2;
	    }
	    else if (zCurrent < m_toroidZ3)
	    {
		stepMax = m_stepMax3;
	    }
	    else
	    {
		stepMax = m_stepMax1;
	    }	
	}
	else if (m_rCurrent > m_toroidR2)
	{
	    // toroid field after inner barrel or outer endcap coil
	    stepMax = m_stepMax3;
	}
	else if (zCurrent < m_toroidZ0 || zCurrent > m_toroidZ1 || m_rCurrent > m_muonR0)
	{
	    // small steps in toroid high field regions and near iron structures
	    stepMax = m_stepMax1;
	}
	else
	{
	    // endcap toroid entrance fringe fields
	    stepMax = m_stepMax3;
	}
    }

    // finally assign Calo regions
    else
    {
	if ((m_rCurrent < m_solenoidR)
	    || (m_rCurrent > m_caloR0 && m_rCurrent < m_caloR1)
	    ||  m_rCurrent > m_caloR3 
	    || (zCurrent > m_caloZ1 && zCurrent < m_caloZ2)
	    ||  zCurrent > m_caloZ3)
	{
	    stepMax = m_stepMax1;
	}
	else if ((m_rCurrent > m_caloR2 && m_rCurrent < m_caloR4)
		 || zCurrent > m_caloZ0)
	{
	    stepMax = m_stepMax2;
	}
    }

    // finally reset stepLength as necessary
    if (std::abs(m_stepLength) < stepMax) return;
    if (m_stepLength > 0.)
    {
	m_stepLength =  stepMax;
    }
    else
    {
	m_stepLength = -stepMax;
    }
}

void
RungeKuttaIntersector::debugFailure (const Surface& surface)
{
    // forget low momentum (prone to looping...)
    if (std::abs(m_qOverP) > m_momentumWarnThreshold)	return;

    double pt	= 1.E+8;
    if (m_qOverP != 0.) pt = m_sinTheta/(m_qOverP*Gaudi::Units::GeV);
    
    MsgStream log(msgSvc(), name());
    if (m_rCurrent > m_rStart)
    {
	log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right)
	    << " fail to intersect surface when extrapolating outwards from R,Z"
	    << std::setw(8) << std::setprecision(1) << m_rStart << ","
	    << std::setw(7) << std::setprecision(0) << m_zStart << " mm, with pt"
	    << std::setw(7) << std::setprecision(2) << pt << " GeV, direction eta"
	    << std::setw(5) << std::setprecision(2) << m_direction.eta();
    }
    else
    {
	log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right)
	    << " fail to intersect surface when extrapolating inwards from R,Z"
	    << std::setw(8) << std::setprecision(1) << m_rStart << ","
	    << std::setw(7) << std::setprecision(0) << m_zStart << " mm, with pt"
	    << std::setw(7) << std::setprecision(2) << pt << " GeV, direction eta"
	    << std::setw(5) << std::setprecision(2) << m_direction.eta();
    }
    // if (m_trapped) log << MSG::DEBUG << " looping in mag field ";
    log << MSG::DEBUG << endmsg;
    
    if (dynamic_cast<const PlaneSurface*>(&surface))
    {
	distanceToPlane (surface.center(),surface.normal());
	log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right) << "   PlaneSurface"
	    << " at R,Z" << std::setw(8) << std::setprecision(1) << surface.center().perp() << ","
	    << std::setw(7) << std::setprecision(0) << surface.center().z()
	    << "  at line distance " << std::setw(9) << std::setprecision(1) << m_stepLength;
	// if (m_trapped) log << MSG::DEBUG << " looping in mag field ";
	log << MSG::DEBUG << endmsg;
    }
    else if (dynamic_cast<const CylinderSurface*>(&surface))
    {
	double cylinderRadius	= (surface.globalReferencePoint() - surface.center()).perp();
	Amg::Vector3D offset	= surface.center() - m_position;
	double rCurrent		= offset.perp();
	distanceToCylinder (cylinderRadius,rCurrent,offset);
	if (m_distance < m_shortStepMin)
	{
	    log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right)
		<< "  closest approach to CylinderSurface at radius "
		<< std::setw(9) << std::setprecision(4) << rCurrent
		<< " mm.  Cylinder radius " << std::setw(9) << std::setprecision(4) << cylinderRadius << " mm"
		<< endmsg;
	}
	else
	{
	    log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right) << "  CylinderSurface"
		<< "  radius " << std::setw(6) << std::setprecision(1) << cylinderRadius
		<< "  rCurrent " << std::setw(6) << std::setprecision(1) << rCurrent
		<< "  distance " << std::setw(6) << std::setprecision(1) << m_stepLength;
	    if (m_trapped) log << MSG::DEBUG << " looping in mag field ";
	    log << MSG::DEBUG << endmsg;
	}
    }
    else if (dynamic_cast<const DiscSurface*>(&surface))
    {	
	distanceToDisc (surface.center().z());
	log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right) << "   DiscSurface"
	    << " at R,Z" << std::setw(8) << std::setprecision(1) << surface.center().perp() << ","
	    << std::setw(7) << std::setprecision(0) << surface.center().z()
	    << "  at line distance " << std::setw(9) << std::setprecision(1) << m_stepLength;
	if (m_trapped) log << MSG::DEBUG << " looping in mag field ";
	log << MSG::DEBUG << endmsg;
    }
    else if (dynamic_cast<const PerigeeSurface*>(&surface))
    {
	log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right) << "   PerigeeSurface  "
	    << endmsg;
    }
    else if (dynamic_cast<const StraightLineSurface*>(&surface))
    {
	log << MSG::DEBUG << std::setiosflags(std::ios::fixed|std::ios::right) << "   StraightLineSurface  "
	    << endmsg;
    }
    log << MSG::DEBUG << endmsg;
}

bool
RungeKuttaIntersector::notTrapped (void)
{
    // catch loopers
    if (m_distance > m_previousDistance)
    {
	m_trapped = true;
	// 	    ATH_MSG_INFO( 0.001/m_qOverP << " trapped: distance,previous  "
	// 			  << m_distance << "  " << m_previousDistance );
	return false;
    }

    if (m_countStep > m_maxSteps)
    {
	m_trapped = true;
// 	ATH_MSG_INFO( 0.001/m_qOverP << " trapped after " << m_stepsUntilTrapped
// 		  << " steps : step,distance  "
// 		  << m_stepLength << "  " << m_previousDistance
// 		  << "  position,direction " << m_position << " " << m_direction );
	return false;
    }

    // store previous value to check steps are converging (with 10% safety factor)
    m_previousDistance = 1.1*m_distance;

    return true;
}

void
RungeKuttaIntersector::shortStep (void)
{
    // as step except for const field assumption
    double	  stepOverP  	=  0.5*m_stepLength*m_cOverP;
    Amg::Vector3D product0	=  stepOverP*m_direction.cross(m_fieldValue);
    // intermediate point (half way through step)
    Amg::Vector3D direction1	=  m_direction + product0;
    Amg::Vector3D product1	=  stepOverP*direction1.cross(m_fieldValue);
    Amg::Vector3D direction2	=  m_direction + product1;
    Amg::Vector3D product2	=  stepOverP*direction2.cross(m_fieldValue);
    // step end point
    m_position			+= m_stepLength*(m_direction + m_third*(product0+product1+product2));
    Amg::Vector3D direction3	=  m_direction + 2.*product2;
    Amg::Vector3D product3	=  stepOverP*direction3.cross(m_fieldValue);
    m_direction			+= m_third*(product0+product3 + 2.*(product1+product2));
    m_rCurrent			=  m_position.perp();
    m_sinTheta			=  m_direction.perp();
    m_pathLength		+= m_stepLength;
    ++m_countShortStep;
}

void
RungeKuttaIntersector::step (void)
{
    double	  stepOverP  	=  0.5*m_stepLength*m_cOverP;
    Amg::Vector3D product0	=  stepOverP*m_direction.cross(m_fieldValue);

    // intermediate field look-up point (half way through step)
    Amg::Vector3D position1	=  m_position + 0.5*m_stepLength*(m_direction + 0.5*product0);
    Amg::Vector3D fieldValue1	=  field(position1);
    Amg::Vector3D direction1	=  m_direction + product0;
    Amg::Vector3D product1	=  stepOverP*direction1.cross(fieldValue1);
    Amg::Vector3D direction2	=  m_direction + product1;
    Amg::Vector3D product2	=  stepOverP*direction2.cross(fieldValue1);

    // field look-up at step end point
    Amg::Vector3D offsetAtEnd	=  m_stepLength*(m_direction + m_third*(product0+product1+product2));
    Amg::Vector3D fieldAtEnd	=  field(m_position+offsetAtEnd);

    // redo with reduced stepLength if non-uniform field derivative detected
    if ((fieldValue1 - 0.5*(m_fieldValue + fieldAtEnd)).mag() > 0.00001
	&& std::abs(m_stepLength) > m_stepMax0)
    {
	if (m_stepLength > 0.)
	{
	    // ATH_MSG_INFO(" reduction at stepLength " << m_stepLength
	    // 		 << " at " << m_position.perp() << ", " << m_position.z());
	    
	    // if (std::abs(m_stepLength) > m_stepMax1 && m_stepFlag > 20 && m_stepFlag < 30)
	    // {
	    // 	double period   =  M_PI/4.;
	    // 	double phi	=  m_position.phi() + 0.5*period;
	    // 	if (phi < 0.) phi += 2.*M_PI;   
	    //     int n           =  static_cast<int>(phi/period);
	    //     phi             -= static_cast<double>(n)*period;
	    //     if (phi > 0.5*period) phi = period - phi;
 	    // 	ATH_MSG_INFO(" reduction at stepLength " << m_stepLength
	    // 		     << "  stepFlag " << m_stepFlag
 	    // 		     << " at " << m_position.perp() << ", " << m_position.z()
	    // 		     << "  phi " << phi << "  field " << 1000.*m_fieldValue);
	    // }
	    m_stepLength =  m_stepMax0;
	}
	else
	{
	    //  if (std::abs(m_stepLength) > m_stepMax1 && m_stepFlag > 20 && m_stepFlag < 30)
	    // {
	    // 	double period   =  M_PI/4.;
	    // 	double phi	=  m_position.phi() + 0.5*period;
	    // 	if (phi < 0.) phi += 2.*M_PI;   
	    //     int n           =  static_cast<int>(phi/period);
	    //     phi             -= static_cast<double>(n)*period;
	    //     if (phi > 0.5*period) phi = period - phi;
 	    // 	ATH_MSG_INFO(" reduction at stepLength " << m_stepLength
	    // 		     << "  stepFlag " << m_stepFlag
 	    // 		     << " at " << m_position.perp() << ", " << m_position.z()
	    // 		     << "  phi " << phi << "  field " << 1000.*m_fieldValue);
	    // }
	    m_stepLength = -m_stepMax0;
	}	    
	++m_countStepReduction;
	stepOverP  			=  0.5*m_stepLength*m_cOverP;
	product0			=  stepOverP*m_direction.cross(m_fieldValue);
	// intermediate point (half way through step)
	Amg::Vector3D position1p	=  m_position + 0.5*m_stepLength*(m_direction + 0.5*product0);
	Amg::Vector3D fieldValue1p	=  field(position1p);
	Amg::Vector3D direction1p	=  m_direction + product0;
	product1			=  stepOverP*direction1p.cross(fieldValue1p);
	Amg::Vector3D direction2p	=  m_direction + product1;
	product2			=  stepOverP*direction2p.cross(fieldValue1p);
	// step end point
	offsetAtEnd			=  m_stepLength *
					   (m_direction + m_third*(product0+product1+product2));
	fieldAtEnd			=  field(m_position+offsetAtEnd);
    }

    Amg::Vector3D direction3	=  m_direction + 2.*product2;
    Amg::Vector3D product3	=  stepOverP*direction3.cross(fieldAtEnd);
    m_direction			+= m_third*(product0+product3 + 2.*(product1+product2));
    m_fieldValue		=  fieldAtEnd;
    m_position			+= offsetAtEnd;
    m_rCurrent			=  m_position.perp();
    m_sinTheta			=  m_direction.perp();
    m_pathLength		+= m_stepLength;
    ++m_countStep;
}

} // end of namespace
