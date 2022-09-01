from typing import Union, Dict

from core.sv_config import config


class TslExtension:
    def __init__(self, tsl_extension_dict: dict) -> None:
        self.__simd_register_size: int = int(tsl_extension_dict['register_size'])
        self.__register_name: str = str(tsl_extension_dict['tsl_extension_name'])
        self.__data_type: str = str(tsl_extension_dict['data_type'])
        self.__data_type_size_b: int = int(config.data_types[self.__data_type]['size_b'])
        self.__lanes_in_simd_reg_count = int(self.__simd_register_size / self.__data_type_size_b)

    @property
    def simd_register_size(self) -> int:
        return self.__simd_register_size

    @property
    def register_name(self) -> str:
        return self.__register_name

    @property
    def data_type(self) -> str:
        return self.__data_type

    @property
    def data_type_size_b(self) -> int:
        return self.__data_type_size_b

    @property
    def lanes_in_simd_reg_count(self) -> int:
        return self.__lanes_in_simd_reg_count

    def as_dict(self) -> Dict[str, Union[str, int]]:
        return {
            "simd_register_size": self.simd_register_size,
            "register_name": self.register_name,
            "data_type": self.data_type,
            "data_type_size_b": self.data_type_size_b,
            "lanes_in_simd_reg_count": self.lanes_in_simd_reg_count
        }