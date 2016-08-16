import sys
import locale
import re
import os
import glob
import numpy as np

class ReadState:
    FIND_SECTION = 1
    FIND_KEY = 2
    FIND_VAR = 3
    state = FIND_SECTION

class DimensionType:
    LINEAR = 1
    LEVELS = 2
    GAUST62 = 3
    GAUSR15 = 4
    GAUSR20 = 5
    GAUSR30 = 6
    GAUSR40 = 7

class ValueDef:
    numValues = 0
    dimension_type = None
    values = []

class TValueDef:
    numValues = 0
    dimension_type = None
    values = []

class Var:
    varName = ""
    vertical_levels = 0
    additional_code = ""
    units = ""
    description = ""

class CHSUB:
    t1 = 0
    t2 = 0
    str = ""



class CTLDescriptor:
    ctl_filename = ""
    dataset_filename = ""
    title = ""
    vec_options = []
    vec_CHSub = []
    undef = 0
    file_header_length = 0
    x_def = None
    y_def = None
    z_def = None
    t_def = None
    vars = []

    def addVar(self, str_var):
        var = Var()
        tokens = split(str_var)
        if not tokens:
            return

        if len(tokens) >= 4:
            var.varName = tokens[0]
            var.vertical_levels = tokens[1]
            var.units = tokens[2]

            # Description may have spaces so we need to concatenate the remaining tokens
            for t in tokens[3:]:
                if var.description:
                    var.description += " "
                var.description += t
        else:
            raise Exception("Wrong number of fields in VAR configuration, expected at least 4")

    def set_key_value(self, key, value):
        found = False
        if key == "DSET":
            self.dataset_filename = value
            found = True
        elif key == "TITLE":
            self.title = value
            found = True
        elif key == "UNDEF":
            value.strip()
            if value:
                self.undef = float(value)
            found = True
        elif key == "OPTIONS":
            self.vec_options = split(value)
            found = len(self.vec_options) > 0
        elif key == "XDEF":
            self.x_def = self.get_value_def(value, "X")
            if self.x_def:
                found = True
        elif key == "YDEF":
            self.y_def = self.get_value_def(value, "Y")
            if self.y_def:
                found = True
        elif key == "ZDEF":
            self.z_def = self.get_value_def(value, "Z")
            if self.z_def:
                found = True
        elif key == "TDEF":
            self.t_def = self.get_t_value_def(value)
            if self.t_def:
                found = True

        if not found:
            raise Exception("Could not find any known key/value")

    def get_value_def(self, value, dimension):
        value_def = ValueDef()

        tokens = split(value)
        if len(tokens) < 3:
            raise Exception("Wrong number of fields in " + dimension + "DEF configuration, expected at least 3")

        if tokens[0]:
            value_def.numValues = int(tokens[0])
        if tokens[1] == "LINEAR":
            value_def.dimensionType_ = DimensionType.LINEAR
        elif tokens[1] == "LEVELS":
            value_def.dimensionType_ = DimensionType.LEVELS
        elif tokens[1] == "GAUST62" and dimension == "Y":
            value_def.dimensionType_ = DimensionType.GAUST62
        elif tokens[1] == "GAUSR15" and dimension == "Y":
            value_def.dimensionType_ = DimensionType.GAUSR15
        elif tokens[1] == "GAUSR20" and dimension == "Y":
            value_def.dimensionType_ = DimensionType.GAUSR20
        elif tokens[1] == "GAUSR30" and dimension == "Y":
            value_def.dimensionType_ = DimensionType.GAUSR30
        elif tokens[1] == "GAUSR40" and dimension == "Y":
            value_def.dimensionType_ = DimensionType.GAUSR40
        else:
            raise Exception("Invalid mapping method for " + dimension + "DEF")

        value_def.values = []
        for t in tokens[2:]:
            t.strip()
            if t:
                value_def.values.append(float(t))

        return value_def

    def get_t_value_def(self, value):
        value_def = TValueDef()

        tokens = split(value)
        if len(tokens) < 3:
            raise Exception("Wrong number of fields in TDEF configuration, expected at least 3")

        value_def.numValues = int(tokens[0].strip())
        if tokens[1] == "LINEAR":
            value_def.dimensionType_ = DimensionType.LINEAR
        elif tokens[1] == "LEVELS":
            value_def.dimensionType_ = DimensionType.LEVELS
        else:
            raise Exception("Invalid mapping method for TDEF")

        for t in tokens[2:]:
            value_def.values.append(t)

        return value_def

def split(value):
    if not value:
        return None
    value = re.sub("\s\s+", " ", value.strip())
    return value.split(' ')

def read_param(line):
    if not line:
        return None

    line.strip()
    key = ""
    value = ""
    pos = line.find(" ")
    if pos != -1:
        key = line[0:pos]
        value = line[pos + 1:]
    else:
        key = line

    return key, value


def read_ctl_file(filename):

    # float numbers in the ctl file are using dot as separator
    locale.setlocale(locale.LC_ALL, "en_US.utf-8")

    with open(filename, 'r') as f:
        state = ReadState.FIND_SECTION
        ctl_descriptor = CTLDescriptor()
        ctl_descriptor.ctl_filename = filename

        line = f.readline()
        line = line.strip()

        if line.startswith("VARS"):
            section = "VARS"
            state = ReadState.FIND_VAR
        else:
            state = ReadState.FIND_KEY

        while line:
            if state == ReadState.FIND_SECTION:
                line = f.readline()
                line = line.strip()

                param = read_param(line)
                if not param:
                    break
                elif param[0] == "VARS":
                    section = "VARS"
                    state = ReadState.FIND_VAR
                elif param[0] == "ENDVARS":
                    section = "VARS"
                    state = ReadState.FIND_SECTION
                    continue
                else:
                    section = ""
                    state = ReadState.FIND_KEY

            if state == ReadState.FIND_KEY:
                if not line:
                    state = ReadState.FIND_SECTION
                    continue

                param = read_param(line)
                if not param:
                    break

                ctl_descriptor.set_key_value(param[0], param[1])
                state = ReadState.FIND_SECTION
                continue
            if state == ReadState.FIND_VAR:

                line = f.readline()
                line = line.strip()

                if not line:
                    continue

                if line == "ENDVARS":
                    state = ReadState.FIND_SECTION
                    continue

                ctl_descriptor.addVar(line)

    return ctl_descriptor


def replace_date_pattern(filename):
    # Year 2 digits
    filename = filename.replace("%y2", "[0-9][0-9]")
    # Year 4 digits
    filename = filename.replace("%y4", "[0-9][0-9][0-9][0-9]")
    # Month 2 digits
    filename = filename.replace("%m2", "[0-9][0-9]")
    # Days 2 digits
    filename = filename.replace("%d2", "[0-9][0-9]")
    # Hours 2 digits
    filename = filename.replace("%h2", "[0-9][0-9]")
    # Minutes 2 digits
    filename = filename.replace("%n2", "[0-9][0-9]")

    # TODO: implement all templates accepted by GrADS CTL

    return filename

def write_hdr(filename, ctl_descriptor):
    filename = filename.replace("bin", "hdr")
    with open(filename, 'w') as file:
        txt = '''ENVI
header offset = 0
file type = ENVI Standard
data type = 4
interleave = bsq
sensor type = Unknown
byte order = 0
coordinate system string = {GEOGCS["GCS_WGS_1984",DATUM["D_WGS_1984",SPHEROID["WGS_1984",6378137.0,298.257223563]],PRIMEM["Greenwich",0.0],UNIT["Degree",0.0174532925199433]]}
wavelength units = Unknown
'''
        txt += "samples = " + str(ctl_descriptor.x_def.numValues) + "\n"
        txt += "lines = " + str(ctl_descriptor.y_def.numValues) + "\n"
        txt += "bands = " + str(ctl_descriptor.z_def.numValues) + "\n"
        txt += "map info = {Geographic Lat/Lon, 1.5000, 1.5000, "
        txt += str(ctl_descriptor.x_def.values[0]) + ", "
        txt += str(ctl_descriptor.y_def.values[0]) + ", "
        txt += str(ctl_descriptor.x_def.values[1]) + ", "
        txt += str(ctl_descriptor.y_def.values[1]) + ", "
        txt += "WGS-84, units=Degrees}\n"
        txt += "description = {" + ctl_descriptor.title + " }"
        file.write(txt)

def write_env(filename, ctl_descriptor):
    arr_data = np.fromfile(file=filename, dtype=np.float32, count=-1)
    filename = filename = filename.replace("bin", "env")

    arr_data.resize(ctl_descriptor.y_def.numValues, ctl_descriptor.x_def.numValues)

    if "yrev" in ctl_descriptor.vec_options:
        arr_yrev = np.rot90(arr_data, 3)
        arr_yrev = np.transpose(arr_yrev)

        arr_result = arr_yrev.astype(np.float32)
        arr_result.tofile(file=filename)
    else:
        arr_data.tofile(file=filename)

def create_files(ctl_descriptor):
    base_dir = ""
    pattern = ""
    if(ctl_descriptor.dataset_filename.startswith("^")):
        base_dir = os.path.dirname(ctl_descriptor.ctl_filename)
        pattern = ctl_descriptor.dataset_filename[1:]
    else:
        base_dir, pattern = os.path.split(ctl_descriptor.dataset_filename)

    pattern = replace_date_pattern(pattern)

    found = False
    for name in glob.glob(os.path.join(base_dir, pattern)):
        write_hdr(name, ctl_descriptor)
        write_env(name, ctl_descriptor)
        found = True

    if not found:
        raise Exception("Could not find a file for the given dataset")


try:
    ctl_descriptor = read_ctl_file(sys.argv[1])
    create_files(ctl_descriptor)
except Exception as e:
    print e.message
