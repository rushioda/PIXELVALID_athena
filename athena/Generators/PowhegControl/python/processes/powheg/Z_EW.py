# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon import Logging
from ..external import ExternalPHOTOS
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Z_EW(PowhegV2):
    """! Default Powheg configuration for single Z-boson production with electroweak corrections.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(Z_EW, self).__init__(base_directory, "Z_ew-BMNNPV", **kwargs)

        # Add algorithms to the sequence
        self.add_algorithm(ExternalPHOTOS(os.path.split(self.executable)[0], "main-PHOTOS-lhef"))

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["z > e+ e-", "z > mu+ mu-", "z > tau+ tau-"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphaem_z")
        self.add_keyword("alphaem")
        self.add_keyword("bmass_lhe")
        self.add_keyword("Bmass")
        self.add_keyword("bornktmin")
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact")
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btildeborn")
        self.add_keyword("btildecoll")
        self.add_keyword("btildereal")
        self.add_keyword("btildevirt")
        self.add_keyword("btlscalect")
        self.add_keyword("btlscalereal")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("check_bad_st2")
        self.add_keyword("clobberlhe")
        self.add_keyword("cmass_lhe")
        self.add_keyword("Cmass")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("Dmass")
        self.add_keyword("doublefsr")
        self.add_keyword("Elmass")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi", 5)
        self.add_keyword("foldphi", 2)
        self.add_keyword("foldy", 5)
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("gmu")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("Hmass")
        self.add_keyword("Hwidth")
        self.add_keyword("icsimax")
        self.add_keyword("iftopinloop")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 6)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("kt2minqed")
        self.add_keyword("lepaslight")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("mass_low", 10.0)
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("Mumass")
        self.add_keyword("ncall1", 5000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 30000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("new_damp")
        self.add_keyword("no_ew")
        self.add_keyword("no_strong")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 40000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("photoninduced")
        self.add_keyword("phsp_Zm")
        self.add_keyword("phsp_Zw")
        self.add_keyword("powhegv2opt")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("quadrupleprec")
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("resc_em_alpha")
        self.add_keyword("runningscale")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("scheme")
        self.add_keyword("skipextratests")
        self.add_keyword("smartsig")
        self.add_keyword("Smass")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("Taumass")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("Tmass")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("Umass")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("vdecaymode", self.allowed_decay_modes[0], name="decay_mode")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("Wmass")
        self.add_keyword("Wwidth")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 4)
        self.add_keyword("Zmass")
        self.add_keyword("Zwidth")

    def validate_decays(self):
        """! Validate vdecaymode keyword and check that 'no_ew' is compatible with 'PHOTOS_enabled'."""
        # If PHOTOS is disabled but EW effects are on, any subsequent PHOTOS process will have to run in vetoed mode which is not supported
        if not self.externals["PHOTOS"].parameters_by_keyword("PHOTOS_enabled")[0].value and self.parameters_by_keyword("no_ew")[0].value == 0:
            logger.error("Attempting to run with native PHOTOS disabled but with EW corrections on.")
            logger.error("This would require any later PHOTOS generation to run in vetoed mode.")
            logger.error("Please change 'PHOTOS_enabled' and/or 'no_ew' in your jobOptions.")
            raise ValueError("Incompatible options. Please change 'PHOTOS_enabled' and/or 'no_ew' in your jobOptions.")
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        __decay_mode_lookup = {"e+ e-": 11, "mu+ mu-": 13, "tau+ tau-": 15}
        self.parameters_by_keyword("vdecaymode")[0].value = __decay_mode_lookup[self.decay_mode.split("> ")[1]]
