from __future__ import annotations
import abc
from typing import List, Any, Dict, Union, Generator

from core.model.tsl.extension import TslExtension


class SVDistribution:
    data_ptr_t = "data_ptr_t"
    parameter_common_name = "data"
    data_ptr_container_name = "m_sources"

    def __init__(self, tsl_extension: TslExtension, number_of_pipeline_ops: int, number_of_columns: int) -> None:
        self._number_of_pipeline_ops: int = number_of_pipeline_ops
        self._number_of_columns: int = number_of_columns
        self._tsl_extension: TslExtension = tsl_extension
        self._base_type: str = tsl_extension.data_type

    @classmethod
    def _create_variable_name(cls, postfix: Any) -> str:
        return f"{SVDistribution.parameter_common_name}_{postfix}"

    @property
    def parameter_list(self) -> List[str]:
        return [SVDistribution._create_variable_name(idx) for idx in range(self._number_of_columns)]

    @property
    def ctor_parameters_list(self) -> List[str]:
        return [f"{SVDistribution.data_ptr_t} {param}" for param in self.parameter_list]


    def increment_instructions_list(self) -> List[dict]:
        raise NotImplementedError

    def access_instructions_list(self) -> List[dict]:
        raise NotImplementedError

    def distribution_description(self) -> List[str]:
        raise NotImplementedError

    @classmethod
    def distribution_name(cls) -> str:
        raise NotImplementedError

    def _as_dict(self) -> Dict[str, Union[str, int, List[str]]]:
        return {
            "number_of_pipeline_ops": self._number_of_pipeline_ops,
            "number_of_columns": self._number_of_columns,
            "dataptr_t": SVDistribution.data_ptr_t,
            "dataptr_container_name": SVDistribution.data_ptr_container_name,
            "ctor_parameter_list": self.ctor_parameters_list,
            "ctor_dataptr_container_inititialization_list": self.parameter_list,
            "distribution_description": self.distribution_description(),
            "access_instructions_list": self.access_instructions_list(),
            "increment_instructions_list": self.increment_instructions_list()
        }

    @classmethod
    def create(cls, distribution_class, tsl_extension: TslExtension) -> dict:
        return {
            "distribution_name": distribution_class.distribution_name(),
            "extension": tsl_extension.as_dict(),
            "distribution_variants": [distribution_class(tsl_extension, number_of_pipeline_ops, number_of_columns)._as_dict() for number_of_pipeline_ops in range(1, tsl_extension.lanes_in_simd_reg_count+1) for number_of_columns in range(1, number_of_pipeline_ops+1)]
        }