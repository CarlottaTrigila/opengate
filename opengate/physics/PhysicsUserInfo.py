import opengate as gate
import opengate_core as g4
from box import Box


class PhysicsUserInfo:
    """
    This class is a simple structure that contains all user general options of ths physics list.

    """

    def __init__(self, simulation):
        # keep pointer to ref
        self.simulation = simulation

        # physics list and decay
        self.physics_list_name = None
        self.enable_decay = False

        # options related to the cuts and user limits
        self.production_cuts = Box()
        self.energy_range_min = None
        self.energy_range_max = None
        self.apply_cuts = None

        # Dictionary of particles for which the user
        # can decide whethr to apply user limits
        self.user_limits_particles = {}
        self.user_limits_particles["all"] = True
        self.user_limits_particles["gamma"] = False
        self.user_limits_particles["electron"] = False
        self.user_limits_particles["positron"] = False
        self.user_limits_particles["proton"] = False

        # special case for EM parameters -> G4 object
        # self.g4_em_parameters = g4.G4EmParameters.Instance()
        self.g4_em_parameters = None

    def __del__(self):
        pass

    def __str__(self):
        s = (
            f"{self.physics_list_name}"
            f"apply cuts : {self.apply_cuts}\n"
            f"prod cuts : {self.production_cuts}"
        )
        return s
