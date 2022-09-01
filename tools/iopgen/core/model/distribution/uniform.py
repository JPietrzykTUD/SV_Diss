from typing import List

from core.model.distribution.distribution import SVDistribution
from core.model.tsl.extension import TslExtension


class SVUniformDistribution(SVDistribution):
    def __init__(self, tsl_extension: TslExtension, number_of_pipeline_ops: int, number_of_columns: int):
        super().__init__(tsl_extension, number_of_pipeline_ops, number_of_columns)
        self.__lanes_per_pipeline_operator = int(self._tsl_extension.lanes_in_simd_reg_count/number_of_pipeline_ops)

    def unique_data_ptr_count(self) -> int:
        if self._number_of_pipeline_ops == 1:
            return 1
        return int(self.__lanes_per_pipeline_operator * self._number_of_columns)

    def dataptr_container_max_increment(self) -> int:
        if self._number_of_pipeline_ops == 1:
            return 1
        return int(self.unique_data_ptr_count() / self._number_of_columns)

    def ctor_dataptr_container_initialization_list(self) -> List[str]:
        if self._number_of_pipeline_ops == 1:
            return self._parameter_list
        DEBUGDATAPTR_CONTAINER_MAX_INCREMENT = self.dataptr_container_max_increment()
        DEBUGRESULT = [f"{param} + {inc}" for param in self._parameter_list for inc in
                range(self.dataptr_container_max_increment())]
        return [f"{param} + {inc}" for param in self._parameter_list for inc in
                range(self.dataptr_container_max_increment())]

    def dataptr_container_increment(self) -> List[str]:
        if self._number_of_pipeline_ops == 1:
            return [self._dataptr_container_increment(0,1)]
        return [self._dataptr_container_increment(idx, self.dataptr_container_max_increment()) for idx in
                range(self.unique_data_ptr_count())]

    def distribution_description(self) -> str:
        return "todo"

    @classmethod
    def distribution_name(cls) -> str:
        return "uniform"

    def dataptr_dereference(self) -> List[str]:
        number_of_unique_data_ptr = self.unique_data_ptr_count()
        rep_count: int = (int(self._tsl_extension.lanes_in_simd_reg_count/self.unique_data_ptr_count()))
        remainder: int = self._tsl_extension.lanes_in_simd_reg_count - (rep_count*number_of_unique_data_ptr)
        result: List[str] = [f"*{self._dataptr_container_access(idx)}" for idx in range(self.unique_data_ptr_count())]*rep_count
        result.extend(["0"]*remainder)
        return result

