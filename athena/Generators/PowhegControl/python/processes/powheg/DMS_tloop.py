# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class DMS_tloop(PowhegV2):
    """! Default Powheg configuration for dark matter plus monojet production (through a top loop).

    Create a configurable object with all applicable Powheg options.
    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory  path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(DMS_tloop, self).__init__(base_directory, "DMS_tloop", **kwargs)

        ## List of allowed decay modes
        self.allowed_decay_modes = ["scalar EFT", "pseudo-scalar EFT", "scalar full theory", "pseudo-scalar full theory"]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("bornktmin", 100.0)
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact")
        self.add_keyword("bornzerodamp")
        self.add_keyword("bottomthr")
        self.add_keyword("bottomthrpdf")
        self.add_keyword("btildeborn")
        self.add_keyword("btildecoll")
        self.add_keyword("btildereal")
        self.add_keyword("btildevirt")
        self.add_keyword("charmthr")
        self.add_keyword("charmthrpdf")
        self.add_keyword("check_bad_st2")
        self.add_keyword("ckkwscalup")
        self.add_keyword("clobberlhe")
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("decaymode", self.allowed_decay_modes[2], name="DM_model", description="dark matter model to use: [{}]".format("; ".join(self.allowed_decay_modes)))
        self.add_keyword("DMgDM")
        self.add_keyword("DMgSM")
        self.add_keyword("DMLambda")
        self.add_keyword("DMmass")
        self.add_keyword("DMphimass")
        self.add_keyword("DMphiwidth")
        self.add_keyword("doublefsr", 1)
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi")
        self.add_keyword("foldphi")
        self.add_keyword("foldy")
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp")
        self.add_keyword("hfact")
        self.add_keyword("idDM")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 5)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 5)
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("mass_high")
        self.add_keyword("mass_low")
        self.add_keyword("masswindow_high")
        self.add_keyword("masswindow_low")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 60000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 400000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscale", 3, description="Choice for mu_R and mu_F in Bbar integration. [0:2mX (fixed); 1:pTj; 2:XXbar invariant mass; 3:Ht/2]")
        self.add_keyword("runningwidth")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tmass")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

    def validate_decays(self):
        """! Validate decaymode keyword."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.DM_model, self.allowed_decay_modes)
        __decay_mode_lookup = {"scalar EFT": 1, "pseudo-scalar EFT": 2, "scalar full theory": -1, "pseudo-scalar full theory": -2}
        self.parameters_by_keyword("decaymode")[0].value = __decay_mode_lookup[self.DM_model]
