import ast
import glob
import os

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
    return LibpyExtension(
        *args,
        optlevel=optlevel,
        debug_symbols=debug_symbols,
        werror=True,
        max_errors=max_errors,
        include_dirs=["."] + kwargs.pop("include_dirs", []),
        depends=glob.glob("**/*.h", recursive=True),
        **kwargs
    )


install_requires = [
    'setuptools',
    'libpy',
]

setup(
    name="libpy_simdjson",
    version="0.1.0",
    description="Python bindings for smidjson, using libpy",
    author="Gerry Manoim",
    packages=find_packages(),
    install_requires=install_requires,
    ext_modules=[
        extension(
            "libpy_simdjson.simdjson",
            ["libpy_simdjson/parser.cc", "libpy_simdjson/simdjson.cpp"],
            extra_compile_args=["-DLIBPY_AUTOCLASS_UNSAFE_API"],
        ),
    ],
)