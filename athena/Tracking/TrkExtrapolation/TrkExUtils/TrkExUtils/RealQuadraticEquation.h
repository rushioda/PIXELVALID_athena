/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RealQuadraticEquation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_REALQUADRATICEQUATION_H
#define TRKEXUTILS_REALQUADRATICEQUATION_H

#include <utility>
#include <cmath>

namespace Trk {
  /** @enum RQESolutionType
    */ 
      
  enum RQESolutionType { none=0, one=1, two=2 };
  
  /** @struct RealQuadradicEquation
      Mathematic struct for solving real quadratic equations
   
     <b>Mathematical motivation</b>:<br>
     The equation is given by:<br>
     @f$ \alpha x^{2} + \beta x + \gamma = 0  @f$
     and has therefore the analytical solution:<br>
     @f$ x_{1, 2} = - \frac{\beta \pm \sqrt{\beta^{2}-4\alpha\gamma}}{2\alpha}@f$ <br>
    <br>
     - case @f$ \beta > 0 @f$:<br>
     @f$ x_{1} = - \frac{\beta + \sqrt{\beta^{2}-4\alpha\gamma}}{2\alpha}  := \frac{q}{\alpha}@f$, <br>
     so that @f$ q= -\frac{1}{2}(\beta+sqrt{\beta^{2}-4\alpha\gamma})@f$.
     @f$ x_{2} @f$ can now be written as: @f$ x_{2} = \frac{\gamma}{q} = -\frac{2\gamma}{\beta+sqrt{\beta^{2}-4\alpha\gamma}}@f$, since: <br>
     @f$ -\frac{2\gamma}{\beta+sqrt{\beta^{2}-4\alpha\gamma}} = -\frac{2\gamma}{\beta}\frac{1}{1+\sqrt{1-4\alpha\gamma/\beta^{2}}}@f$, and <br>
     @f$ x_{2}\frac{1}{1-\sqrt{1-4\alpha\gamma/\beta^{2}}} = -\frac{2\gamma}{\beta}\frac{1}{1-1+4\alpha\gamma/\beta^{2}}=-\frac{\beta}{2\alpha}.@f$<br>
     Hence,@f$ -\frac{\beta(1-\sqrt{1-4\alpha\gamma/\beta^{2}}}{2\alpha} = - \frac{\beta - \sqrt{\beta^{2}-4\alpha\gamma}}{2\alpha} @f$.<br>
     - case @f$ \beta > 0 @f$ is similar.
   
    @author Andreas.Salzburger@cern.ch   
   */
 
  struct RealQuadraticEquation {

    double first;
    double second;
    RQESolutionType solutions;

    RealQuadraticEquation( double alpha, double beta, double gamma):first{},second{} {
      double discriminant = beta*beta - 4*alpha*gamma;
      if (discriminant<0) solutions = none;
      else {
        solutions = (discriminant==0) ? one : two;
        double q = -0.5*(beta + (beta>0 ? sqrt(discriminant) : -sqrt(discriminant)));
        first = q/alpha;
        second = gamma/q;
      }
    }

  };

} // end of namespace

#endif  //  TRKEXUTILS_REALQUADRATICEQUATION_H

