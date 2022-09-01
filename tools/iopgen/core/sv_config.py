import argparse
from pathlib import Path
from typing import Generator, Dict

from jinja2 import Environment

from utils.dict_utils import dict_from_list
from utils.yaml_utils import yaml_load


class SVGeneratorConfig:
    class JinjaConfig:
        env = Environment(trim_blocks=True, lstrip_blocks=True)

    def __init__(self):
        self.__data_types = dict_from_list(yaml_load(Path("./model/data/cdata_types.yaml"))['types'], "alt_keys")

    @property
    def data_types(self) -> Dict[str, int]:
        return self.__data_types

config = SVGeneratorConfig()


def parse_args() -> dict:
    parser = argparse.ArgumentParser(description="SV Generator.")
    parser.add_argument("--extensions", nargs='*', dest='extensions')
    args = parser.parse_args()
    args_dict = dict()
    for key, value in vars(args).items():
        if value is not None:
            args_dict[key] = value
    return args_dict
