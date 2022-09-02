from typing import List, Tuple, Dict

from core.model.distribution.distribution import SVDistribution
from core.model.tsl.extension import TslExtension


class SVUniformDistribution(SVDistribution):
    def __init__(self, tsl_extension: TslExtension, number_of_pipeline_ops: int, number_of_columns: int):
        super().__init__(tsl_extension, number_of_pipeline_ops, number_of_columns)
        self.__lanes_per_pipeline_operator = int(self._tsl_extension.lanes_in_simd_reg_count/number_of_pipeline_ops)
        self.__lanes_per_column = int(self._tsl_extension.lanes_in_simd_reg_count/number_of_columns)
        def position_dict_entry(dataptr_container_idx: int) -> dict:
            result: dict = {
                "dataptr_container_pos": dataptr_container_idx
            }
            offsets = [inc % self.__lanes_per_pipeline_operator for inc in range(self.__lanes_per_column)]
            offsets_set = sorted(list(set(offsets)))
            if len(offsets_set) == self.__lanes_per_column:
                result["redundant_access"] = False
                result["offsets"] = offsets
                # result["increment"] =
            else:
                result["redundant_access"] = True
                result["assignments"] = []
                for offset in offsets_set:
                    result["assignments"].append(
                        {
                            "variable_name": SVDistribution._create_variable_name(f"{dataptr_container_idx}_{offset}"),
                            "offset": offset
                        }
                    )
                result["access"] = [SVDistribution._create_variable_name(f"{dataptr_container_idx}_{offset}") for offset
                                    in offsets]
            return result

        self.__access_list: list = [position_dict_entry(dataptr_container_idx) for dataptr_container_idx in
                                               range(number_of_columns)]
    def increment_instructions_list(self) -> List[dict]:
        result: List[dict] = []
        for access_entry in self.__access_list:
            if access_entry["redundant_access"]:
                result.append({
                    "dataptr_container_pos": access_entry["dataptr_container_pos"],
                    "increment": len(access_entry["assignments"])
                })
            else:
                result.append({
                    "dataptr_container_pos": access_entry["dataptr_container_pos"],
                    "increment": len(access_entry["offsets"])
                })
        return result

    def access_instructions_list(self) -> List[dict]:
        return self.__access_list

    def distribution_description(self) -> str:
        return "todo"

    @classmethod
    def distribution_name(cls) -> str:
        return "uniform"


