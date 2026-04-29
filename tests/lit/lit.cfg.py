import os
import lit.formats

config.name = "rocky-lit"
config.test_format = lit.formats.ShTest(execute_external=True)
config.suffixes = [".rocky"]
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = config.test_source_root

parser = lit_config.params.get("parser", "parser-missing")
filecheck = lit_config.params.get("filecheck", "FileCheck")

config.substitutions.append(("%parser", parser))
config.substitutions.append(("FileCheck", filecheck))
