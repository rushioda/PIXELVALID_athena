/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidCaloEnergyParam
//  AlgTool returning the parametrized value for the calorimeter energy
//  deposited by a traversing muon. The energy is parametrized according
//  to trackParameters given at the calo mid-surface.
//  
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "MuidCaloEnergyTools/MuidCaloEnergyParam.h"
#include "muonEvent/CaloEnergy.h"
#include "AthenaKernel/Units.h"

namespace Units = Athena::Units;

namespace Rec
{
 
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
 
MuidCaloEnergyParam::MuidCaloEnergyParam (const std::string&type,
					  const std::string&name,
					  const IInterface*parent)
    :	AthAlgTool		(type, name, parent),
	m_cosmics		(false),
	m_smoothingFraction	(0.5),
	m_binWidth(0.),
	m_etaOffset(0.),
	m_inverseWidth(0.),
	m_etaBin1(0),
	m_etaBin2(0),
	m_etaWeight1(0.),
	m_etaWeight2(0.),
	m_etaGranularity(0.)
{
    declareInterface<IMuidCaloEnergyParam>(this);
    declareProperty ("Cosmics",			m_cosmics);
    declareProperty ("SmoothingFraction",	m_smoothingFraction);
}

MuidCaloEnergyParam::~MuidCaloEnergyParam (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
MuidCaloEnergyParam::initialize()
{
    ATH_MSG_DEBUG( " Initializing " );

    // set up eta bin width and precomputed smoothing constants
    m_binWidth		= 0.1;
    if (m_smoothingFraction < 0.) m_smoothingFraction = 0.;
    if (m_smoothingFraction > 1.) m_smoothingFraction = 1.;
    m_etaOffset		= 0.5*m_binWidth*m_smoothingFraction;
    m_inverseWidth	= 1./m_binWidth;
    
    // fix to constant term for ATLAS-GEO-10-00-00 --> derived using a pt=5GeV single muon sample
    // comparing the actual eloss and the expected according to the parametrisation
    // after comparison with data, this is only to be applied to barrel region
//     double fix[26]			= { 0.315388,   0.212444,   0.186079,   0.212734,   0.254688,
// 					    0.294132,   0.307659,   0.333515,   0.272366,   0.312063,
// 					    0.300346,   0.42782,    0.164785,   0.0766723, -0.0290494,
// 					    -0.0334845, 0.102646,   0.0255806, -0.0300964, -0.0214991,
// 					    0.0488582,  0.0592446,  0.0530347,  0.10965,   -0.00470456,
// 					    1.41556 };
    double fix[26]			= { 0.21892,    0.13412,    0.064942,   0.154388,   0.16903,
					    0.212424,   0.246268,   0.234471,   0.23724,    0.209337,
					    0.186304,   0.100489,   0.0740633,  0.0319171,  0.0352365,
					    0.0218442,  0.19691,    0.124486,   0.0882254,  0.132367,
					    0.181742,   0.213001,   0.192657,   0.219348,   0.261353,
					    0.142727};
    
    //Initialization of the mean energy deposition arrays
    //(function [0]+[1]*Log(0.0067*p*p)+[2]*p with GeV units)
    double meanEnergyLoss_p0[26]	= { 2.7909,     2.82994,    2.91726,    3.00969,    3.09838,
					    3.25242,    3.32701,    2.99668,    3.05648,    3.50282,
					    4.41603,    4.64789,    3.90948,    3.29712,    2.86013,
					    2.8295,     3.13659,    3.6972,     3.69033,    3.76311,
					    3.62784,    3.60054,    3.67615,    3.57151,    3.59287,
					    3.40605 };
    double meanEnergyLoss_p1[26]	= { 0.0497463,  0.0440959,  0.0331098,  0.0565649,  0.0721456,
					    0.0948259,  0.0825271,  0.0543386,  0.0790654,  0.0494549,
					    0.118818,   0.139855,   0.00587402, 0.0589792,  0.0461476,
					    0.0285649,  0.0460483,  0.130774,   0.0999097,  0.126468,
					    0.0302406,  0.0645617,  0.0416555,  0.0503394, -0.00448376,
					    0.135223 };
    double meanEnergyLoss_p2[26]	= { 0.0102245,  0.0113924,  0.0090614,  0.0105087,  0.0104329,
					    0.010759,   0.0103308,  0.011115,   0.00944668, 0.0126312,
					    0.0127012,  0.0162508,  0.0131209,  0.00842482, 0.00889747,
					    0.00755592, 0.00944529, 0.0106429,  0.0111037,  0.0113795,
					    0.0129818,  0.0128469,  0.0138801,  0.015034,   0.0135201,
					    0.0109176 };
    // tabulated coefficients for left assymetric error: pol1 with GeV units
    double meanEnergyLoss_left_p0[26]	= { 0.290254,   0.253085,   0.252006,   0.279803,   0.284166,
					    0.269267,   0.370476,   0.309723,   0.339191,   0.392122,
					    0.504243,   0.452033,   0.389579,   0.325196,   0.241338,
					    0.230355,   0.319177,   0.341208,   0.262456,   0.300411,
					    0.255884,   0.257605,   0.272939,   0.247617,   0.267175,
					    0.377071 };
    double meanEnergyLoss_left_p1[26]	= { 0.00570519, 0.00594466, 0.00519917, 0.0054666,  0.00552764,
					    0.00631414, 0.00539921, 0.00573473, 0.00508457, 0.00624818,
					    0.00691296, 0.00864038, 0.00640888, 0.00488251, 0.00507001,
					    0.00462523, 0.00529166, 0.00594752, 0.00640289, 0.00655527,
					    0.00631339, 0.00643479, 0.00749501, 0.00766192, 0.00637578,
					    0.00567618 };
    // tabulated coefficients for right assymetric error: pol1 with GeV units
    double meanEnergyLoss_right_p0[26]	= { 0.478493,   0.587466,   0.678961,   0.723157,   0.536312,
					    0.60896,    0.570827,   0.641256,   0.713216,   0.648768,
					    0.684193,   0.472149,   0.697594,   0.648713,   0.577021,
					    0.52603,    0.536587,   0.573851,   0.603372,   0.817339,
					    0.631103,   0.491342,   0.546488,   0.46241,    0.46241,
					    0.951949 };
    double meanEnergyLoss_right_p1[26]	= { 0.0250521,  0.0284547,  0.0202018,  0.0202702,  0.0257285,
					    0.0230366,  0.0240065,  0.0209904,  0.0220515,  0.0296286,
					    0.0314476,  0.0452839,  0.0343686,  0.0197284,  0.0217681,
					    0.018329,   0.0193914,  0.0305364,  0.0258112,  0.0272972,
					    0.0279697,  0.0228007,  0.0268285,  0.0312118,  0.0312118,
					    0.0205223};

    for (int i = 0; i < 26; ++i)
    {
	if (i < 13)
	{
	    m_meanEnergyLossP0[i]	= meanEnergyLoss_p0[i] + fix[i];
	}
	else
	{
	    m_meanEnergyLossP0[i]	= meanEnergyLoss_p0[i];
	}   
	m_meanEnergyLossP1[i]		= meanEnergyLoss_p1[i];
	m_meanEnergyLossP2[i]		= meanEnergyLoss_p2[i];
	m_meanEnergyLossErrorLeftP0[i]	= meanEnergyLoss_left_p0[i]; 
	m_meanEnergyLossErrorLeftP1[i]	= meanEnergyLoss_left_p1[i]; 
	m_meanEnergyLossErrorRightP0[i]	= meanEnergyLoss_right_p0[i]; 
	m_meanEnergyLossErrorRightP1[i]	= meanEnergyLoss_right_p1[i];
    }

    //Initialization of the mop energy deposition arrays
    // tabulated coefficients for MOP energy loss and for its error
    //  (function [0]+[1]*Log(0.0067*p*p)+[2]*p with GeV units)
    double mopEnergyLoss_p0[26]		= { 2.57291,    2.57581,    2.65592,    2.73852,    2.81242,
					    2.94167,    3.01323,    2.72711,    2.75402,    3.17494,
					    4.07206,    4.28525,    3.58247,    2.99343,    2.59807,
					    2.56842,    2.83854,    3.4031,     3.40046,    3.3927,
					    3.38412,    3.38123,    3.4004,     3.37481,    3.3171,
					    3.28507 };
    double mopEnergyLoss_p1[26]		= { 0.0464874,  0.0213596,  0.0113974,  0.0314924,  0.0283828,
					    0.0363767,  0.0405601,  0.0211039,  0.0273863,  0.0351794,
					    0.0381777,  0.079885,   0.0176414,  0.0280359,  0.0173828,
					    0.0280728,  0.0366678,  0.0416954,  0.029448,   0.0300134,
					    0.0262876,  0.0229897,  0.0386583,  0.0305293,  0.0214848,
					    0.0316924 };
    double mopEnergyLoss_p2[26]		= { 0.00172448, 0.00225414, 0.00225085, 0.00196091, 0.00229175,
					    0.00232876, 0.00248703, 0.00239371, 0.00204022, 0.00238259,
					    0.00387238, 0.00398731, 0.0030019,  0.00220271, 0.00173978,
					    0.00153614, 0.00224971, 0.00291137, 0.00297603, 0.00313257,
					    0.00330651, 0.00319543, 0.00287297, 0.00333396, 0.00301831,
					    0.0027259 };

    double mopEnergyLoss_sigma_p0[26]	= { 0.129825,   0.120123,   0.110096,   0.129288,   0.13066,
					    0.128621,   0.139855,   0.136144,   0.143521,   0.192355,
					    0.22055,    0.24224,    0.195584,   0.147344,   0.121476,
					    0.112492,   0.175842,   0.127176,   0.12163,    0.123569,
					    0.116129,   0.118597,   0.126694,   0.129753,   0.128578,
					    0.109232 };
    double mopEnergyLoss_sigma_p1[26]	= { 0.00117338, 0.00114104, 0.00116575, 0.00106617, 0.00121612,
					    0.0013242,  0.00133937, 0.00125934, 0.00114077, 0.00122792,
					    0.00190011, 0.00203161, 0.00127366, 0.00110695, 0.000916009,
					    0.000920279,0.00131875, 0.00147309, 0.0015842,  0.0016292,
					    0.00166247, 0.00151376, 0.00149124, 0.00163482, 0.00156624,
					    0.00148398 };

    for  (int i =0; i < 26; ++i)
    {
	if (i < 13)
	{
	    m_mopEnergyLossP0[i]	= mopEnergyLoss_p0[i] + fix[i];
	}
	else
	{
	    m_mopEnergyLossP0[i]	= mopEnergyLoss_p0[i];
	}   
	m_mopEnergyLossP1[i]		= mopEnergyLoss_p1[i];
	m_mopEnergyLossP2[i]		= mopEnergyLoss_p2[i];
	m_mopEnergyLossErrorP0[i]	= mopEnergyLoss_sigma_p0[i];
	m_mopEnergyLossErrorP1[i]	= mopEnergyLoss_sigma_p1[i];
    }

    // not sure the mop symmetric is still needed...
    double mopSymmetricEnergyLoss_p0[26]={2.70345,2.68656,2.75213,2.85555,2.95905,3.09844,3.19676,2.87172,2.92657,3.38863,4.28484,4.54497,3.77225,3.18316,2.72541,2.68793,3.08594,3.52879,3.5142,3.48087,3.50184,3.4893,3.52484,3.48467,3.44116,3.43226};
    double mopSymmetricEnergyLoss_p1[26]={0.0388166,0.00777747,0.0261084,0.0406787,0.0322767,0.038223,0.0409763,0.00096891,0.0439047,0.0430484,0.0419927,0.0766375,-0.000214255,0.0420977,-0.0143302,-0.0022226,0.0574891,0.0260815,-0.00223455,0.0016692,-0.00404322,-0.00620859,0.00603695,-0.0271089,0.0386722,0.0273018};
    double mopSymmetricEnergyLoss_p2[26]={0.00263823,0.00360428,0.00293494,0.00235128,0.00245869,0.00304957,0.00317534,0.00428433,0.00231015,0.0029778,0.00518089,0.00522931,0.00446165,0.0023943,0.00365962,0.00310547,0.00236659,0.00405503,0.00488483,0.00499606,0.0050601,0.0045985,0.00515479,0.00607963,0.00405829,0.00440702};
    for(int i = 0 ; i < 26; ++i)
    {
	m_mopSymmetricEnergyLossP0[i] = mopSymmetricEnergyLoss_p0[i];
	m_mopSymmetricEnergyLossP1[i] = mopSymmetricEnergyLoss_p1[i];
	m_mopSymmetricEnergyLossP2[i] = mopSymmetricEnergyLoss_p2[i];
    }

    // Initialize the em energy deposit fraction
    double emMopFraction[26]	= { 0.100105 , 0.101756 , 0.1033   , 0.103466  , 0.10543   ,
				    0.103732 , 0.106252 , 0.120923 , 0.116363  , 0.116984  ,
				    0.118591 , 0.104436 , 0.100136 , 0.0934386 , 0.104249  ,
				    0.122974 , 0.122026 , 0.113485 , 0.0978895 , 0.0967849 ,
				    0.100603 , 0.101762 , 0.104887 , 0.106119  , 0.104617  ,
				    0.100805 };
    for(int i = 0 ; i < 26; ++i)
    {
	m_emMopFraction[i] = emMopFraction[i];
    }
	
    //Initialization of the dead material map arrays
    double dead_eta[277]={0.005,0.015,0.025,0.035,0.045,0.055,0.065,0.075,0.085,0.095,0.105,0.115,0.125,0.135,0.145,0.155,0.165,0.175,0.185,0.195,0.205,0.215,0.225,0.235,0.245,0.255,0.265,0.275,0.285,0.295,0.305,0.315,0.325,0.335,0.345,0.355,0.365,0.375,0.385,0.395,0.405,0.415,0.425,0.435,0.445,0.455,0.465,0.475,0.485,0.495,0.505,0.515,0.525,0.535,0.545,0.555,0.565,0.575,0.585,0.595,0.605,0.615,0.625,0.635,0.645,0.655,0.665,0.675,0.685,0.695,0.705,0.715,0.725,0.735,0.745,0.755,0.765,0.775,0.785,0.795,0.805,0.815,0.825,0.835,0.845,0.855,0.865,0.875,0.885,0.895,0.905,0.915,0.925,0.935,0.945,0.955,0.965,0.975,0.985,0.995,1.005,1.015,1.025,1.035,1.045,1.055,1.065,1.075,1.085,1.095,1.105,1.115,1.125,1.135,1.145,1.155,1.165,1.175,1.185,1.195,1.205,1.215,1.225,1.235,1.245,1.255,1.265,1.275,1.285,1.295,1.305,1.315,1.325,1.335,1.345,1.355,1.365,1.375,1.385,1.395,1.405,1.415,1.425,1.435,1.445,1.455,1.465,1.475,1.485,1.495,1.505,1.515,1.525,1.535,1.545,1.555,1.565,1.575,1.585,1.595,1.605,1.615,1.625,1.635,1.645,1.655,1.665,1.675,1.685,1.695,1.705,1.715,1.725,1.735,1.745,1.755,1.765,1.775,1.785,1.795,1.805,1.815,1.825,1.835,1.845,1.855,1.865,1.875,1.885,1.895,1.905,1.915,1.925,1.935,1.945,1.955,1.965,1.975,1.985,1.995,2.005,2.015,2.025,2.035,2.045,2.055,2.065,2.075,2.085,2.095,2.105,2.115,2.125,2.135,2.145,2.155,2.165,2.175,2.185,2.195,2.205,2.215,2.225,2.235,2.245,2.255,2.265,2.275,2.285,2.295,2.305,2.315,2.325,2.335,2.345,2.355,2.365,2.375,2.385,2.395,2.405,2.415,2.425,2.435,2.445,2.455,2.465,2.475,2.485,2.495,2.505,2.515,2.525,2.535,2.545,2.555,2.565,2.575,2.585,2.595,2.605,2.615,2.625,2.635,2.645,2.655,2.665,2.675,2.685,2.695,2.705,2.715,2.725,2.735,2.745,2.755,2.765};
    double dead_x01[277]={1.8469,1.7213,1.7217,1.7222,1.7366,1.7402,1.7667,1.7847,1.7927,1.7968,1.8054,1.7959,1.7737,1.7626,1.7475,1.7259,1.7211,1.7032,1.6776,1.6731,1.6688,1.6516,1.6474,1.6497,1.6394,1.6384,1.6322,1.6288,1.6187,1.6251,1.6474,1.6547,1.6712,1.6758,1.6901,1.6935,1.6883,1.6966,1.7005,1.6978,1.7044,1.7184,1.7257,1.7322,1.7327,1.7402,1.7483,1.7332,1.7477,1.7599,1.783,1.7907,1.8299,1.8343,1.8427,1.8513,1.8271,1.8276,1.8345,1.8702,1.8821,1.8889,1.9048,1.9538,2.0558,2.1443,2.2201,2.2642,2.2656,2.2472,2.2776,2.2798,2.3388,2.3809,2.3763,2.3869,2.4254,2.4758,2.5282,2.5364,2.5637,2.5917,2.5999,2.6376,2.6386,2.6423,2.6479,2.674,2.676,2.6806,2.7002,2.6985,2.704,2.6898,2.7206,2.7414,2.7647,2.7856,2.7867,2.8078,2.8187,2.8461,2.8572,2.8544,2.8487,2.8796,2.9303,2.9401,2.928,2.9376,2.9437,3.0083,2.9823,2.9945,2.9809,3.0485,3.0494,3.0595,3.0642,3.1098,3.1232,3.1306,3.0995,3.1497,3.1399,3.1285,3.1489,3.1748,3.2728,3.2219,3.2252,3.2826,3.3505,3.4002,3.4492,3.4564,3.4622,3.4631,3.4628,3.5027,3.5506,3.6616,3.8859,4.0115,4.1715,4.3591,4.4486,4.4909,4.5445,4.5871,4.6396,4.398,3.6563,4.672,3.9715,3.0968,2.7105,2.7465,2.7719,2.7361,2.8049,2.7947,2.7961,2.8328,2.8847,2.8935,3.1969,3.5616,3.9473,3.3614,2.5151,2.3433,2.3557,2.0129,2.0082,2.0186,1.9549,1.9559,1.873,1.9516,1.9924,2.0921,2.1637,2.0858,2.0419,1.9849,2.0455,1.9273,1.8368,1.7655,1.779,1.726,1.724,1.7487,1.7251,1.6957,1.7127,1.6828,1.6412,1.6795,1.6421,1.631,1.6366,1.6835,1.6884,1.6811,1.6724,1.6534,1.6404,1.6498,1.6268,1.6205,1.6453,1.657,1.6372,1.6172,1.5865,1.561,1.5822,1.6654,1.6161,1.57,1.5173,1.4629,1.5068,1.4886,1.496,1.5121,1.4521,1.4178,1.4192,1.4095,1.42,1.4151,1.4129,1.3897,1.4006,1.3767,1.3668,1.352,1.3715,1.3606,1.3526,1.3903,1.4008,1.3913,1.363,1.3655,1.3634,1.366,1.3687,1.3626,1.3925,1.3764,1.3442,1.3366,1.3301,1.3277,1.3036,1.304,1.3068,1.3114,1.303,1.345,1.3567,1.3424,1.3092,1.3122,1.3137,1.3231,1.3236,1.2951,1.2928,1.3456,1.8221,2.0029,2.0075};
    double dead_x02[277]={27.3438,27.2208,27.2263,27.243,31.8853,28.1255,27.5756,27.0132,27.0413,27.0637,27.1041,27.1223,27.1304,27.1563,27.1702,27.1865,27.2222,27.8551,28.488,30.206,28.5837,28.0049,27.4459,27.5156,27.5572,27.6216,27.6856,27.7451,27.8147,27.8949,27.9914,28.078,28.1787,28.2678,28.3671,28.4603,29.1869,29.9369,31.3911,29.6731,29.6085,29.0849,29.2069,29.3229,29.4405,29.5618,29.6891,29.7915,29.9375,30.0806,30.2322,30.3764,30.5561,30.6942,30.8476,31.6978,31.8183,33.6375,33.149,32.3431,32.5023,31.9797,32.1619,32.3803,32.6547,32.9188,33.1731,33.3989,33.5851,33.7547,33.9763,34.1728,34.4293,34.676,35.3402,35.8687,36.8943,37.3228,37.0672,35.8788,30.0848,30.2997,30.4957,30.7282,30.917,31.1227,31.3252,31.5502,31.7627,31.9752,32.2057,32.4178,32.6401,32.8458,33.9831,34.4918,34.9247,35.4172,35.0033,35.2645,34.6033,34.8723,35.1359,35.8106,37.0775,37.4231,37.5118,37.6158,38.0132,38.5355,38.9488,40.1903,40.5554,39.3216,40.47,40.5203,40.3146,40.0119,40.0371,40.0744,40.0102,40.0563,39.9966,40.0213,40.0384,39.9564,38.3485,37.0049,36.011,34.3675,32.746,31.6373,30.2222,28.8545,31.2748,30.5887,29.1005,28.1896,28.9692,29.0944,29.5132,29.8182,30.2405,30.4836,30.6649,31.4598,31.8968,30.9419,32.2847,32.2785,29.4552,28.8184,27.8418,26.9469,26.5213,26.0314,26.0441,26.4897,26.9313,27.3156,27.7043,27.9047,28.1472,28.4043,28.6613,28.8634,29.3494,29.8874,30.4377,30.0112,29.3163,29.2913,29.4448,29.2385,29.3655,29.5045,29.5655,29.6873,29.7209,29.9143,30.0671,30.2626,30.4579,30.4513,30.5094,30.5531,30.7121,30.6904,30.6949,30.7176,30.8232,30.9326,31.0953,31.376,31.6177,31.8488,32.1461,32.4569,32.7735,33.1196,33.4516,33.7533,34.0885,34.4624,34.7467,34.9918,35.2164,35.4157,35.6083,35.812,35.9719,36.1537,36.3483,36.5131,36.65,36.7811,36.8951,37.0119,37.1696,37.3852,37.4656,37.5443,37.6143,37.6783,37.839,37.9339,38.0519,38.1968,38.2431,38.3125,38.4156,38.5057,38.6145,38.7049,38.797,38.8656,38.9668,39.031,39.108,39.177,39.2796,39.2613,39.3312,39.4452,39.6109,39.6561,39.7265,39.7582,39.7801,39.8325,40.0373,30.445,30.4259,30.3236,30.335,30.4099,30.4834,30.4998,30.549,30.6477,30.6725,30.7221,30.7792,30.8878,30.9677,31.025,31.0644,31.1425,31.2212,31.3097,31.392,31.4477,31.5309,31.6727,32.2409,32.5151,32.6164};
    double dead_x0tot[277]={108.064,114.015,102.328,103.979,108.575,99.904,107.609,114.499,110.58,105.503,111.6,109.618,107.369,108.663,108.656,108.724,107.947,111.123,113.801,114.927,110.352,113.048,109.105,112.754,110.352,112.808,113.401,111.948,113.811,111.447,114.069,115.163,115.728,113.215,116.055,115.433,116.233,118.157,119.787,118.035,117.5,117.333,116.777,119.61,119.342,121.258,119.331,121.887,122.346,122.314,122.692,122.296,123.452,124.217,125.029,126.766,127.559,129.726,129.524,130.828,128.679,129.477,129.923,131.236,132.793,132.212,132.597,133.999,131.29,129.22,126.212,126.427,127.45,124.279,119.29,115.547,114.276,116.323,117.272,117.058,112.01,112.727,114.061,115.384,115.753,114.081,112.184,111.034,109.526,109.854,111.763,113.335,114.989,116.799,119.382,121.044,122.683,123.843,123.798,124.567,122.571,122.951,124.179,124.977,128.058,131.607,135.404,139.132,142.167,146.221,150.063,154.225,158.159,160.614,164.357,168.218,171.728,174.947,177.505,181.286,183.258,186.901,192.562,189.001,189.092,185.551,181.202,177.128,168.268,159.719,156.125,151.878,149.137,144.408,141.419,138.169,133.679,130.259,128.061,126.093,123.544,121.135,119.087,116.403,113.911,112.066,109.821,105.606,104.274,101.77,100.493,99.9533,98.9401,98.3901,97.6717,97.3916,97.613,98.0975,98.6056,99.3877,99.9002,100.231,100.728,101.391,102.93,105.539,106.374,107.412,108.678,108.733,111.575,110.964,113.989,117.078,120.737,124.857,129.56,134.323,138.298,142.47,146.55,149.97,153.422,153.182,152.819,152.886,152.96,152.822,152.713,152.624,152.621,152.623,152.681,152.86,153.001,153.134,153.335,153.551,153.775,154.031,154.275,154.49,154.74,155.03,155.233,155.398,155.545,155.667,155.785,155.915,156.003,156.114,156.24,156.337,156.407,156.474,156.524,162.991,163.283,163.632,163.845,164.055,164.256,164.851,165.542,165.713,165.796,165.9,166.052,166.246,166.473,166.686,166.917,167.129,167.341,167.53,167.75,167.932,168.201,168.312,168.131,168.228,168.412,168.64,168.918,169.075,169.257,169.399,169.53,169.691,170.004,160.519,160.606,160.61,160.726,160.905,161.082,161.201,161.352,161.552,161.677,161.826,161.982,162.189,162.366,162.52,162.655,162.829,163.002,163.184,163.36,163.508,163.683,163.916,164.574,164.938,165.128};
    double dead_cryo0[277]={0.6732,0.6734,0.6735,0.6737,0.6739,0.6743,0.6746,0.6751,0.6757,0.6763,0.677,0.6777,0.6785,0.6794,0.6804,0.6813,0.6825,0.6836,0.6848,0.6861,0.6874,0.6889,0.6904,0.6919,0.6936,0.6952,0.697,0.6988,0.7007,0.7028,0.7048,0.707,0.7091,0.7114,0.7137,0.7162,0.7186,0.7212,0.7237,0.7265,0.7293,0.7321,0.735,0.7379,0.741,0.7442,0.7474,0.7507,0.754,0.7575,0.7609,0.7646,0.7682,0.7719,0.7758,0.7796,0.7836,0.7876,0.7918,0.796,0.8002,0.8046,0.8091,0.8136,0.8182,0.8229,0.8277,0.8326,0.8375,0.8425,0.8476,0.8528,0.8581,0.8634,0.869,0.8744,0.88,0.8857,0.8916,0.8975,0.9034,0.9095,0.9157,0.9219,0.9283,0.9347,0.9412,0.9479,0.9546,0.9614,0.9683,0.9753,0.9824,0.9896,0.997,1.0043,1.0119,1.0194,1.0271,1.0349,1.0429,1.0509,1.059,1.0672,1.0756,1.084,1.0925,1.1012,1.1099,1.1189,1.1278,1.137,1.1461,1.1556,1.1649,1.1746,1.1842,1.1941,1.2039,1.214,1.2242,1.2344,1.2449,1.2553,1.266,1.2768,1.2877,1.2988,1.31,1.3212,1.3327,1.3443,1.3559,1.3678,1.3798,1.3919,1.4041,1.4165,1.429,1.4418,1.4545,1.4675,1.4807,1.4938,1.5073,1.5208,1.5346,1.5485,1.5625,1.5766,1.5601,1.2354,0.4486,0.5128,0.3507,0.1844,0.1841,0.1837,0.1835,0.1832,0.1828,0.1826,0.1823,0.182,0.1817,0.1814,0.1812,0.181,0.1807,0.1804,0.1802,0.1799,0.1797,0.1795,0.1793,0.1791,0.1789,0.1787,0.1784,0.1782,0.178,0.1778,0.1777,0.1775,0.1773,0.1771,0.177,0.1768,0.1766,0.1764,0.1763,0.1762,0.176,0.1759,0.1758,0.1756,0.1754,0.1753,0.1752,0.1751,0.1749,0.1748,0.1746,0.1745,0.1745,0.1743,0.1742,0.174,0.174,0.1739,0.1737,0.1736,0.1735,0.1735,0.1734,0.1732,0.1731,0.1731,0.173,0.1729,0.1728,0.1728,0.1727,0.1726,0.1725,0.1725,0.1724,0.1722,0.1722,0.1721,0.172,0.172,0.1719,0.1719,0.1718,0.1717,0.1716,0.1716,0.1716,0.1715,0.1714,0.1714,0.1713,0.1713,0.1712,0.1711,0.1711,0.1711,0.1711,0.171,0.1709,0.1709,0.1709,0.1708,0.1708,0.1708,0.1707,0.1706,0.1706,0.1706,0.1706,0.1705,0.1704,0.1704,0.1704,0.1703,0.1703,0.1703,0.1702,0.1702,0.1702,0.1702,0.1701,0.1701,0.1701,0.17,0.17};
    double dead_cryo1[277]={1.6192,1.6194,1.6197,1.6203,6.3509,2.5841,1.9874,1.3905,1.3916,1.3929,1.3943,1.3957,1.3975,1.3992,1.4013,1.4033,1.4055,2.0138,2.6243,4.3032,2.6344,2.0294,1.4221,1.4251,1.4284,1.432,1.4357,1.4394,1.4433,1.4475,1.4516,1.4561,1.4605,1.4652,1.4702,1.475,2.117,2.7636,4.1287,2.3159,2.1483,1.5079,1.5139,1.5199,1.5261,1.5326,1.5393,1.546,1.553,1.5599,1.5673,1.5747,1.5821,1.5899,1.5977,2.297,2.3084,3.9766,3.3404,2.3449,2.345,1.6573,1.6664,1.6757,1.6853,1.6949,1.7047,1.7148,1.7249,1.7352,1.7457,1.7564,1.7673,1.7784,2.2539,2.576,3.3648,3.5384,3.0253,2.6439,1.8606,1.8733,1.8859,1.8989,1.9119,1.9251,1.9387,1.9521,1.966,1.98,1.9943,2.0088,2.0235,2.0383,2.937,3.214,3.4082,3.6655,3.026,3.0489,2.148,2.1643,2.1812,2.198,3.6572,3.7318,3.5266,3.3742,3.534,3.7906,3.7431,4.3293,4.4603,2.9506,3.8466,3.5582,3.078,2.4882,2.2279,1.7277,1.2731,1.0272,0.7105,0.3933,0.2912,0.2902,0.2893,0.2883,0.2875,0.2866,0.2857,0.2849,0.284,0.2834,2.6585,2.6785,2.6989,2.7197,2.7405,2.7614,2.7828,2.8045,2.8265,2.8488,2.8712,2.8939,2.9114,2.7992,2.9324,3.4047,0.414,0.252,0.1168,0.9172,0.9156,0.914,0.9123,0.9108,0.9091,0.9076,0.9062,0.9048,0.9033,0.9018,0.9006,0.8993,0.8979,0.8966,0.8955,0.8942,0.893,0.8918,0.8907,0.8896,0.8886,0.8874,0.8865,0.8853,0.8843,0.8833,0.8823,0.9288,0.9279,0.927,0.926,0.9251,0.9242,0.9233,0.9224,0.9217,0.9208,0.9199,0.9192,0.9184,0.9176,0.9169,0.9162,0.9154,0.9148,0.9141,0.9135,0.9128,0.9121,0.9115,0.9109,0.9103,0.9097,0.9091,0.9086,0.9079,0.9074,0.9069,0.9064,0.9059,0.9053,0.9048,0.9043,0.9038,0.9034,0.903,0.9025,0.9022,0.9017,0.9013,0.9009,0.9004,0.9002,0.8996,0.8994,0.899,0.8986,0.8982,0.8979,0.8976,0.8971,0.897,0.8966,0.8963,0.896,0.8957,0.8954,0.8951,0.8948,0.8945,0.8942,0.8941,0.8937,0.8934,0.8932,0.8929,0.8927,0.8924,0.8922,0.892,0.8917,0.8915,0.8914,0.8912,0.891,0.8907,0.8905,0.8903,0.8902,0.89,0.8898,0.8897,0.8894,0.8893,0.889,0.8889,0.8888,0.8886,0.8886,0.8883,0.8882,0.888,0.8879};
    double dead_cryo2[277]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.2895,0.9101,1.5272,1.8249,1.8279,1.8312,1.8348,1.8385,1.8424,1.8466,1.851,1.8555,1.8604,1.5309,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0763,0.6379,0.8382,1.0003,1.2294,1.5831,2.0439,2.3796,2.6456,2.9827,3.3447,1.141,1.138,1.1349,1.132,1.1291,1.1264,1.1236,1.1208,1.1183,1.1156,1.1132,1.1108,1.1084,1.106,1.1037,1.1014,4.514,4.4934,4.5226,3.5091,3.5412,3.5737,3.6066,3.6398,3.6734,3.7074,3.7418,3.7764,3.8115,4.423,5.2744,6.1316,6.6266,6.6847,6.7435,6.803,9.6759,9.0086,8.1971,7.7761,7.5583,7.7543,8.5776,9.3644,9.4149,9.466,9.4281,8.6493,7.8651,7.5418,7.2376,7.3746,7.4009,7.3938,7.3869,7.3801,7.3735,7.3671,7.3608,7.3546,7.3485,7.3426,7.3367,7.331,7.3254,7.3199,7.3146,7.3093,7.3042,7.2991,7.2943,7.2894,7.2846,7.28,7.2754,7.271,7.2666,7.2624,7.2582,7.2541,7.2501,7.2461,7.2423,7.2386,7.2348,7.2312,7.2276,7.2242,7.2207,7.2174,7.2141,7.211,7.2078,7.2047,7.2017,7.1987,7.1958,7.1931,7.1902,7.1875,7.1848,7.1822,7.1796,7.1772,7.1747,7.1723,7.1699,7.1676,7.1653,7.1632,7.161,7.1588,7.1567,7.1547,7.1527,7.1507,7.1487,7.1469,7.145,7.1433,7.1415,7.1397,7.138,7.1364,7.1347,7.1331,7.1315,7.13,7.1284,7.1269,7.1255,7.1241,7.1227,7.1213,7.12,7.1187,7.1175,7.1162,7.1149,7.1137,7.1126,7.1114,7.1102};  double dead_girder[277]={11.9514,11.9525,11.9549,11.9584,11.9632,11.9693,11.9764,11.9848,11.9943,12.0051,12.0171,12.0303,12.0446,12.0602,12.077,12.095,12.1142,12.1346,12.1563,12.1791,12.2032,12.2285,12.255,12.2827,12.3116,12.3418,12.3732,12.4059,12.4398,12.475,12.5113,12.549,12.5879,12.6281,12.6695,12.7122,12.7561,12.8014,12.8479,12.8957,12.9448,12.9952,13.0469,13.0998,13.1541,13.2098,13.2667,13.325,13.3845,13.4455,13.5078,13.5714,13.6364,13.7028,13.7705,13.8396,13.91,13.9818,14.0552,14.1298,14.206,14.2835,12.3788,7.5701,7.4957,6.6037,3.8717,0,0,0,0,3.5585,7.2483,7.505,8.4513,10.9561,15.539,15.7235,15.8265,15.9311,16.0372,16.1449,16.2543,16.3653,16.4779,16.5921,16.7081,16.8256,16.9449,17.0659,17.1886,17.313,17.4392,17.567,17.6966,17.8281,17.9612,18.0962,18.233,18.3716,18.5121,18.6543,18.7985,18.9446,19.0925,19.2424,19.3942,19.5479,19.7036,19.8612,20.0208,20.1824,20.3461,20.5118,20.6795,20.8493,21.0212,16.6712,11.697,11.2949,10.114,9.906,8.5687,3.7101,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(int i=0;i<277;i++)
    {
	m_dead_eta[i]= dead_eta[i];
	m_dead_x01[i]= dead_x01[i];
	m_dead_x02[i]= dead_x02[i];
	m_dead_x0tot[i]= dead_x0tot[i];
	m_dead_cryo0[i]= dead_cryo0[i];
	m_dead_cryo1[i]= dead_cryo1[i];
	m_dead_cryo2[i]= dead_cryo2[i];
	m_dead_girder[i]= dead_girder[i];
    }
    m_etaGranularity = 0.01;
    //---- End of material map arrays initialization -----//

    return StatusCode::SUCCESS;
}

StatusCode
MuidCaloEnergyParam::finalize()
{
    ATH_MSG_DEBUG( "Finalizing" );  
    
    return StatusCode::SUCCESS;
}

CaloEnergy*
MuidCaloEnergyParam::meanParametrizedEnergy(double trackMomentum,
					    double eta,
					    double phi) const
{
    CaloEnergy* caloEnergy			= 0;
    double meanEloss				= meanEnergyLoss(eta,trackMomentum);
    std::pair<double,double> parametrizedErrors	= meanEnergyLossError(eta,trackMomentum);
    
    ATH_MSG_DEBUG( " Mean_deposited_energy = " << meanEloss/Units::GeV );
    
    // cosmics: energy sign flip in upper hemisphere
    if (m_cosmics && phi > 0.)
    {
	caloEnergy	= new CaloEnergy(-meanEloss,
					 0.5*(parametrizedErrors.first +
					      parametrizedErrors.second),
					 parametrizedErrors.second,
					 parametrizedErrors.first,
					 CaloEnergy::Parametrized);
    }
    else
    {
	caloEnergy	=  new CaloEnergy(meanEloss,
					  0.5*(parametrizedErrors.first +
					       parametrizedErrors.second),
					  parametrizedErrors.first,
					  parametrizedErrors.second,
					  CaloEnergy::Parametrized);
    }
    // set parametrized energy loss in em
    double emLoss	= caloEnergy->deltaE()*emMopFraction(eta);
    caloEnergy->set_fsrCandidateEnergy(emLoss);
    return caloEnergy;
}

CaloEnergy*
MuidCaloEnergyParam::mopParametrizedEnergy(double trackMomentum,
					   double eta,
					   double phi) const
{
    CaloEnergy* caloEnergy			= 0;
    
    // mop and mean energy deposition
    double mopEloss				= mopEnergyLoss(eta,trackMomentum);
    double meanEloss				= meanEnergyLoss(eta,trackMomentum);

    ATH_MSG_DEBUG( " Most probable (mean) deposited energy " << mopEloss/Units::GeV
		   << " (" << meanEloss/Units::GeV << ")" );

    // mop energy deposition uncertainty (Gaussian fit around mop value +- 2 sigma)
    double mopError				= mopEnergyLossError(eta,trackMomentum);

    // scale low transverse momentum to allow for fluctuations
    double pt					= trackMomentum*sin(2.*atan(exp(-eta)));
    double scale				= 1.;
    if (pt < 15.*Units::GeV) scale = (2.5 - pt*(1.5/(15.*Units::GeV)));
						  
    // asymmetric error to allow for Landau
    double alpha				= 0.2;
    double beta					= 300.*Units::MeV;
    double mopErrorPlus				= mopError + alpha*(meanEloss - mopEloss) + beta;
        
    // cosmics: energy sign flip in upper hemisphere
    if (m_cosmics && phi > 0.)
    {
	caloEnergy	= new CaloEnergy(-mopEloss,
					 scale*mopError,
					 scale*mopErrorPlus,
					 scale*mopError,
					 CaloEnergy::Parametrized);
    }
    else
    {
	caloEnergy	= new CaloEnergy(mopEloss,
					 scale*mopError,
					 scale*mopError,
					 scale*mopErrorPlus,
					 CaloEnergy::Parametrized);
    }
    // set parametrized energy loss in em
    double emLoss	= caloEnergy->deltaE()*emMopFraction(eta);
    caloEnergy->set_fsrCandidateEnergy(emLoss);
    return caloEnergy;
}

CaloEnergy*
MuidCaloEnergyParam::mopPeakEnergy(double trackMomentum,
				   double eta,
				   double phi) const
{
    CaloEnergy* caloEnergy			= 0;
    
    // mop and mean energy deposition
    double mopEloss				= mopEnergyLoss(eta,trackMomentum);
    double meanEloss				= meanEnergyLoss(eta,trackMomentum);

    ATH_MSG_DEBUG( " Most probable (mean) deposited energy " << mopEloss/Units::GeV
		   << " (" << meanEloss/Units::GeV << ")" );

    // mop energy deposition uncertainty (Gaussian fit around mop value +- 2 sigma)
    double mopError				= mopEnergyLossError(eta,trackMomentum);


    // scale low transverse momentum to allow for fluctuations
    double pt					= trackMomentum*sin(2.*atan(exp(-eta)));
    double scale				= 1.;
    if (pt < 15.*Units::GeV) scale = (2.5 - pt*(1.5/(15.*Units::GeV)));
									  
    // asymmetric error to allow for Landau
    double alpha				= 0.1;
    double beta					= 150.*Units::MeV;
    double mopErrorPlus				= mopError + alpha*(meanEloss - mopEloss) + beta;
        
    // cosmics: energy sign flip in upper hemisphere
    if (m_cosmics && phi > 0.)
    {
	caloEnergy	= new CaloEnergy(-mopEloss,
					 scale*mopError,
					 scale*mopErrorPlus,
					 scale*mopError,
					 CaloEnergy::MOP);
    }
    else
    {
	caloEnergy	= new CaloEnergy(mopEloss,
					 scale*mopError,
					 scale*mopError,
					 scale*mopErrorPlus,
					 CaloEnergy::MOP);
    }
    // set parametrized energy loss in em
    double emLoss	= caloEnergy->deltaE()*emMopFraction(eta);
    caloEnergy->set_fsrCandidateEnergy(emLoss);
    return caloEnergy;
}

CaloEnergy*
MuidCaloEnergyParam::mopDepositedEnergy(double trackMomentum,
					double eta,
					double phi) const
{
    CaloEnergy* caloEnergy			= 0;
    
    std::pair<double, double> eta_phi;
    eta_phi.first = eta;
    eta_phi.second = phi;
  
    double mop_eloss	= mopEnergyLoss(eta_phi.first,trackMomentum);

    
    //double mop_eloss	= symmetricMopEnergyLoss(eta_phi.first,trackMomentum);
    double mop_eloss_error= mopEnergyLossError(eta_phi.first,trackMomentum);
    
    ATH_MSG_DEBUG( " Most probable deposited energy... " << mop_eloss/Units::GeV );

    // cosmics: energy sign flip in upper hemisphere
    if (m_cosmics && phi > 0.)
    {
	caloEnergy	= new CaloEnergy(-mop_eloss,
					 mop_eloss_error,
					 mop_eloss_error,
					 mop_eloss_error,
					 CaloEnergy::Parametrized);
    }
    else
    {
	caloEnergy	= new CaloEnergy(mop_eloss,
					 mop_eloss_error,
					 mop_eloss_error,
					 mop_eloss_error,
					 CaloEnergy::Parametrized);
    }
    // set parametrized energy loss in em
    double emLoss	= caloEnergy->deltaE()*emMopFraction(eta);
    caloEnergy->set_fsrCandidateEnergy(emLoss);
    return caloEnergy;
}

void
MuidCaloEnergyParam::etaFixedBin(double eta) const
{
    eta			= fabs(eta);
    double offsetEta1	= eta - m_etaOffset;
    double offsetEta2	= eta + m_etaOffset;
    m_etaWeight2	= 0.5;
    if (offsetEta2 < m_binWidth)	// first (i.e. central) bin #0 - no interpolation
    {
	m_etaBin1	= 0;
	m_etaBin2	= 0;
    }
    else if (offsetEta1 >= 2.50)	// last bin #25 (eta > 2.5) - no interpolation
    {
	m_etaBin1	= 25;
	m_etaBin2	= 25;
    }
    else if (m_smoothingFraction > 0.)
    {
	double eta1	= floor(m_inverseWidth*offsetEta1);
	m_etaBin1	= static_cast<int> (eta1);
	double eta2	= floor(m_inverseWidth*offsetEta2);
	m_etaBin2	= static_cast<int> (eta2);
	if (m_etaBin2 > m_etaBin1) m_etaWeight2 = (m_inverseWidth*offsetEta2 - eta2)/m_smoothingFraction;
    }
    else
    {
	double eta1	= floor(m_inverseWidth*eta);
	m_etaBin1	= static_cast<int> (eta1);
	m_etaBin2	= m_etaBin1;
    }

    m_etaWeight1	= 1. - m_etaWeight2;
}

void
MuidCaloEnergyParam::etaVariableBin(double eta) const
{
    eta			= fabs(eta);
    if (eta < 1.05)			// barrel: contains the variable bin region
    {
	if (eta < 0.75)			// identical to fixed binning
	{
	    etaFixedBin(eta);
	}
	else if (eta < 0.80)		// wide bin#7
	{
	    etaFixedBin(0.75);
	}
	else if (eta < 1.00)		// similar to fixed binning (with half bin offset)
	{
	    etaFixedBin(eta - 0.5*m_binWidth);
	}
	else				// wide bin#9
	{
	    etaFixedBin(0.95);
	}
    }
    else				// endcap: fixed binning (with full bin offset)
    {
	etaFixedBin(eta - m_binWidth);
    }
}

double
MuidCaloEnergyParam::meanEnergyLoss(double eta, double momentum) const
{
    // Mean Energy Loss parametrization with asymmetric gaussian fit
    //  tabulated coefficients for function [0]+[1]*Log(0.0067*p*p)+[2]*p with GeV units
    etaVariableBin(eta);
    double p0			= m_etaWeight1*m_meanEnergyLossP0[m_etaBin1] +
				  m_etaWeight2*m_meanEnergyLossP0[m_etaBin2];
    double p1			= m_etaWeight1*m_meanEnergyLossP1[m_etaBin1] +
				  m_etaWeight2*m_meanEnergyLossP1[m_etaBin2];
    double p2			= m_etaWeight1*m_meanEnergyLossP2[m_etaBin1] +
				  m_etaWeight2*m_meanEnergyLossP2[m_etaBin2];
    double parametrisedDeposit	= p0*Units::GeV +
                                  p1*Units::GeV*log(0.0067*momentum*momentum/Units::GeV/Units::GeV) +
				  p2*momentum;
 
    //  additional offset from high-statistics Z->mumu MC (measured by Peter K 30/11/2011, 03/2012)
    //  NOTE:  constant bin width used here
    etaFixedBin(eta);
    // mc11 tune
//     double fixFromPeter[26]	= { -0.203781 , -0.146775 , -0.0952301 , -0.186141 , -0.180687 ,
// 				    -0.225095 , -0.195912 , -0.221158  , -0.198556 , -0.271551 ,
// 				    -0.200737 , -0.151854 , -0.0799826 , -0.237688 , -0.255126 ,
// 				    -0.313466 , -0.35815  , -0.334661  , -0.458948 , -0.469967 ,
// 				    -0.516059 , -0.522978 , -0.53896   , -0.550683 , -0.51722  ,
// 				    -0.543858 };
    // mc12 tune
    double fixFromPeter[26]	= { -0.159195 , -0.039180 ,  0.071644 , -0.118753 , -0.144227 ,
				    -0.182778 , -0.158818 , -0.188853 , -0.180997 , -0.269571 ,
				    -0.227767 , -0.191740 , -0.121575 , -0.304938 , -0.296636 ,
				    -0.312171 , -0.352982 , -0.303020 , -0.357250 , -0.396249 ,
				    -0.433535 , -0.417214 , -0.424093 , -0.415557 , -0.325372 ,
				    -0.553009 };
    double fix			=  m_etaWeight1*fixFromPeter[m_etaBin1] +
				   m_etaWeight2*fixFromPeter[m_etaBin2];
    parametrisedDeposit		+= fix*Units::GeV;
    
    return parametrisedDeposit;
}

std::pair<double,double>
MuidCaloEnergyParam::meanEnergyLossError(double eta, double momentum) const
{
    etaVariableBin(eta);
    double left_p0	= m_etaWeight1*m_meanEnergyLossErrorLeftP0[m_etaBin1] +
			  m_etaWeight2*m_meanEnergyLossErrorLeftP0[m_etaBin2];
    double left_p1	= m_etaWeight1*m_meanEnergyLossErrorLeftP1[m_etaBin1] +
			  m_etaWeight2*m_meanEnergyLossErrorLeftP1[m_etaBin2];
    double right_p0	= m_etaWeight1*m_meanEnergyLossErrorRightP0[m_etaBin1] +
			  m_etaWeight2*m_meanEnergyLossErrorRightP0[m_etaBin2];
    double right_p1	= m_etaWeight1*m_meanEnergyLossErrorRightP1[m_etaBin1] +
			  m_etaWeight2*m_meanEnergyLossErrorRightP1[m_etaBin2];
    double sigma_left	= left_p0*Units::GeV + left_p1*momentum;
    double sigma_right	= right_p0*Units::GeV + right_p1*momentum;

    return std::make_pair(sigma_left,sigma_right);
}

double
MuidCaloEnergyParam::mopEnergyLoss(double eta, double momentum) const
{
    // Mop Energy Loss parametrization with asymmetric gaussian fit in the mop region
    //  tabulated coefficients for function [0]+[1]*Log(0.0067*p*p)+[2]*p with GeV units
    etaVariableBin(eta);
    double p0			= m_etaWeight1*m_mopEnergyLossP0[m_etaBin1] +
				  m_etaWeight2*m_mopEnergyLossP0[m_etaBin2];
    double p1			= m_etaWeight1*m_mopEnergyLossP1[m_etaBin1] +
				  m_etaWeight2*m_mopEnergyLossP1[m_etaBin2] ;
    double p2			= m_etaWeight1*m_mopEnergyLossP2[m_etaBin1] +
				  m_etaWeight2*m_mopEnergyLossP2[m_etaBin2];
    double parametrisedDeposit	= p0*Units::GeV +
                                  p1*Units::GeV*log(0.0067*momentum*momentum/Units::GeV/Units::GeV) +
				  p2*momentum;
 
    //  additional offset from high-statistics Z->mumu MC (measured by Peter K 30/11/2011)
    //  NOTE:  constant bin width used here
    etaFixedBin(eta);
    // mc11 tune
//     double fixFromPeter[26]	= { -0.203781 , -0.146775 , -0.0952301 , -0.186141 , -0.180687 ,
// 				    -0.225095 , -0.195912 , -0.221158  , -0.198556 , -0.271551 ,
// 				    -0.200737 , -0.151854 , -0.0799826 , -0.237688 , -0.255126 ,
// 				    -0.313466 , -0.35815  , -0.334661  , -0.458948 , -0.469967 ,
// 				    -0.516059 , -0.522978 , -0.53896   , -0.550683 , -0.51722  ,
// 				    -0.543858 };
    // mc12 tune
    double fixFromPeter[26]	= { -0.159195 , -0.039180 ,  0.071644 , -0.118753 , -0.144227 ,
				    -0.182778 , -0.158818 , -0.188853 , -0.180997 , -0.269571 ,
				    -0.227767 , -0.191740 , -0.121575 , -0.304938 , -0.296636 ,
				    -0.312171 , -0.352982 , -0.303020 , -0.357250 , -0.396249 ,
				    -0.433535 , -0.417214 , -0.424093 , -0.415557 , -0.325372 ,
				    -0.553009 };

    double fix			=  m_etaWeight1*fixFromPeter[m_etaBin1] +
				   m_etaWeight2*fixFromPeter[m_etaBin2];
    parametrisedDeposit		+= fix*Units::GeV;
    
    return parametrisedDeposit;
}

double
MuidCaloEnergyParam::mopEnergyLossError(double eta, double momentum) const
{
    eta			= fabs(eta);
    etaVariableBin(eta);
    double sigma_p0	= m_etaWeight1*m_mopEnergyLossErrorP0[m_etaBin1] +
			  m_etaWeight2*m_mopEnergyLossErrorP0[m_etaBin2];
    double sigma_p1	= m_etaWeight1*m_mopEnergyLossErrorP1[m_etaBin1] +
			  m_etaWeight2*m_mopEnergyLossErrorP1[m_etaBin2];
    double sigma	= sigma_p0*Units::GeV + sigma_p1*momentum;

    // fix for mc11 as underestimate in barrel, overestimate in endcap
    if (eta < 1.7)
    {
	sigma	*= 1.1;
    }
    else
    {
	sigma	*= 0.9;
    }
    
    return sigma;
}

double
MuidCaloEnergyParam::symmetricMopEnergyLoss(double eta, double momentum) const
{
    // Mop Energy Loss parametrization with symmetric gaussian fit in the mop region
    //  new coefficients with symmetric fits
    etaVariableBin(eta);
    double p0			= m_etaWeight1*m_mopSymmetricEnergyLossP0[m_etaBin1] +
				  m_etaWeight2*m_mopSymmetricEnergyLossP0[m_etaBin2];
    double p1			= m_etaWeight1*m_mopSymmetricEnergyLossP1[m_etaBin1] +
				  m_etaWeight2*m_mopSymmetricEnergyLossP1[m_etaBin2];
    double p2			= m_etaWeight1*m_mopSymmetricEnergyLossP2[m_etaBin1] +
				  m_etaWeight2*m_mopSymmetricEnergyLossP2[m_etaBin2];
    double parametrisedDeposit	= p0*Units::GeV +
                                  p1*Units::GeV*log(0.0067*momentum*momentum/Units::GeV/Units::GeV) +
				  p2*momentum;
    return parametrisedDeposit;
}

double
MuidCaloEnergyParam::x0mapInertMaterial(double eta) const
{
    int index = static_cast<int> (floor(fabs(eta)/m_etaGranularity));
    if(index>=277)index=276;
 
    double x01      = m_dead_x01[index];
    //double x02      = m_dead_x02[index];
    double x0tot    = m_dead_x0tot[index];
    double x0cryo1  = m_dead_cryo1[index];
    double x0cryo2  = m_dead_cryo2[index];
    double x0girder = m_dead_girder[index];
    double sum      = x01+x0cryo1+x0cryo2+x0girder;
    return sum/x0tot;

}

double
MuidCaloEnergyParam::x0mapEmMaterial(double eta) const
{
    int index = static_cast<int> (floor(fabs(eta)/m_etaGranularity));
    if(index>=277)index=276;
 
    double x01      = m_dead_x01[index];
    double x02      = m_dead_x02[index];
    double x0tot    = m_dead_x0tot[index];
    double x0cryo1  = m_dead_cryo1[index];
    //double x0cryo2  = m_dead_cryo2[index];
    //double x0girder = m_dead_girder[index];
    double sum      = x02-x01-x0cryo1;
    return sum/x0tot;

}

double
MuidCaloEnergyParam::x0mapHecMaterial(double eta) const
{
    int index = static_cast<int> (floor(fabs(eta)/m_etaGranularity));
    if(index>=277)index=276;
  
    //double x01      = m_dead_x01[index];
    double x02      = m_dead_x02[index];
    double x0tot    = m_dead_x0tot[index];
    //double x0cryo1  = m_dead_cryo1[index];
    double x0cryo2  = m_dead_cryo2[index];
    double x0girder = m_dead_girder[index];
    double sum = x0tot - x02 - x0cryo2 - x0girder;
    return sum/x0tot;

}

double
MuidCaloEnergyParam::caloCompartmentDepth(int icalo,int icomp) const
{
    /* ------------------
       icalo calorimeter
       0     Tile
       1	   LArHEC
       2	   LArEM
       ------------------ */
    if(icalo == 0)
	return caloCompartmentDepthTile(icomp);
    else if(icalo == 1)
	return caloCompartmentDepthLArHEC(icomp);
    else if(icalo == 2)
	return caloCompartmentDepthLArEM(icomp);
    else
	ATH_MSG_WARNING( " Incorrect selection of calorimeter... returning 0." );
    return 0.;

}

double
MuidCaloEnergyParam::caloCompartmentDepthTile(int icomp) const
{
    // depth in absorption lengths (from Tile TDR p 22) eta = 0
    const double comp[4] = {1.5,4.2,1.9,0.}; // assign 0. to ITC for the moment
    double total = 0.;
    for(int i=0;i<4;i++)
	total += comp[i];
    if(icomp >= 0 && icomp <4)
    {
	return comp[icomp]/total;
    }
    return 0.;
}

double
MuidCaloEnergyParam::caloCompartmentDepthLArHEC(int icomp) const
{
    // depth in absorption lengths (from Liquid Argon TDR Table 1-3 p 11)
    const double comp[4] = {1.4,2.9,5.7/2.,5.7/2.}; // divide 5.7 rad lengths equally to 3rd and 4th sampling
    double total = 0.;
    for(int i=0;i<4;i++)
	total+=comp[i];
    const double inv_total = 1. /total;
    if(icomp >= 0 && icomp <4)
    {
	return comp[icomp]*inv_total;
    }
    return 0.;
}

double
MuidCaloEnergyParam::caloCompartmentDepthLArEM(int icomp) const
{
    // depth in radiation lengths (from Liquid Argon TDR p 5) eta = 0
    const double comp[4] = {0.2,4.3,16.,2.}; 
    double total = 0.;
    for(int i=0;i<4;i++)
	total+=comp[i];
    if(icomp >= 0 && icomp <4)
    {
	return comp[icomp]/total;
    }
    return 0.;
}

double
MuidCaloEnergyParam::emMopFraction(double eta) const
{
    int ieta			=  static_cast<int> (fabs(eta)/0.10);
    if (ieta > 25) ieta 	=  25;
    
    return m_emMopFraction[ieta];
}
 
double
MuidCaloEnergyParam::em1MopFraction(double eta) const
{
    // FIXME!!
    double em1Fraction	= 0.1*emMopFraction(eta);    
    return em1Fraction;
}
    
}	// end of namespace


