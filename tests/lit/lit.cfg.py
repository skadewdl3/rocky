import os
import lit.formats

config.name = "rocky-lit"
config.test_format = lit.formats.ShTest(execute_external=False)
config.suffixes = [".rocky"]
config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = config.test_source_root

# Paths come from CMake: -Dparser=... -Drocky=... -Dfilecheck=...
parser = lit_config.params.get("parser", "parser-missing")
rocky = lit_config.params.get("rocky", "rocky-missing")
filecheck = lit_config.params.get("filecheck", "FileCheck")

# So test files can write %parser, %rocky, FileCheck
config.substitutions.append(("%parser", parser))
config.substitutions.append(("%rocky", rocky))
config.substitutions.append(("FileCheck", filecheck))

