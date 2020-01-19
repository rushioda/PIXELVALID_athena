# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,re
from envutil import *

POOL_HOME = 'POOL_HOME'

def setup_environment():
    # get POOL_HOME
    pool_home = os.environ.get(POOL_HOME)
    if not pool_home:
        # try to extract from PATH
        poolRE = r'.*/POOL/.*/bin'
        pool_path = find_path_env('PATH', poolRE)
        if not pool_path:
            raise EnvironmentError('Could not determine POOL_HOME')

        pool_home = os.path.dirname(pool_path[0])
        os.environ[POOL_HOME] = pool_home

    pool_lib = os.path.join( pool_home, 'lib' )
    pool_bin = os.path.join( pool_home, 'bin' )

    append_path_env_if('PYTHONPATH',pool_bin)


setup_environment()

from PyFCAction import *


