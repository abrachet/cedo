import json

constants = None
with open("./constants.json") as f:
    constants = json.load(f)


def emit_constant_struct(name, format):
    print(f"struct DW_{name} {{")
    for name in format:
        print(f"  {format[name]} {name};")

    print("  constexpr operator decltype(value)() const { return value; }")
    print("};\n")


def get_initialzer_list(value):
    name = list(value.keys())[0]

    args = "{"
    for v in value[name]:
        args += f"{v}, "
    args = args[:-2]
    args += "}"
    return args


def emit_constants_for_struct(type_name, value):
    assert len(value) == 1, f"malformed value '{value}'"

    name = list(value.keys())[0]
    print(f"constexpr DW_{type_name} DW_{type_name}_{name}{get_initialzer_list(value)};")


def emit_as_array(type_name):
    print(f"constexpr std::array DW_{type_name}_static_list{{")

    initialzer_list = ""
    for value in constants[type_name]["values"]:
        initialzer_list += f"  DW_{type_name}"
        initialzer_list += get_initialzer_list(value)
        initialzer_list += ",\n"

    initialzer_list = initialzer_list[:-2]
    initialzer_list += "\n};"
    print(initialzer_list)

    for i in range(len(constants[type_name]["values"])):
        name = [k for k in constants[type_name]["values"][i].keys()][0]
        print(f"constexpr DW_{type_name} DW_{type_name}_{name} = DW_{type_name}_static_list[{i}];")


def emit_get_from_value(type_name, value_type):
    print(f"constexpr DW_{type_name} get_DW_{type_name}(decltype(DW_{type_name}::value) value) {{")
    print(f"  for (const auto &a : DW_{type_name}_static_list)")
    print(f"    if (a.value == value) return a;")
    print(f'  assert(0 && "value was not part of DW_{type_name}_static_list");')
    print("}")


for constant in constants:
    emit_constant_struct(constant, constants[constant]["format"])
    if "genCreateFromValue" in constants[constant] and constants[constant]["genCreateFromValue"]:
        emit_as_array(constant)
        print("")
        emit_get_from_value(constant, constants[constant]["format"]["value"])
    else:
        for i in range(len(constants[constant]["values"])):
            emit_constants_for_struct(constant, constants[constant]["values"][i])
    print("")
