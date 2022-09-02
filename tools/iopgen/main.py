import json
from pathlib import Path


import core.sv_config
from core.model.distribution.distribution import SVDistribution
from core.model.distribution.uniform import SVUniformDistribution
from core.model.tsl.extension import TslExtension
from utils.file_template import load_template_from_file
from utils.yaml_utils import yaml_load

# from pathlib import Path
#
from jinja2 import Environment, meta, Template, FileSystemLoader
#
# from core.model.distribution.uniform import SVUniformDistribution
# from core.model.tsl.extension import TslExtension
# from utils.file_template import load_template_from_file
# from utils.yaml_utils import yaml_load
#

def derefence(d, l) -> str:
    result = ""
    if d is not None:
        for var in d:
            print(f"{var} = *(m_sources[{d[var]['pos']}]+{d[var]['offset']});")
    for idx in range(len(l)):
        el = l[idx]
        if isinstance(el, dict):
            print(f"buf[{idx}] = *(m_sources[{el['pos']}]+{el['offset']});")
        else:
            print(f"buf[{idx}] = {el};")

def dereference1(l) -> None:
    for el in l:
        if el["redundant_access"]:
            for assignment in el["assignments"]:
                print(
                    f"{assignment['variable_name']} = *(m_source[{el['dataptr_container_pos']}]+{assignment['offset']});")
    global_pos: int = 0
    for el in l:
        if el["redundant_access"]:
            for var in el['access']:
                print(f"m_buf[{global_pos}] = {var};")
                global_pos += 1
        else:
            for offset in el["offsets"]:
                print(f"m_buf[{global_pos}] = *(m_source[{el['dataptr_container_pos']}] + {offset});")
                global_pos += 1

def test(dist):
    print("============================================================================================================")
    print(f"{pipeline_ops} - {column_count}")
    print(f"Parameter-List  : {dist.ctor_parameters_list}")
    # print(f"Initialization  : {dist.ctor_dataptr_container_initialization_list}")
    # print(f"Container Values: {dist.dataptr_container_values_dict}")
    lanes_per_pipeline_operator = int(dist._tsl_extension.lanes_in_simd_reg_count/dist._number_of_pipeline_ops)
    print(f"Lanes per POp   : {lanes_per_pipeline_operator}")
    lanes_per_column = int(dist._tsl_extension.lanes_in_simd_reg_count/dist._number_of_columns)
    print(f"Lanes per Column: {lanes_per_column}")

    def create_position_dict_entry(dataptr_idx) -> dict:
        def variable_name(idx: int, offset: int) -> str:
            return f"tmp_{idx}_{offset}"

        result: dict = {
            "dataptr_container_pos": dataptr_idx,
            "increment": lanes_per_column
        }
        offsets = [inc % lanes_per_pipeline_operator for inc in range(lanes_per_column)]
        offsets_set = sorted(list(set(offsets)))
        if len(offsets_set) == lanes_per_column:
            result["redundant_access"] = False
            result["offsets"] = offsets
        else:
            result["redundant_access"] = True
            result["assignments"] = []
            for offset in offsets_set:
                result["assignments"].append(
                    {
                        "variable_name": variable_name(dataptr_idx,offset),
                        "offset": offset
                    }
                )

            result["access"] = [variable_name(dataptr_idx,offset) for offset in offsets]
        return result

    data = [create_position_dict_entry(dataptr_idx) for dataptr_idx in range(dist._number_of_columns)]
    print(data)
    dereference1(data)
    print("------------------------------------------------------------------------------------------------------------")


if __name__ == '__main__':
    extension = [TslExtension(extension) for extension in
                  yaml_load(Path("../../libs/tsl/avail_extensions.yaml"))['generated_extensions']][0]
    # pip_cols = {2**i: [2**j for j in range(i+1)] for i in range(4)}
    # pip_cols = {i: [j for j in range(1,i + 1)] for i in range(1,9)}
    pip_cols = {3:[3]}
    for pipeline_ops, columns in pip_cols.items():
        for column_count in columns:

            dist = SVUniformDistribution(extension, pipeline_ops, column_count)
            # print(dist.as_dict())
            # print(json.dumps(dist.as_dict(), sort_keys=True, indent=4))

    # d = {
    #     "extension": extension.as_dict(),
    #     "distributions": [dist for dist in SVDistribution.create(SVUniformDistribution, extension)]
    # }
    # print(json.dumps(d, indent=4))
    x = SVDistribution.create(SVUniformDistribution, extension)
    # print(json.dumps(x, indent=4))


#
    env = Environment(trim_blocks=True, lstrip_blocks=True, loader=FileSystemLoader('./model/templates/'), extensions=['jinja2.ext.do'])
    template = load_template_from_file(Path("model/templates/input/load.t2"))
    print(env.from_string(template).render(x))
    #
    ## for extension in extensions:
    # for extension in [extensions[0]]:
    #     d = {
    #         "distribution_name": SVUniformDistribution.distribution_name(),
    #         "extension": extension.as_dict(),
    #         "iterators": [SVUniformDistribution(extension, number_of_pipeline_ops, number_of_columns).as_dict() for
    #                       number_of_pipeline_ops in
    #                       range(2, 3) for number_of_columns in range(2, 3)]
    ##                       range(1, extension.lanes_in_simd_reg_count + 1) for number_of_columns in
    ##                       range(1, number_of_pipeline_ops + 1)]
        # }
        # print(env.from_string(template).render(d))
#