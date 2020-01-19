# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ..powheg_V2 import PowhegV2
import itertools

class tt_NLOdecays(PowhegV2):
    """! Default Powheg configuration for top pair production, with NLO corrections to the decays.

    Create a configurable object with all applicable Powheg options.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, **kwargs):
        """! Constructor: all process options are set here.

        @param base_directory: path to PowhegBox code.
        @param kwargs          dictionary of arguments from Generate_tf.
        """
        super(tt_NLOdecays, self).__init__(base_directory, "ttb_NLO_dec", **kwargs)

        # Add parameter validation functions
        self.validation_functions.append("validate_decays")

        # List of t and t~ decay modes
        __tp_decays = ["t > b e+ ve", "t > b mu+ vm", "t > b tau+ vt", "t > b u d~", "t > b c s~"]
        __tm_decays = ["t~ > b~ e- ve~", "t~ > b~ mu- vm~", "t~ > b~ tau- vt~", "t~ > b~ u~ d", "t~ > b~ c~ s"]

        ## List of allowed decay modes
        self.allowed_decay_modes = ["t t~ > {} {}".format(tp.split("> ")[1], tm.split("> ")[1]) for tp, tm in itertools.product(__tp_decays, __tm_decays)]

        # Add all keywords for this process, overriding defaults if required
        self.add_keyword("allrad", 1)
        self.add_keyword("alphas_from_lhapdf")
        self.add_keyword("altmap")
        self.add_keyword("arXiv:1207.5018")
        self.add_keyword("bmass")
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
        self.add_keyword("colltest")
        self.add_keyword("complexmass")
        self.add_keyword("compress_lhe")
        self.add_keyword("compress_upb")
        self.add_keyword("compute_rwgt")
        self.add_keyword("doublefsr")
        self.add_keyword("doubleresonant")
        self.add_keyword("evenmaxrat")
        self.add_keyword("facscfact", self.default_scales[0])
        self.add_keyword("fastbtlbound")
        self.add_keyword("fixedgrid")
        self.add_keyword("fixedscale")
        self.add_keyword("flg_debug")
        self.add_keyword("foldcsi")
        self.add_keyword("foldphi")
        self.add_keyword("foldy")
        self.add_keyword("fullrwgt")
        self.add_keyword("fullrwgtmode")
        self.add_keyword("hdamp", 175)
        self.add_keyword("hfact")
        self.add_keyword("icsimax")
        self.add_keyword("ih1")
        self.add_keyword("ih2")
        self.add_keyword("itmx1", 12)
        self.add_keyword("itmx1rm")
        self.add_keyword("itmx2", 6)
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
        self.add_keyword("mglowidth")
        self.add_keyword("minlo")
        self.add_keyword("mintupbratlim")
        self.add_keyword("mintupbxless")
        self.add_keyword("mockoffshelltop")
        self.add_keyword("ncall1", 1000000)
        self.add_keyword("ncall1rm")
        self.add_keyword("ncall2", 200000)
        self.add_keyword("ncall2rm")
        self.add_keyword("ncallfrominput")
        self.add_keyword("nlowhich")
        self.add_keyword("noevents")
        self.add_keyword("nospincorr")
        self.add_keyword("novirtual")
        self.add_keyword("nubound", 200000)
        self.add_keyword("offshellscales")
        self.add_keyword("offshelltop")
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
        self.add_keyword("rwl_add")
        self.add_keyword("rwl_file")
        self.add_keyword("rwl_format_rwgt")
        self.add_keyword("rwl_group_events")
        self.add_keyword("smartsig")
        self.add_keyword("softtest")
        self.add_keyword("stage2init")
        self.add_keyword("storeinfo_rwgt")
        self.add_keyword("storemintupb")
        self.add_keyword("tbardec", hidden=True)
        self.add_keyword("testplots")
        self.add_keyword("testsuda")
        self.add_keyword("tmass")
        self.add_keyword("topdec", self.allowed_decay_modes[0], name="decay_mode")
        self.add_keyword("ubexcess_correct")
        self.add_keyword("ubsigmadetails")
        self.add_keyword("use-old-grid")
        self.add_keyword("use-old-ubound")
        self.add_keyword("withdamp")
        self.add_keyword("withnegweights")
        self.add_keyword("withsubtr")
        self.add_keyword("xgriditeration")
        self.add_keyword("xupbound", 5)

    def validate_decays(self):
        """! Validate topdec and tbardec keywords."""
        self.expose()  # convenience call to simplify syntax
        self.check_decay_mode(self.decay_mode, self.allowed_decay_modes)
        # Calculate appropriate decay mode numbers
        __decay_products = map(str.strip, self.decay_mode.replace("~", "").replace("+", "").replace("-", "").split("b"))
        __decay_mode_lookup = {"e ve": 11, "mu vm": 13, "tau vt": 15, "u d": 1, "c s": 3}
        self.parameters_by_keyword("topdec")[0].value = __decay_mode_lookup[__decay_products[1]]
        self.parameters_by_keyword("tbardec")[0].value = __decay_mode_lookup[__decay_products[2]]
