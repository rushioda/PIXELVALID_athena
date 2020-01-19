# Modify the hit wrapping action to wrap on 75 ns

from G4AtlasApps.SimFlags import simFlags
# new MT actions. Note that this will only work with one thread.
simFlags.OptionalUserActionList.addAction('G4UA::HitWrapperTool',['Event'])
simFlags.UserActionConfig.addConfig('G4UA::HitWrapperTool','WrapTime',75)

