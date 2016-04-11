import history
import dcp
import occurrence
from terrama2 import *

class Buffer:
    OUTSIDE = 1
    INSIDE = 2
    OUTSIDE_INSIDE = 3
    OUTSIDE_AND_GEOMETRY = 4
    GEOMETRY_WITHOUT_INSIDE = 5

class Statistic:
    MIN = 1
    MAX = 2
    SUM = 3
    MEAN = 4
    MEDIAN = 5
    STANDARD_DEVIATION = 6
