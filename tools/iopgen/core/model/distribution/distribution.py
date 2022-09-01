import abc
from typing import List, Any, Dict, Union

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

    def _dataptr_container_access(self, idx: int) -> str:
        return f"{SVDistribution.data_ptr_container_name}[{idx}]"

    @property
    def _parameter_list(self) -> List[str]:
        return [SVDistribution._create_variable_name(idx) for idx in range(self._number_of_columns)]

    @property
    def ctor_parameters(self) -> List[str]:
        return [f"{SVDistribution.data_ptr_t} {param}" for param in self._parameter_list]

    def _dataptr_container_increment(self, idx: int, inc: int) -> str:
        if inc == 1:
            return f"++{self._dataptr_container_access(idx)}"
        return f"{self._dataptr_container_access(idx)} += {inc}"

    def unique_data_ptr_count(self) -> int:
        raise NotImplementedError

    def ctor_dataptr_container_initialization_list(self) -> List[str]:
        raise NotImplementedError

    def dataptr_container_max_increment(self) -> int:
        raise NotImplementedError

    def dataptr_container_increment(self) -> List[str]:
        raise NotImplementedError

    def distribution_description(self) -> List[str]:
        raise NotImplementedError

    def dataptr_dereference(self) -> List[str]:
        raise NotImplementedError

    def as_dict(self) -> Dict[str, Union[str, int, List[str]]]:
        return {
            "number_of_pipeline_ops": self._number_of_pipeline_ops,
            "number_of_columns": self._number_of_columns,
            "dataptr_t": SVDistribution.data_ptr_t,
            "dataptr_container_name": SVDistribution.data_ptr_container_name,
            "unique_data_ptr_count": self.unique_data_ptr_count(),
            "ctor_parameter_list": self.ctor_parameters,
            "ctor_dataptr_container_inititialization_list": self.ctor_dataptr_container_initialization_list(),
            "dataptr_container_max_increment": self.dataptr_container_max_increment(),
            "dataptr_container_increment_ops_list": self.dataptr_container_increment(),
            "distribution_description": self.distribution_description(),
            "dataptr_dereference_list": self.dataptr_dereference()
        }