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


def emit_constants_for_struct(type_name, value):
    assert len(value) == 1, f"malformed value '{value}'"

    name = list(value.keys())[0]
    print(f"constexpr DW_{type_name} DW_{type_name}_{name}{{", end='')

    args = ""
    for v in value[name]:
        args += f"{v}, "
    args = args[:-2]
    args += "};"
    print(args)


for constant in constants:
    emit_constant_struct(constant, constants[constant]["format"])
    for i in range(len(constants[constant]["values"])):
        emit_constants_for_struct(constant, constants[constant]["values"][i])
    print("")

