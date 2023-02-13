


"""
input: [current cadence, current gear, all gear ratios, slope]
settings: [
    prefered cadence(low,med,high),
    how much is "long", "medium", "short" distance/time
    ]


"""







def val_in_range(val, range_low, range_high):
    return val >= range_low and val <= range_high

gear_up_shift = "upshift ^^"
gear_down_shift = "downshift .."
gear_no_shift = "no shift"

class Current_State:
    speed = None
    cadence = None
    gear = None

    gears = {
        "front" : [32],
        "rear" : [51, 45, 39, 33, 28, 24, 21, 18, 15, 13, 11]
    }

    gear_ratios = []

    def __init__(self):
        for f in self.gears["front"]:
            for r in self.gears["rear"]:
                self.gear_ratios.append(f/r)
        print(self.gear_ratios)
        # input
        self.accel = 0.0 # km/h/s
        self.speed = 0.0 # km/h
        self.cadence = 0.0 # rpm
        self.gear = 6

        # constants
        self.__accel_up = 5
        self.__cadence_min = 80
        self.__cadence_max = 90

        # calc
        self.cadence_min = 80
        self.cadence_max = 90

    def calc_optimal_cadence(self):
        # whole logic is here
        self.cadence_min = self.__cadence_min + self.accel * self.__accel_up
        self.cadence_max = self.__cadence_max + self.accel * self.__accel_up

    def get_too_slow_cadence_range(self, cadence):
        return val_in_range(cadence, 0, self.cadence_min - 1)

    def get_optimal_cadence_range(self, cadence):
        return val_in_range(cadence, self.cadence_min, self.cadence_max)

    def get_too_fast_cadence_range(self, cadence):
        return val_in_range(cadence, self.cadence_max + 1, 200)

    def get_current_ratio(self):
        return self.gear_ratios[self.gear - 1]

    def get_next_ratio(self):
        if self.gear == len(self.gears["rear"]):
            return None
        return self.gear_ratios[self.gear]

    def get_prev_ratio(self):
        if self.gear == 1:
            return None
        return self.gear_ratios[self.gear - 2]

    def get_next_cadence(self):
        if self.gear == len(self.gears["rear"]):
            return None
        current_r = self.get_current_ratio()
        next_r = self.get_next_ratio()
        return next_r / current_r * self.cadence

    def get_prev_cadence(self):
        if self.gear == 1:
            return None
        current_r = self.get_current_ratio()
        prev_r = self.get_prev_ratio()
        return prev_r / current_r * self.cadence

    def get_suggested_gear(self):
        self.calc_optimal_cadence()
        if self.get_too_slow_cadence_range(self.cadence) and self.get_prev_ratio() is not None:
            return "downshift .."
        if self.get_too_fast_cadence_range(self.cadence)  \
            and self.get_next_cadence() is not None \
            and (self.get_optimal_cadence_range(self.get_next_cadence()) or self.get_too_fast_cadence_range(self.get_next_cadence())):
            return "upshift ^^"
        return "no shift"



current_state = Current_State()
current_state.speed = 25.0
current_state.cadence = 100.0
print(current_state.get_suggested_gear())

# print(current_state.get_prev_ratio())

print("TESTS:")
current_state.gear = 6
current_state.accel = 0.0

current_state.cadence = 70.0
gear_suggestion = current_state.get_suggested_gear()
assert gear_suggestion == gear_down_shift, gear_suggestion
print("PASSED")

current_state.cadence = 80.0
gear_suggestion = current_state.get_suggested_gear()
assert gear_suggestion == gear_no_shift, gear_suggestion
print("PASSED")

current_state.cadence = 100.0
gear_suggestion = current_state.get_suggested_gear()
print(current_state.get_next_cadence())
assert gear_suggestion == gear_up_shift, gear_suggestion
print("PASSED")

current_state.cadence = 91.0
gear_suggestion = current_state.get_suggested_gear()
print(current_state.get_next_cadence())
assert gear_suggestion == gear_up_shift, gear_suggestion
print("PASSED")


current_state.accel = 1.0
current_state.cadence = 91.0
gear_suggestion = current_state.get_suggested_gear()
print(current_state.get_next_cadence())
assert gear_suggestion == gear_no_shift, gear_suggestion
print("PASSED")
