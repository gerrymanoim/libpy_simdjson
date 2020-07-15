import ast
import glob
import os
import sys

from libpy.build import LibpyExtension
from setuptools import find_packages, setup

if ast.literal_eval(os.environ.get("LIBPY_SIMDJSON_DEBUG_BUILD", "0")):
    optlevel = 0
    debug_symbols = True
    max_errors = 5
else:
    optlevel = 3
    debug_symbols = False
    max_errors = None


def extension(*args, **kwargs):
    extra_compile_args = ["-DLIBPY_AUTOCLASS_UNSAFE_API"]
    if sys.platform == "darwin":
        extra_compile_args.append("-mmacosx-version-min=10.15")

    return LibpyExtension(
        *args,
        optlevel=optlevel,
        debug_symbols=debug_symbols,
        werror=True,
        max_errors=max_errors,
        include_dirs=(
            [".", "submodules/range-v3/include/"] +
            kwargs.pop("include_dirs", [])
        ),
        extra_compile_args=extra_compile_args,
        depends=glob.glob("**/*.h", recursive=True),
        **kwargs
    )


install_requires = [
    "setuptools",
    "libpy",
]

setup(
    name="libpy_simdjson",
    version="0.1.0",
    description="Python bindings for smidjson, using libpy",
    author="Gerry Manoim",
    packages=find_packages(),
    install_requires=install_requires,
    extras_require={
        "test": ["pytest"],
        "benchmark": [
            "pytest-benchmark",
            "orjson",
            "python-rapidjson",
            "pysimdjson",
            "ujson",
        ],
    },
    ext_modules=[
        extension(
            "libpy_simdjson.parser",
            ["libpy_simdjson/parser.cc", "libpy_simdjson/simdjson.cpp"],
        ),
    ],
)
