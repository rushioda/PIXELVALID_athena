# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..powheg_V2 import PowhegV2
import itertools

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ssWWjj(PowhegV2):
    """! Default Powheg configuration for same-sign W-boson pair plus jet pair production.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(ssWWjj, self).__init__(base_directory, "Wp_Wp_J_J", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        ## List of allowed decay modes
        self.allowed_decay_modes = ["w- w- > {} {}".format(*x) for x in itertools.product(["e- ve~", "mu- vm~", "tau- vt~", "emu- vemu~", "l- vl~"], repeat=2)] +\
                                   ["w+ w+ > {} {}".format(*x) for x in itertools.product(["e+ ve", "mu+ vm", "tau+ vt", "emu+ vemu", "l+ vl"], repeat=2)]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("alphas_from_lhapdf")
        self.add_keyword("bornktmin", 20.0)
        self.add_keyword("bornonly")
        self.add_keyword("bornsuppfact", 100)
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
        self.add_keyword("colltest")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fakevirt")
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
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1")
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2")
        self.add_keyword("itmx2rm")
        self.add_keyword("iupperfsr")
        self.add_keyword("iupperisr")
        self.add_keyword("iymax")
        self.add_keyword("lhans1", self.default_PDFs)
        self.add_keyword("lhans2", self.default_PDFs)
        self.add_keyword("lhapdf6maxsets")
        self.add_keyword("lhrwgt_descr")
        self.add_keyword("lhrwgt_group_combine")
        self.add_keyword("lhrwgt_group_name")
        self.add_keyword("lhrwgt_id")
        self.add_keyword("LOevents")
        self.add_keyword("manyseeds")
        self.add_keyword("max_io_bufsize")
        self.add_keyword("maxseeds")
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("ncall1", 5000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 5000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("noevents")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 5000)
        self.add_keyword("olddij")
        self.add_keyword("par_2gsupp")
        self.add_keyword("par_diexp")
        self.add_keyword("par_dijexp")
        self.add_keyword("parallelstage")
        self.add_keyword("pdfreweight")
        self.add_keyword("ptsqmin")
        self.add_keyword("ptsupp")
        self.add_keyword("radregion")
        self.add_keyword("rand1")
        self.add_keyword("rand2")
        self.add_keyword("renscfact", self.default_scales[1])
        self.add_keyword("runningscales")
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("smartsig")
        self.add_keyword("smincuts")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("vdecaymodeW1", self.allowed_decay_modes[0], name="decay_mode")
        self.add_keyword("vdecaymodeW2", hidden=True)
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 2)
        self.add_keyword("zerowidth")

    def validate_decays(self):
        """! Validate idvecbos, vdecaymodew1 and vdecaymodew2 keywords."""
        self.expose()  # convenience call to simplify syntax
        if self.decay_mode not in self.allowed_decay_modes:
            logger.fatal("Decay mode {} not recognised!".format(self.decay_mode))
            raise ValueError("Decay mode {} not recognised!".format(self.decay_mode))
        # Calculate appropriate decay mode numbers
        __decay_products = [p for p in self.decay_mode.split("> ")[1].split(" ")]
        __decay_mode_lookup = {"e- ve~": 11, "mu- vm~": 13, "tau- vt~": 15, "emu- vemu~": 113, "l- vl~": 135,
                               "e+ ve": -11, "mu+ vm": -13, "tau+ vt": -15, "emu+ vemu": -113, "l+ vl": -135}
        self.parameters_by_keyword("vdecaymodeW1")[0].value = __decay_mode_lookup[" ".join(__decay_products[0:2])]
        self.parameters_by_keyword("vdecaymodeW2")[0].value = __decay_mode_lookup[" ".join(__decay_products[2:4])]
