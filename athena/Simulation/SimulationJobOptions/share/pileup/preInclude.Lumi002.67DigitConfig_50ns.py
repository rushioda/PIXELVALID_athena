from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 50  
digitizationFlags.numberOfCavern = 1
digitizationFlags.numberOfCollisions = 1.2
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 1.2
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0012
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 1.2
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.initialBunchCrossing = -16
digitizationFlags.finalBunchCrossing = 16
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003

