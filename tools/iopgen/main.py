from typing import Any, Dict, List

import core.sv_config
from pathlib import Path

from jinja2 import Environment, meta, Template, FileSystemLoader

from core.model.distribution.uniform import SVUniformDistribution
from core.model.tsl.extension import TslExtension
from utils.file_template import load_template_from_file
from utils.yaml_utils import yaml_load

if __name__ == '__main__':

    extensions = [TslExtension(extension) for extension in
                  yaml_load(Path("../../libs/tsl/avail_extensions.yaml"))['generated_extensions']]

    env = Environment(trim_blocks=True, lstrip_blocks=True, loader=FileSystemLoader('./model/templates/'), extensions=['jinja2.ext.do'])
    template = load_template_from_file(Path("model/templates/input/load.t2"))

    # for extension in extensions:
    for extension in [extensions[0]]:
        d = {
            "distribution_name": SVUniformDistribution.distribution_name(),
            "extension": extension.as_dict(),
            "iterators": [SVUniformDistribution(extension, number_of_pipeline_ops, number_of_columns).as_dict() for
                          number_of_pipeline_ops in
                          range(2, 3) for number_of_columns in range(2, 3)]
                          # range(1, extension.lanes_in_simd_reg_count + 1) for number_of_columns in
                          # range(1, number_of_pipeline_ops + 1)]
        }
        print(env.from_string(template).render(d))
